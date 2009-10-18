
#include <string.h>
#include "logging.h"

const uint32_t master_mask = 0xffff; // this is only temporary
char out[512];

#define HEX_PRINT(X,Y) { int zz; for(zz = 0; zz < Y; zz++) fprintf(stderr, "%02X", X[zz]); fprintf(stderr, "\n"); }

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
