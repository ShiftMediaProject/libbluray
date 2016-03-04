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

#include "libbluray/bluray.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

#define OPTS "ahs:l"

static void _usage(char *cmd)
{
    fprintf(stderr,
"%s <BD base dir> [-s <seconds>]\n"
"   List the titles and title info of a BD\n"
"   -s #    - Filter out titles shorter than # seconds\n"
"   -a      - List all titles\n"
"   -l      - Show language codes\n"
"   -h      - This message\n",
        cmd
    );
    exit(EXIT_FAILURE);
}

static void _print_langs(const char *tag, const BLURAY_STREAM_INFO *si, int count)
{
    int ii;
    printf("\t%s: ", tag);
    for (ii = 0; ii < count; ii++) {
        printf("%3.3s ", si[ii].lang);
    }
    printf("\n");
}

static void _dump_langs(const BLURAY_CLIP_INFO *ci)
{
    if (ci->audio_streams) {
        _print_langs("AUD",  ci->audio_streams, ci->audio_stream_count);
    }
    if (ci->pg_streams) {
        _print_langs("PG ",  ci->pg_streams,    ci->pg_stream_count);
    }
}

int main(int argc, char *argv[])
{
    BLURAY *bd;
    int count, ii, opt, main_title;
    unsigned int seconds = 0, langs = 0;
    unsigned int flags = TITLES_RELEVANT;
    char *bd_dir = NULL;

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
            case 'a':
                flags = TITLES_ALL;
                break;
            case 's':
                seconds = strtol(optarg, NULL, 0);
                break;
            case 'l':
                langs = 1;
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
    if (!bd) {
        fprintf(stderr, "bd_open(%s) failed\n", bd_dir);
        exit(EXIT_FAILURE);
    }
    count = bd_get_titles(bd, flags, seconds);
    main_title = bd_get_main_title(bd);
    if (main_title >= 0) {
        printf("Main title: %d\n", main_title + 1);
    }
    for (ii = 0; ii < count; ii++)
    {
        BLURAY_TITLE_INFO* ti;
        ti = bd_get_title_info(bd, ii, 0);
        printf(
       "index: %3d duration: %02"PRIu64":%02"PRIu64":%02"PRIu64" chapters: %3d angles: %2u clips: %3u (playlist: %05d.mpls) "
       "V:%d A:%-2d PG:%-2d IG:%-2d SV:%d SA:%d\n",
              ii + 1,
              (ti->duration / 90000) / (3600),
              ((ti->duration / 90000) % 3600) / 60,
              ((ti->duration / 90000) % 60),
              ti->chapter_count, ti->angle_count, ti->clip_count, ti->playlist,
              ti->clips[0].video_stream_count,
              ti->clips[0].audio_stream_count,
              ti->clips[0].pg_stream_count,
              ti->clips[0].ig_stream_count,
              ti->clips[0].sec_video_stream_count,
              ti->clips[0].sec_audio_stream_count
        );
        if (langs) {
            _dump_langs(&ti->clips[0]);
        }
        bd_free_title_info(ti);
    }
    bd_close(bd);
    return 0;
}
