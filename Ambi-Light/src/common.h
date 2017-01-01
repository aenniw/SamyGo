#ifndef SCCAPTURE_COMMON_H
#define SCCAPTURE_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define DEBUG_LOG    "/mtd_ram/ambi.log"

void debug(const char *format, ...);

uint32_t itoa_(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

uint32_t stoa(char *str);


#endif //SCCAPTURE_COMMON_H
