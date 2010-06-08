/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  cRTrn13
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

#include "logging.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static int debug_init = 0;
static debug_mask_t debug_mask = DBG_CRIT;
static FILE *logfile = NULL;

char *print_hex(char *out, const uint8_t *buf, int count)
{
    int zz;
    for(zz = 0; zz < count; zz++) {
        sprintf(out + (zz * 2), "%02X", buf[zz]);
    }

    return out;
}

void debug(const char *file, int line, uint32_t mask, const char *format, ...)
{
    // Only call getenv() once.
    if (!debug_init) {
        debug_init = 1;
        logfile = stderr;

        char *env1 = NULL, *env2 = NULL;
        if ((env1 = getenv("BD_DEBUG_MASK")))
            debug_mask = strtol(env1, NULL, 0);

        // Send DEBUG to file?
        if ((env2 = getenv("BD_DEBUG_FILE"))) {
            logfile = fopen(env2, "wb");
            setvbuf(logfile, NULL, _IOLBF, 0);
        }
    }

    if (mask & debug_mask) {
        char buffer[512];
        va_list args;

        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);

        fprintf(logfile, "%s:%d: %s", file, line, buffer);
    }
}
