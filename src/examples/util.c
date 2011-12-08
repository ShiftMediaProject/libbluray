/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  John Stebbins
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdarg.h>

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
indent_printf(int level, const char *fmt, ...)
{
    va_list ap;
    int ii;

    for (ii = 0; ii < level; ii++)
    {
        printf("    ");
    }
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

