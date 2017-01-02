#include <stdio.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <pthread.h>

#include "common.h"

// cache flush function for changes made (injected code area)
static void cacheflush(void *start, size_t size) {
    // For details, please see the Linux sources, files
    // arch/arm/kernel/traps.c, arch/arm/kernel/entry-common.S and
    // asm-arm/unistd.h

    // This syscall is supported by all 2.4 and 2.6 Linux kernels.  2.2 linux
    // kernels got first support with version 2.2.18.

    __asm__("mov r0, %0\n"
            "mov r1, %1\n"
            "mov r2, #0\n"
            // EABI or Thumb syscall: syscall number passed in 'r7' (syscall base
            // number is 0x0).  Note that Thumb and EABI are generally not the
            // same.  It just happens that the simple cacheflush syscall doesn't
            // expose any of differences in calling conventions.
            "mov r7, #0xf0000\n"
            "add r7, #0x02\n"
            "swi #0\n"::
    "g"(start), "g"((long) start + size) :
    "r0", "r1", "r2", "r7");
}

// injecting procedure
void injector(const char *path) {
    char s[256];

    debug("injector() started");

    unsigned *handle = NULL;
    debug("- getting self handle");
    if ((handle = dlopen(NULL, RTLD_LAZY | RTLD_GLOBAL)) == NULL) {
        debug(dlerror());
        return;
    }
    debug("- self handle retrieved");

    debug("- getting keypress function");
    unsigned *KeyCommon_SendKeyPressInput = dlsym(handle, "_ZN9KeyCommon17SendKeyPressInputEii");
    if (KeyCommon_SendKeyPressInput == NULL) {
        debug(dlerror());
        return;
    }
    debug("- keypress function retrieved");

    dlclose(handle);

    // Load OSDcap library
    sprintf(s, "%sambi.so", path);
    debug("- loading library:");
    debug(s);
    if ((handle = dlopen(s, RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE)) == NULL) {
        debug(dlerror());
        return;
    }

    debug("- library loaded");

    // obtain injection procedure address
    debug("- obtain injection procedure address");
    unsigned *ex_injection = dlsym(handle, "injection");
    if (ex_injection == NULL) {
        debug(dlerror());
        dlclose(handle);
        return;
    }
    debug("- injection function retrieved");

    // obtain path setting function address
    debug("- obtain ambi function address");
    void (*ex_ambi_routine)(void *);
    *(void **) (&ex_ambi_routine) = dlsym(handle, "ambi_routine");
    if (ex_ambi_routine == NULL) {
        debug(dlerror());
        dlclose(handle);
        return;
    }
    debug("- ambi function retrieved");

    dlclose(handle);

    // Injecting!
    debug("- injecting...");
    int page = ((unsigned) KeyCommon_SendKeyPressInput) & ~0x0FFF;
    mprotect((void *) page, 8192, PROT_READ | PROT_WRITE | PROT_EXEC);

    if (KeyCommon_SendKeyPressInput[4] == 0xE51FF004)    // remapped already (by other application) ?
    {
        debug("- old injection detected");
        // get previous injection function address
        // modify ex_injection returning command (self-injection)
        int page2 = ((unsigned) ex_injection) & ~0x0FFF;
        mprotect((void *) page2, 8192, PROT_READ | PROT_WRITE | PROT_EXEC);
        ex_injection[9] = KeyCommon_SendKeyPressInput[4];
        ex_injection[10] = KeyCommon_SendKeyPressInput[5];
        mprotect((void *) page2, 8192, PROT_READ | PROT_EXEC);
        cacheflush(ex_injection + 9, 8);
    }
    // else first injection...

    // set "classic" injection function
    KeyCommon_SendKeyPressInput[4] = 0xE51FF004;    // single data transfer:
    // [E] - condition=always,
    // [5] - I=0: offset is an immediate value, P=1: add offset before transfer
    // [1] - U=0: substract offset from base (no add to), B=0: tranfer word quantity (not byte),
    //	 W=0: no write-back (to base) MUST BE ZERO FOR PC!!!, L=1: load from memory (not store),
    // [F] - Rn=15 (PC) - base register,
    // [F] - Rd=15 (PC) - source/destination register,
    // [004] - offset=4 (immediate --> see above),
    // this is probably a word load command: LDR PC, =(next_32bit_value, i.e. ex_injection)
    KeyCommon_SendKeyPressInput[5] = (unsigned) ex_injection;
    mprotect((void *) page, 8192, PROT_READ | PROT_EXEC);

    pthread_t helper;
    pthread_create(&helper, NULL, (void *) ex_ambi_routine, NULL);
    debug("ambi routine started in separate thread:%d", helper);

    cacheflush(KeyCommon_SendKeyPressInput + 4, 8);
    // injection complete!
    debug("injector() exiting");
}

int Game_Main(const char *path, const char *udn __attribute__ ((unused))) {
    debug("Game_Main() started");
    injector(path);        // load, configure and inject our code leaving it in memory
    return 1;        // exit loader while injected module stays in memory
}

