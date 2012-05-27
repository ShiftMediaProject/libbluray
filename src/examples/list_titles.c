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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "libbluray/bluray.h"
#include "util/logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

#define OPTS "hs:"

static void _usage(char *cmd)
{
    fprintf(stderr, 
"%s <BD base dir> [-s <seconds>]\n"
"   List the titles and title info of a BD\n"
"   -s #    - Filter out titles shorter then # seconds\n"
"   -h      - This message\n",
        cmd
    );
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    BLURAY *bd;
    int count, ii, opt;
    unsigned int seconds = 0;
    char *bd_dir = NULL;

    BD_DEBUG(DBG_BLURAY,"\nListing titles:\n");

    do
    {
        opt = getopt(argc, argv, OPTS);
        switch (opt)
        {
            case -1:
                if (optind < argc && bd_dir == NULL)
                {
                    bd_dir = argv[optind];
                    optind++;
                    opt = 1;
                }
                break;
            case 's':
                seconds = strtol(optarg, NULL, 0);
                break;
            case 'h':
            default:
                _usage(argv[0]);
                break;
        }
    } while (opt != -1);

    if (bd_dir == NULL)
    {
        _usage(argv[0]);
    }
    bd = bd_open(bd_dir, NULL);

    count = bd_get_titles(bd, TITLES_RELEVANT, seconds);
    for (ii = 0; ii < count; ii++)
    {
        BLURAY_TITLE_INFO* ti;
        ti = bd_get_title_info(bd, ii, 0);
        printf(
       "index: %d duration: %02"PRIu64":%02"PRIu64":%02"PRIu64" chapters: %d angles: %u clips %u\n",
              ii + 1,
              (ti->duration / 90000) / (3600),
              ((ti->duration / 90000) % 3600) / 60,
              ((ti->duration / 90000) % 60),
              ti->chapter_count, ti->angle_count, ti->clip_count
        );
        bd_free_title_info(ti);
    }
    bd_close(bd);
    return 0;
}
