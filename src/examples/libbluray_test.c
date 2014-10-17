/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  Obliter0n
 * Copyright (C) 2013       VideoLAN
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
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "libbluray/bluray.h"

int main(int argc, char *argv[])
{
    BLURAY *bd;
    int     ii, num_titles;

    if (argc < 3) {
        fprintf(stderr, "Usage:\n   %s <media_path> [<title_number>]\n\n", argv[0]);
    }
    if (argc < 2) {
        return -1;
    }

    /* open disc */
    bd = bd_open(argv[1], NULL);
    if (!bd) {
        fprintf(stderr, "Error opening %s\n", argv[1]);
        return -1;
    }

    /* get titles */
    num_titles = bd_get_titles(bd, TITLES_RELEVANT, 0);

    /* list titles */
    if (argc < 3) {
        printf("Listing titles:\n");

        for (ii = 0; ii < num_titles; ii++) {
            BLURAY_TITLE_INFO* ti;
            ti = bd_get_title_info(bd, ii, 0);
            printf("index: %d duration: %02"PRIu64":%02"PRIu64":%02"PRIu64" chapters: %d\n",
                   ii,
                   (ti->duration / 90000) / (3600),
                   ((ti->duration / 90000) % 3600) / 60,
                   ((ti->duration / 90000) % 60),
                   ti->chapter_count);
        }

        bd_close(bd);
        return 0;
    }

    /* select title */
    if (bd_select_title(bd, atoi(argv[2])) <= 0) {
        fprintf(stderr, "Error opening title #%s\n", argv[2]);
        bd_close(bd);
        return -1;
    }

    /* dump */
    FILE *fd = fopen("streamdump.m2ts", "wb");
    if (fd) {
        printf("Writing to streamdump.m2ts\n");
        ii = 0;
        while (1) {
            unsigned char buf[6144];
            int len = bd_read(bd, buf, 6144);
            if (len <= 0) break;
            if (fwrite(buf, len, 1, fd) < 1) {
                fprintf(stderr, "Write error\n");
                break;
            }
            if (!(ii % 100)) {
                printf("Block %d\r", ii);
            }
            ii++;
        }
        fclose(fd);
    } else {
        printf("Error opening streamdump.m2ts\n");
    }

    bd_close(bd);

    return 0;
}

