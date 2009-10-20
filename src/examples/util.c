#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void
hex_dump(uint8_t *buf, int count)
{
    int ii;

    for (ii = 0; ii < count; ii++)
    {
        if ((ii & 0xF) == 0xF)
            fprintf(stderr, "%02x\n", buf[ii]);
        else if ((ii & 0x7) == 0x7)
            fprintf(stderr, "%02x  ", buf[ii]);
        else
            fprintf(stderr, "%02x ", buf[ii]);
    }
    if ((ii & 0xF) != 0x0)
        fprintf(stderr, "\n");
}

void
indent_printf(int level, char *fmt, ...)
{
    va_list ap;
    int ii;
    size_t wrote;

    for (ii = 0; ii < level; ii++)
    {
        wrote = fwrite("    ", 1, 4, stdout);
    }
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    wrote = fwrite("\n", 1, 1, stdout);
}

