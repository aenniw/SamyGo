// ===========================================================================
// SamyConsole 2.1 loader (c)2010 geo650 / SamyGO
// ===========================================================================

#include <stdio.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <string.h>
#include <SDL/SDL.h>

#define KEY_POWER    0x02
#define KEY_TV        0x1B
#define KEY_NUM1    0x04
#define KEY_NUM2    0x05
#define KEY_NUM3    0x06
#define KEY_NUM4    0x08
#define KEY_NUM5    0x09
#define KEY_NUM6    0x0A
#define KEY_NUM7    0x0C
#define KEY_NUM8    0x0D
#define KEY_NUM9    0x0E
#define KEY_ENTER    0x68
#define KEY_NUM0    0x11
#define KEY_PRE_CH    0x13
#define KEY_VOL_PLUS    0x07
#define KEY_VOL_MINUS    0x0B
#define KEY_MUTE    0x0F
#define KEY_SOURCE    0x01
#define KEY_P_UP    0x12
#define KEY_P_DOWN    0x10
#define KEY_CH_LIST    0x6B
#define KEY_MENU    0x1A
#define KEY_FAV_CH    0x44
#define KEY_TOOLS    0x4B
#define KEY_RETURN    0x58
#define KEY_CUR_UP    0x60
#define KEY_CUR_LEFT    0x65
#define KEY_CUR_RIGHT    0x62
#define KEY_CUR_DOWN    0x61
#define KEY_INTERNET    0x93
#define KEY_EXIT    0x2D
#define KEY_COL_RED    0x6C
#define KEY_COL_GREEN    0x14
#define KEY_COL_YELLOW    0x15
#define KEY_COL_BLUE    0x16
#define KEY_TTX_MIX    0x2C
#define KEY_MEDIA_P    0x8C
#define KEY_CONTENT    0x79
#define KEY_INFO    0x1F
#define KEY_GUIDE    0x4F
#define KEY_SUBT    0x25
#define KEY_REWIND    0x45
#define KEY_PAUSE    0x4A
#define KEY_FORWARD    0x48
#define KEY_RECORD    0x49
#define KEY_PLAY    0x47
#define KEY_STOP    0x46
#define KEY_BEZEL_SOURCE    0x101
#define KEY_BEZEL_POWER        0x102
#define KEY_BEZEL_VOL_UP    0x107
#define KEY_BEZEL_VOL_DOWN    0x10B
#define KEY_BEZEL_P_DOWN    0x110
#define KEY_BEZEL_P_UP        0x112

static unsigned injector_data[2 + 2];
int *ex_key_code;
int *ex_key_mod;

int button_pressed(int key) {
    int newkey = 0;
    int newmod = 0;
    switch (key) {
        case KEY_POWER:
        case KEY_BEZEL_POWER:
        case KEY_TV:
            newkey = SDLK_F10;
            break;
        case KEY_NUM1:
        case KEY_NUM2:
        case KEY_NUM3:
        case KEY_NUM4:
        case KEY_NUM5:
        case KEY_NUM6:
        case KEY_NUM7:
        case KEY_NUM8:
        case KEY_NUM9:
            newkey = -1;
            break;
        case KEY_ENTER:
            newkey = SDLK_RETURN;
            break;
        case KEY_NUM0:
        case KEY_PRE_CH:
            newkey = -1;
            break;
        case KEY_VOL_PLUS:
        case KEY_BEZEL_VOL_UP:
        case KEY_VOL_MINUS:
        case KEY_BEZEL_VOL_DOWN:
        case KEY_MUTE:
            break;
        case KEY_SOURCE:
        case KEY_BEZEL_SOURCE:
        case KEY_P_UP:
        case KEY_BEZEL_P_UP:
        case KEY_P_DOWN:
        case KEY_BEZEL_P_DOWN:
        case KEY_CH_LIST:
            newkey = -1;
            break;
        case KEY_MENU:
            newkey = SDLK_F1;
            break;
        case KEY_FAV_CH:
            newkey = -1;
            break;
        case KEY_TOOLS:
            newkey = SDLK_F12;
            break;
        case KEY_RETURN:
            newkey = SDLK_F10;
            break;
        case KEY_CUR_UP:
            newkey = SDLK_UP;
            break;
        case KEY_CUR_LEFT:
            newkey = SDLK_LEFT;
            break;
        case KEY_CUR_RIGHT:
            newkey = SDLK_RIGHT;
            break;
        case KEY_CUR_DOWN:
            newkey = SDLK_DOWN;
            break;
        case KEY_INTERNET:
            newkey = -1;
            break;
        case KEY_EXIT:
            newkey = SDLK_F10;
            break;
        case KEY_COL_RED:
        case KEY_COL_GREEN:
        case KEY_COL_YELLOW:
        case KEY_COL_BLUE:
        case KEY_TTX_MIX:
        case KEY_MEDIA_P:
            newkey = -1;
            break;
        case KEY_CONTENT:
            newkey = SDLK_F10;
            break;
        case KEY_INFO:
        case KEY_GUIDE:
        case KEY_SUBT:
        case KEY_REWIND:
        case KEY_PAUSE:
        case KEY_FORWARD:
        case KEY_RECORD:
        case KEY_PLAY:
        case KEY_STOP:
            newkey = -1;
            break;
        default:
            break;
    }

    if (newkey != 0) {
        if ((newmod != -1) && (ex_key_mod)) *ex_key_mod = newmod;
        if ((newkey != -1) && (ex_key_code)) *ex_key_code = newkey;
        key = KEY_STOP;
    }

    return (key);
}

void button_released(void) {
    if (*ex_key_code > 0) *ex_key_code = -(*ex_key_code);
}

void inj_press(int a, int key, int arg_r2) {
    asm volatile(
    "STMFD   SP!, {R0}\n"
            "STMFD   SP!, {R1-R12}\n"
    );

    button_pressed(key);

    asm volatile(
    "LDMFD   SP!, {R1-R12}\n"
            "MOV     R1, R0\n"
            "MOV     R6, R0\n"
            "LDMFD   SP!, {R0}\n"
            "LDMFD   SP!, {R4,LR}\n"
            "MOV     R7, R2\n"
            "LDR     R3, [R8]\n"
            "LDR     PC, =(_ZN9KeyCommon17SendKeyPressInputEii + 0x18 )\n"
    );
}

void inj_release(int a, int key, int arg_r2) {
    asm volatile(
    "STMFD   SP!, {R0-R12, R14}\n"
    );

    button_released();

    asm volatile(
    "LDMFD   SP!, {R0-R12, R14}\n"
            "LDMFD   SP!, {R4,LR}\n"
            "LDR     PC, =(_ZN9KeyCommon19SendKeyReleaseInputEi + 0x18 )\n"
    );
}

static void cacheflush(void *start, size_t size) {
    asm("mov r0, %0\n"
            "mov r1, %1\n"
            "mov r2, #0\n"
            "mov r7, #0xf0000\n"
            "add r7, #0x02\n"
            "swi #0\n"::
    "g"(start), "g"((long) start + size) :
    "r0", "r1", "r2", "r7");
}

static void inject(unsigned inj_p, unsigned *KeyCommon_SendKeyPressInput, unsigned inj_r,
                   unsigned *KeyCommon_KeyReleaseInput) {
    int page = ((unsigned) KeyCommon_SendKeyPressInput) & ~0x0FFF;
    mprotect((void *) page, 8192, PROT_READ | PROT_WRITE | PROT_EXEC);

    if (inj_p && inj_r) {
        injector_data[0] = KeyCommon_SendKeyPressInput[4];
        injector_data[1] = KeyCommon_SendKeyPressInput[5];
        KeyCommon_SendKeyPressInput[4] = 0xE51FF004;
        KeyCommon_SendKeyPressInput[5] = inj_p;
        injector_data[2] = KeyCommon_KeyReleaseInput[4];
        injector_data[3] = KeyCommon_KeyReleaseInput[5];
        KeyCommon_KeyReleaseInput[4] = 0xE51FF004;
        KeyCommon_KeyReleaseInput[5] = inj_r;
    }
    else {
        KeyCommon_SendKeyPressInput[4] = injector_data[0];
        KeyCommon_SendKeyPressInput[5] = injector_data[1];
        KeyCommon_KeyReleaseInput[4] = injector_data[2];
        KeyCommon_KeyReleaseInput[5] = injector_data[3];
    }
    mprotect((void *) page, 8192, PROT_READ | PROT_EXEC);
    cacheflush(KeyCommon_SendKeyPressInput + 4, 8);
    cacheflush(KeyCommon_KeyReleaseInput + 4, 8);
}

int Game_Main(const char *path, const char *udn __attribute__ ((unused))) {
    char s[256];

    unsigned *handle = NULL;

    if ((handle = dlopen(NULL, RTLD_LAZY | RTLD_GLOBAL)) == NULL) return 2;

    unsigned *KeyCommon_SendKeyPressInput = dlsym(handle, "_ZN9KeyCommon17SendKeyPressInputEii");
    unsigned *KeyCommon_KeyReleaseInput = dlsym(handle, "_ZN9KeyCommon19SendKeyReleaseInputEi");

    if ((KeyCommon_SendKeyPressInput == NULL) || (KeyCommon_KeyReleaseInput == NULL)) {
        dlclose(handle);
        return 2;
    }

    dlclose(handle);

    strcpy(s, path);
    strcat(s, "console.so");
    if ((handle = dlopen(s, RTLD_NOW | RTLD_LOCAL)) == NULL) return 2;

    int (*ex_Game_Main)(const char *, const char *);
    *(int **) (&ex_Game_Main) = dlsym(handle, "_Game_Main");
    if (!ex_Game_Main) {
        dclose(handle);
        return 2;
    }

    ex_key_code = dlsym(handle, "ex_key_code");
    ex_key_mod = dlsym(handle, "ex_key_mod");
    if ((!ex_key_code) || (!ex_key_mod)) {
        dclose(handle);
        return 2;
    }

    inject((unsigned) inj_press, KeyCommon_SendKeyPressInput, (unsigned) inj_release, KeyCommon_KeyReleaseInput);

    ex_Game_Main(path, udn);

    inject((unsigned) 0, KeyCommon_SendKeyPressInput, (unsigned) 0, KeyCommon_KeyReleaseInput);

    dlclose(handle);
    return 1;
}

