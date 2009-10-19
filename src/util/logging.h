
#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#define DEBUG(X,Y,...) debug(__FILE__,__LINE__,X,Y,##__VA_ARGS__)

enum {
    DBG_CONFIGFILE = 2,
    DBG_FILE = 4,
    DBG_AACS = 8,
    DBG_MKB = 16,
    DBG_MMC = 32,
    DBG_BLURAY = 64
} debug_mask;

const uint32_t master_mask = 0xffff; // this is only temporary
char out[512];

char *print_hex(uint8_t *str, int count);
void debug(char *file, int line, uint32_t mask, const char *format, ...);


char *print_hex(uint8_t *buf, int count)
{
    memset(out, 0, count);

    int zz;
    for(zz = 0; zz < count; zz++) {
        sprintf(out + (zz * 2), "%02X", buf[zz]);
    }

    return out;
}

void debug(char *file, int line, uint32_t mask, const char *format, ...)
{
    uint32_t type = (mask & master_mask) & 0xfffe,
             verbose = !((!(master_mask & 1)) & (mask & 1));

    if (type && verbose) {
        char buffer[512];
        va_list args;

        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);

        fprintf(stderr, "%s:%d: %s", file, line, buffer);
    }
}

#endif /* LOGGING_H_ */
