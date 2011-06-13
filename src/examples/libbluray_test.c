/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  Obliter0n
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "libbluray/bluray.h"
#include "util/logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define HEX_PRINT(X,Y) { int zz; for(zz = 0; zz < Y; zz++) fprintf(stderr, "%02X", X[zz]); fprintf(stderr, "\n"); }

int main(int argc, char *argv[])
{
    if (argc == 4) {
    BLURAY *bd = bd_open(argv[1], argv[2]);
    int count, ii;

    //bd_get_titles(bd, 0);

    BD_DEBUG(DBG_BLURAY,"\nListing titles:\n");

    count = bd_get_titles(bd, TITLES_RELEVANT, 0);
    for (ii = 0; ii < count; ii++)
    {
        BLURAY_TITLE_INFO* ti;
        ti = bd_get_title_info(bd, ii, 0);
        BD_DEBUG(DBG_BLURAY,
       "index: %d duration: %02"PRIu64":%02"PRIu64":%02"PRIu64" chapters: %d\n",
              ii,
              (ti->duration / 90000) / (3600),
              ((ti->duration / 90000) % 3600) / 60,
              ((ti->duration / 90000) % 60),
              ti->chapter_count);
    }

    bd_select_title(bd, atoi(argv[3]));

    unsigned char *buf = malloc(6144);
    memset(buf,0,6144);

#if 0
    bd_read(bd, buf, 2048);
    bd_read(bd, buf, 2048);
    bd_read(bd, buf, 2048);
    bd_read(bd, buf, 6144);
#else

    FILE *fd = fopen("streamdump.m2ts", "wb");
    if (fd) {
        while (1) {
            int len = bd_read(bd, buf, 6144);
            if (len <= 0) break;
            if (fwrite(buf, len, 1, fd) < 1)
              break;
            if (!(ii % 1000)) {
                BD_DEBUG(DBG_BLURAY,
                         "%d\r", ii);
            }

        }
        fclose(fd);
    }

#endif

    bd_close(bd);
    } else {
        printf("\nUsage:\n   %s <media_path> <keyfile_path> <title_number>\n\n", argv[0]);
    }

    return 0;
}

