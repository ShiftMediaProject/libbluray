
#include <stdlib.h>

#include "logging.h"

char out[512];

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
    uint32_t master_mask;
    char *env;

    if ((env = getenv("BD_DEBUG_MASK"))) {
        master_mask = atoi(env);
    } else {
        master_mask = 0xffff;
    }

    if (mask & master_mask) {
        char buffer[512];
        va_list args;

        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);

        fprintf(stderr, "%s:%d: %s", file, line, buffer);
    }
}
