#ifndef SCCAPTURE_COMMON_H
#define SCCAPTURE_COMMON_H

#include <stdio.h>
#include <stdarg.h>

#if !defined(__x86_64__)
#define DEBUG_LOG    "/mtd_ram/template.log"
#else
#define DEBUG_LOG    "/tmp/template.log"
#endif

void debug(const char *format, ...);

#endif //SCCAPTURE_COMMON_H
