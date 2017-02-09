#ifndef SCCAPTURE_COMMON_H
#define SCCAPTURE_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define DEBUG_LOG    "/mtd_ram/ambi.log"
#define CONFIG_FILE "./config.ini"

typedef struct {
    unsigned int port, pixels_w, pixels_h;
    const char *ip;
} configuration;

void debug(const char *format, ...);

uint32_t itoa_(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

uint32_t stoa(const char *str);

int parse_config(void *cfg, const char *section, const char *name, const char *value);


#endif //SCCAPTURE_COMMON_H
