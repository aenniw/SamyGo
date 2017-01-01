#include <SDL/SDL.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <netdb.h>

#include "externs.h"
#include "buttons.h"
#include "common.h"

#define AMBI_IP "192.168.43.30"
#define AMBI_PORT 65000
#define FF_MMAP_SIZE 0x800000
#define DP_INST0 0
#define DP_ON 1

#define get_a(w, h) mm_base[ ((h) * WIDTH + w) * 4 + 3 + offsetb ]
#define get_r(w, h) mm_base[ ((h) * WIDTH + w) * 4 + 2 + offsetb ]
#define get_g(w, h) mm_base[ ((h) * WIDTH + w) * 4 + 1 + offsetb ]
#define get_b(w, h) mm_base[ ((h) * WIDTH + w) * 4 + offsetb ]

#define sample_a(w, h, s) samples[s][3] += get_a(w,h)
#define sample_r(w, h, s) samples[s][2] += get_r(w,h)
#define sample_g(w, h, s) samples[s][1] += get_g(w,h)
#define sample_b(w, h, s) samples[s][0] += get_b(w,h)

#define sample_pixel(w, h, s) sample_r(w,h,s); sample_g(w,h,s); sample_b(w,h,s)
#define avg_pixel(s, v) color[2] = samples[s][2] / (v); color[3] = samples[s][1] / (v); color[4] = samples[s][0] / (v)
#define send_pixel(p, s) color[1] = p; sendto(sock, color, 5, 0, (struct sockaddr *) &serveraddr, sizeof(serveraddr))


static const unsigned int WIDTH = 1920;
static const unsigned int HEIGHT = 1080;
static const unsigned int WIDTH_SECTORS = 10;
static const unsigned int HEIGHT_SECTORS = 8;
volatile static int exit_routine = 0, active = 0;
static struct sockaddr_in serveraddr;

int fd = -1;
unsigned char *mm_base = NULL;

static int spIDp_DumpImage_samples(unsigned int hDp, int sock) {
    int activeVSize = 0;
    unsigned int dp_baddr;

    if (hDp == 0 || hDp == -1 || hDp <= 0x10000 || (*((unsigned int *) hDp) != 0x20050823)) {
        debug("_DVr(ERROR): Invalid handle value=0x%08X", hDp);
        return -1;
    }
    if (fd < 0) {
        uldSys_GetMemoryConfig(0x34, &dp_baddr);
        dp_baddr += 0x1FFFF;
        dp_baddr >>= 17;
        dp_baddr <<= 17;
        //debug("_DVr(DEBUG): alligned DP mem base: 0x%08X", dp_baddr);

        //lldDpTmgStillOn1(1);
        //OsaWait(0x11);
        modIncapt_GetActiveVSize(0, &activeVSize);
        //debug("_DVr(DEBUG): got ActiveVSize=%d\n", activeVSize);
        modIncapt_SetActiveVSize(0, 0);
        OsaWait(0x11);
        lldDp_Ve_Burst3216(1);
        lldDp_Ve_SetDumpAddress(dp_baddr);
        lldDp_Ve_CapPos(DP_ON);
        lldDp_Ve_WrCapOn(1);
        OsaWait(0x11);
        modIncapt_SetActiveVSize(0, activeVSize);


        fd = open("/dev/mem", O_RDONLY | O_SYNC);
        if (fd < 0) {
            debug("_DVr(ERROR): /dev/mem device open failed");
            return (-1);
        }
        if (mm_base == NULL)mm_base = mmap(0, FF_MMAP_SIZE, PROT_READ, MAP_SHARED, fd, dp_baddr);
        if (mm_base == (void *) -1) {
            debug("_DVr(ERROR): mmap() failed");
            close(fd);
            return -1;
        }
        debug("_DVr(DEBUG): Memory mapped at address %p %p.", mm_base, dp_baddr);
    }
    unsigned int *hDp_ptr = (unsigned int *) hDp;
    unsigned int *ptmpr2 = (unsigned int *) (*(hDp_ptr + 0x258 / 4));
    unsigned int *ptmpr1 = (unsigned int *) (*(hDp_ptr + 0x268 / 4));
    int mtmp2 = *(ptmpr2 + (0xC / 4));
    int mtmp1 = *(ptmpr1 + (8 / 4));
    int offsetb = mtmp1 * mtmp2 * 4;
    //debug("_DVr(DEBUG): offset1=%d offset2=%d bytes=%d", mtmp1, mtmp2, offsetb);

    const uint32_t sample_size = HEIGHT / 32, pixel_skip = 3,
            h_size = HEIGHT / HEIGHT_SECTORS, w_size = WIDTH / WIDTH_SECTORS,
            pixels_per_sector =
            ((HEIGHT / HEIGHT_SECTORS) / pixel_skip) * ((WIDTH / WIDTH_SECTORS) / pixel_skip);
    for (int s = 0; s < WIDTH_SECTORS; s++) {
        uint32_t samples[2][3] = {0};
        uint8_t color[5] = {0};
        for (int w = sample_size + w_size * s;
             w < WIDTH - sample_size && w < sample_size + w_size * (s + 1); w += pixel_skip) {
            for (int h = 0; h < sample_size; h += pixel_skip) {
                sample_pixel(w, h, 0);
                sample_pixel(w, h + HEIGHT - sample_size, 1);
            }
        }
        avg_pixel(0, pixels_per_sector);
        send_pixel(2 * WIDTH_SECTORS + HEIGHT_SECTORS - s, 0);
        avg_pixel(1, pixels_per_sector);
        send_pixel(s + 1, 1);
    }
    for (int s = 0; s < HEIGHT_SECTORS; s++) {
        uint32_t samples[2][3] = {0};
        uint8_t color[5] = {0};
        for (int h = sample_size + h_size * s;
             h < HEIGHT - sample_size && h < sample_size + h_size * (s + 1); h += pixel_skip) {
            for (int w = 0; w < sample_size; w += pixel_skip) { //LEFT
                sample_pixel(w, h, 0);
                sample_pixel(w + WIDTH - sample_size, h, 1);
            }
        }
        avg_pixel(0, pixels_per_sector);
        send_pixel(WIDTH_SECTORS + HEIGHT_SECTORS - s, 0);
        avg_pixel(1, pixels_per_sector);
        send_pixel(2 * WIDTH_SECTORS + HEIGHT_SECTORS + s + 1, 1);
    }
    return 0;
}

static int button_pressed(int key) __attribute__ ((noinline));

static int button_pressed(const int key) {
    debug("Button pressed 0x%04X", key);
    if (key == KEY_COL_GREEN) {
        active = 1;
        return KEY_NOTHING;
    }
    if (key == KEY_COL_RED) {
        active = 0;
        return KEY_NOTHING;
    }
    if (key == KEY_POWER) {
        exit_routine = 1;
    }
    return (key);        // return destination button code to be processed by exeDSP
}

void injection(int a, int key, int arg_r2) __attribute__ ((noinline));

void injection(int a, int key, int arg_r2) {
    // CAUTION! Do not add/modify code here... or modify injection indexes in loader

    // [0][1] gcc should put "STMFD SP!, {R4,LR}" and "MOV R4,[R1]" here
    asm volatile(
    "STMFD   SP!, {R0,R2,R3,R12}\n"        // [2] store some registers
    );

    // first, we have to execute our function
    button_pressed(key);                // [3][4] calling local function with key variable in R0, returning R0

    // then, prepare for calling and call previous injection function
    asm volatile(
    "MOV     R1, R0\n"            // [5] remapped keycode
            "MOV     R6, R0\n"            // [6] remapped keycode
            "LDMFD   SP!, {R0,R2,R3,R12}\n"        // [7] restore registers stored before
            "LDMFD   SP!, {R4,LR}\n"        // [8] restore registers...
            "MOV     R7, R2\n"            // [9] This is original command that we overwrite (and replace at next injection)
            "LDR     R3, [R8]\n"            // [10] This is original command that we overwrite (and replace at next injection)
            "LDR     PC, =(_ZN9KeyCommon17SendKeyPressInputEii + 0x18 )\n"    // [11] return after injection (not reached at next injection)
    );
    // and that's all; the rest (returning to exeDSP) is already programmed in the first injected function
}

void *ambi_routine(void *__attribute__ ((unused))) __attribute__ ((noinline));

void *ambi_routine(void *unused __attribute__ ((unused))) {
    debug("routine started");
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return NULL;
    fcntl(sock, F_SETFL, O_NONBLOCK | fcntl(sock, F_GETFL, 0));
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(AMBI_PORT);
    serveraddr.sin_addr.s_addr = htonl(stoa(AMBI_IP));
    debug("Socket %s:%d binded", AMBI_IP, AMBI_PORT);

    unsigned int hDp = 0;
    int retv = spIDp_Open(DP_INST0, &hDp);
    if (retv != 0 || hDp == 0) {
        debug("DEBUG(_CF): ERROR: spIDp_Open() ret=%d\n hDp=0x%08X", retv, hDp);
        return NULL;
    }
    while (!exit_routine) {
        if (active) {
            retv = spIDp_DumpImage_samples(hDp, sock);
            if (retv != 0) debug("DEBUG(_CF): ERROR: spIDp_DumpImage() return=0x%08X", retv);
            usleep(100000);
        } else {
            sleep(1);
        }
    }
    close(sock);
    munmap(mm_base, FF_MMAP_SIZE);
    close(fd);
    retv = spIDp_Close(hDp);
    if (retv != 0) debug("DEBUG(_CF): ERROR: spIDp_Close() return=0x%08X", retv);
    debug("routine end");
    return 0;
}