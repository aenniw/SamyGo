#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <netdb.h>
#include <inih/ini.h>
#include <string.h>

#include "externs.h"
#include "buttons.h"
#include "common.h"

#define FF_MMAP_SIZE 0x800000
#define DP_INST0 0
#define DP_ON 1

#define get_r(w, h) mm_base[ ((h) * WIDTH + (w)) * 4 + 2 + offsetb ]
#define get_g(w, h) mm_base[ ((h) * WIDTH + (w)) * 4 + 1 + offsetb ]
#define get_b(w, h) mm_base[ ((h) * WIDTH + (w)) * 4 + offsetb ]

#define sample_r(w, h, s) samples[s][2] += get_r(w,h)
#define sample_g(w, h, s) samples[s][1] += get_g(w,h)
#define sample_b(w, h, s) samples[s][0] += get_b(w,h)

#define sample_pixel(w, h, s) sample_r(w,h,s); sample_g(w,h,s); sample_b(w,h,s)
#define avg_pixel(s, v, p) colors[0 + 3*(p)] = samples[s][2] / (v); colors[1 + 3*(p)] = samples[s][1] / (v); colors[2 + 3*(p)] = samples[s][0] / (v)

static configuration config = {0, 0, 0, NULL};
static const unsigned int WIDTH = 1920;
static const unsigned int HEIGHT = 1080;
volatile static int exit_routine = 0, active = 0;
static struct sockaddr_in serveraddr = {0};

int fd = -1;
unsigned char *mm_base = NULL;
static uint8_t *colors;

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

        modIncapt_GetActiveVSize(0, &activeVSize);
        modIncapt_SetActiveVSize(0, 0);
        lldDp_Ve_Burst3216(1);
        lldDp_Ve_SetDumpAddress(dp_baddr);
        lldDp_Ve_CapPos(DP_ON);
        lldDp_Ve_WrCapOn(1);
        modIncapt_SetActiveVSize(0, activeVSize);

        fd = open("/dev/mem", O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            debug("_DVr(ERROR): /dev/mem device open failed");
            return -1;
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

    const long sample_size = 30, pixel_skip = 5,
            h_size = (const long) ((HEIGHT - 2 * sample_size) / config.pixels_h), w_size = (const long) (
            (WIDTH - 2 * sample_size) / config.pixels_w);
    long pixels_per_sector = (long) (sample_size * ((WIDTH - 2 * sample_size) / config.pixels_w) / pixel_skip);
    for (int s = 0; s < config.pixels_w; s++) {
        long long samples[2][3] = {0};
        for (int w = sample_size + w_size * s;
             w < sample_size + w_size * (s + 1); w += pixel_skip) {
            for (int h = 0; h < sample_size; h += pixel_skip) {
                sample_pixel(w, h, 0); // bottom
                sample_pixel(w, h + HEIGHT - sample_size, 1); // top
            }
        }
        avg_pixel(0, pixels_per_sector, s); // bottom
        avg_pixel(1, pixels_per_sector, 2 * config.pixels_w + config.pixels_h - s - 1); // top
    }
    pixels_per_sector = (long) (sample_size * ((HEIGHT - 2 * sample_size) / config.pixels_h) / pixel_skip);
    for (int s = 0; s < config.pixels_h; s++) {
        long long samples[2][3] = {0};
        for (int h = sample_size + h_size * s;
             h < sample_size + h_size * (s + 1); h += pixel_skip) {
            for (int w = 0; w < sample_size; w += pixel_skip) {
                sample_pixel(w, h, 0); // right
                sample_pixel(w + WIDTH - sample_size, h, 1); // left
            }
        }
        avg_pixel(0, pixels_per_sector, 2 * (config.pixels_w + config.pixels_h) - s - 1); // right
        avg_pixel(1, pixels_per_sector, config.pixels_w + s); // left
    }
    sendto(sock, colors, 6 * (config.pixels_w + config.pixels_h), 0, (struct sockaddr *) &serveraddr,
           sizeof(serveraddr));
    return 0;
}

static int button_pressed(int key) __attribute__ ((noinline));

static int button_pressed(const int key) {
    switch (key) {
        case KEY_COL_GREEN:
            if (active == 0) {
                active = 1;
                return KEY_NOTHING;
            }
            break;
        case KEY_COL_YELLOW:
            if (active != 0) {
                active = 0;
                return KEY_NOTHING;
            }
            break;
        case KEY_BEZEL_POWER:
        case KEY_POWER:
            exit_routine = 1;
            break;
    }
    return (key);        // return destination button code to be processed by exeDSP
}

void injection(int a, int key, int arg_r2) __attribute__ ((noinline));

void injection(int a, int key, int arg_r2) {
    // CAUTION! Do not add/modify code here... or modify injection indexes in loader

    // [0][1] gcc should put "STMFD SP!, {R4,LR}" and "MOV R4,[R1]" here
    __asm__ volatile(
    "STMFD   SP!, {R0,R2,R3,R12}\n"        // [2] store some registers
    );

    // first, we have to execute our function
    button_pressed(key);                // [3][4] calling local function with key variable in R0, returning R0

    // then, prepare for calling and call previous injection function
    __asm__ volatile(
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

void *ambi_routine(void *) __attribute__ ((noinline));

void *ambi_routine(void *path) {
    debug("loading config %s", (char *) path);
    if (path != NULL) chdir(path);
    if (ini_parse(CONFIG_FILE, parse_config, &config) < 0) {
        debug("Can't load '%s'", CONFIG_FILE);
        return NULL;
    }
    debug("config loaded from '%s': ip=%s, port=%d, w*h [%d * %d]", CONFIG_FILE, config.ip, config.port,
          config.pixels_w, config.pixels_h);
    if (config.ip == NULL || config.port == 0 || config.pixels_w == 0 || config.pixels_h == 0) return NULL;
    free(path);
    debug("routine started");
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return NULL;
    fcntl(sock, F_SETFL, O_NONBLOCK | fcntl(sock, F_GETFL, 0));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(config.port);
    serveraddr.sin_addr.s_addr = htonl(stoa(config.ip));
    unsigned int hDp = 0;
    int retv = spIDp_Open(DP_INST0, &hDp);
    if (retv != 0 || hDp == 0) {
        debug("DEBUG(_CF): ERROR: spIDp_Open() ret=%d\n hDp=0x%08X", retv, hDp);
        return NULL;
    }
    colors = malloc(sizeof(uint8_t) * (6 * (config.pixels_h + config.pixels_w)));
    if (!colors) {
        debug("Cannot malloc buffer for colors");
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
    free(colors);
    if (mm_base != NULL) munmap(mm_base, FF_MMAP_SIZE);
    if (fd >= 0) close(fd);
    retv = spIDp_Close(hDp);
    if (retv != 0) debug("DEBUG(_CF): ERROR: spIDp_Close() return=0x%08X", retv);
    debug("routine end");
    return NULL;
}