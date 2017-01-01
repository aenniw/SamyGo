#include "log.h"

void debug(const char *format, ...) {
    FILE *f = fopen(DEBUG_LOG, "at");
    if (f) {
        va_list args;
        va_start(args, format);
        vfprintf(f, format, args);
        fprintf(f, "\n");
        fflush(f);
        fclose(f);
        va_end(args);
    }
}