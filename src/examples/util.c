/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  John Stebbins
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders of libbluray
 * gives permission to link the code of its release of libbluray with the
 * OpenSSL project's "OpenSSL" library (or with modified versions of it
 * that use the same license as the "OpenSSL" library), and distribute
 * the linked executables.  You must obey the GNU General Public License
 * in all respects for all of the code used other than "OpenSSL".  If you
 * modify this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to do
 * so, delete this exception statement from your version.
 */

#include "util.h"

#include <stdio.h>
#include <stdarg.h>

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

