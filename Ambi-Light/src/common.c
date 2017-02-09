#include "common.h"

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

uint32_t itoa_(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return a << 24 | b << 16 | c << 8 | d;
}

uint32_t stoa(const char *str) {
    if (str == NULL) return 0;
    const char *num[4] = {str, "0", "0", "0"};
    uint8_t n = 1, next = 0;
    while (*(str++) != '\0') {
        if (next) {
            num[n++] = str;
            next = 0;
        }
        if (*str == '.') next = 1;
    }
    return itoa_((uint8_t) strtol(num[0], NULL, 10),
                 (uint8_t) strtol(num[1], NULL, 10),
                 (uint8_t) strtol(num[2], NULL, 10),
                 (uint8_t) strtol(num[3], NULL, 10));
}

int parse_config(void *cfg, const char *section, const char *name, const char *value) {
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    configuration *pconfig = (configuration *) cfg;
    if (MATCH("COMMON", "IP")) {
        pconfig->ip = strdup(value);
    } else if (MATCH("COMMON", "PORT")) {
        pconfig->port = (unsigned int) atoi(value);
    } else if (MATCH("STRIP", "HEIGHT")) {
        pconfig->pixels_h = (unsigned int) atoi(value);
    } else if (MATCH("STRIP", "WIDTH")) {
        pconfig->pixels_w = (unsigned int) atoi(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}