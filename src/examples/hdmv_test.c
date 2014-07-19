/*
 * This file is part of libbluray
 * Copyright (C) 2010  hpi1
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

#include "util/log_control.h"
#include "libbluray/bluray.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

static void _print_event(BD_EVENT *ev)
{
    switch (ev->event) {
        case BD_EVENT_NONE:
            break;
        case BD_EVENT_ERROR:
            printf("EVENT_ERROR:\t%d\n", ev->param);
            break;
        case BD_EVENT_READ_ERROR:
            printf("EVENT_READ_ERROR:\t%d\n", ev->param);
            break;
        case BD_EVENT_ENCRYPTED:
            printf("EVENT_ENCRYPTED:\t%d\n", ev->param);
            break;

        /* current playback position */

        case BD_EVENT_ANGLE:
            printf("EVENT_ANGLE:\t%d\n", ev->param);
            break;
        case BD_EVENT_TITLE:
            printf("EVENT_TITLE:\t%d\n", ev->param);
            break;
        case BD_EVENT_PLAYLIST:
            printf("EVENT_PLAYLIST:\t%d\n", ev->param);
            break;
        case BD_EVENT_PLAYITEM:
            printf("EVENT_PLAYITEM:\t%d\n", ev->param);
            break;
        case BD_EVENT_CHAPTER:
            printf("EVENT_CHAPTER:\t%d\n", ev->param);
            break;

        /* */

        case BD_EVENT_STILL:
            printf("EVENT_STILL:\t%d\n", ev->param);
            break;

        case BD_EVENT_SEEK:
            printf("EVENT_SEEK:\t%d\n", ev->param);
            break;

        case BD_EVENT_STILL_TIME:
            if (ev->param) {
                printf("EVENT_STILL_TIME:\t%d\n", ev->param);
            } else {
                printf("EVENT_STILL_TIME:\tinfinite\n");
            }
            break;

        /* stream selection */

        case BD_EVENT_AUDIO_STREAM:
            printf("EVENT_AUDIO_STREAM:\t%d\n", ev->param);
            break;
        case BD_EVENT_IG_STREAM:
            printf("EVENT_IG_STREAM:\t%d\n", ev->param);
            break;
        case BD_EVENT_PG_TEXTST_STREAM:
            printf("EVENT_PG_TEXTST_STREAM:\t%d\n", ev->param);
            break;
        case BD_EVENT_SECONDARY_AUDIO_STREAM:
            printf("EVENT_SECONDARY_AUDIO_STREAM:\t%d\n", ev->param);
            break;
        case BD_EVENT_SECONDARY_VIDEO_STREAM:
            printf("EVENT_SECONDARY_VIDEO_STREAM:\t%d\n", ev->param);
            break;

        case BD_EVENT_PG_TEXTST:
            printf("EVENT_PG_TEXTST:\t%s\n", ev->param ? "enable" : "disable");
            break;
        case BD_EVENT_SECONDARY_AUDIO:
            printf("EVENT_SECONDARY_AUDIO:\t%s\n", ev->param ? "enable" : "disable");
            break;
        case BD_EVENT_SECONDARY_VIDEO:
            printf("EVENT_SECONDARY_VIDEO:\t%s\n", ev->param ? "enable" : "disable");
            break;
        case BD_EVENT_SECONDARY_VIDEO_SIZE:
            printf("EVENT_SECONDARY_VIDEO_SIZE:\t%s\n", ev->param==0 ? "PIP" : "fullscreen");
            break;

        default:
            printf("UNKNOWN EVENT %d:\t%d\n", ev->event, ev->param);
            break;
      }

      fflush(stdout);
}

static void _read_to_eof(BLURAY *bd)
{
    BD_EVENT ev;
    int      bytes;
    uint64_t total = 0;
    uint8_t  buf[6144];

    bd_seek(bd, bd_get_title_size(bd) - 6144);

    do {
        bytes = bd_read_ext(bd, buf, 6144, &ev);
        total += bytes < 0 ? 0 : bytes;
        _print_event(&ev);
    } while (bytes > 0);

    printf("_read_to_eof(): read %"PRIu64" bytes\n", total);
}

static void _print_events(BLURAY *bd)
{
    BD_EVENT ev;

    do {
        bd_read_ext(bd, NULL, 0, &ev);
        _print_event(&ev);
    } while (ev.event != BD_EVENT_NONE && ev.event != BD_EVENT_ERROR);
}

static void _play_pl(BLURAY *bd)
{
    printf("Playing playlist\n");

    fflush(stdout);
    _read_to_eof(bd);

    printf("Playing playlist done\n\n");

    _print_events(bd);

    printf("\n");
}

int main(int argc, char *argv[])
{
    int title = -1;
    int verbose = 0;
    int args = 0;

    /*
     * parse arguments
     */

    if (argc < 2) {
        printf("\nUsage:\n   %s [-v] [-t <title>] <media_path> [<keyfile_path>]\n\n", argv[0]);
        return -1;
    }

    if (!strcmp(argv[1+args], "-v")) {
        verbose = 1;
        args++;
    }

    if (!strcmp(argv[1+args], "-t")) {
        args++;
        title = atoi(argv[1+args]);
        args++;
        printf("Requested title %d\n", title);
    }

    if (verbose) {
        printf("Enabling verbose debug\n");
        bd_set_debug_mask(bd_get_debug_mask() | DBG_HDMV | DBG_BLURAY);
    }

    printf("\n");

    /*
     * open and setup
     */

    BLURAY  *bd = bd_open(argv[1+args], argv[2+args]);

    if (!bd) {
        printf("bd_open(\'%s\') failed\n", argv[1]);
        return -1;
    }

    bd_set_player_setting    (bd, BLURAY_PLAYER_SETTING_PARENTAL,     99);
    bd_set_player_setting_str(bd, BLURAY_PLAYER_SETTING_AUDIO_LANG,   "eng");
    bd_set_player_setting_str(bd, BLURAY_PLAYER_SETTING_PG_LANG,      "eng");
    bd_set_player_setting_str(bd, BLURAY_PLAYER_SETTING_MENU_LANG,    "eng");
    bd_set_player_setting_str(bd, BLURAY_PLAYER_SETTING_COUNTRY_CODE, NULL);

    /*
     * play
     */

    printf("Running first play movie object\n");

    fflush(stdout);
    bd_play(bd);

    _print_events(bd);

    printf("\n");

    /*
     * play title
     */

    if (title >= 0) {
        printf("Playing title %d\n", title);

        fflush(stdout);
        bd_play_title(bd, title);

        _print_events(bd);

        printf("\n");
    }

    /*
     * play playlist
     */

    _play_pl(bd);

    _play_pl(bd);

    _play_pl(bd);

    /*
     * clean up
     */

    bd_close(bd);

    return 0;
}

