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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "util/log_control.h"
#include "libbluray/bluray.h"
#include "libbluray/decoders/overlay.h"


#define PRINT_EV0(e)                                \
  case BD_EVENT_##e:                                \
      printf(#e "\n");                              \
      break
#define PRINT_EV1(e,f)                              \
  case BD_EVENT_##e:                                \
    printf("%-25s " f "\n", #e ":", ev->param);     \
      break

static void _print_event(BD_EVENT *ev)
{
    switch ((bd_event_e)ev->event) {

        case BD_EVENT_NONE:
            break;

        /* errors */

        PRINT_EV1(ERROR,      "%u");
        PRINT_EV1(READ_ERROR, "%u");
        PRINT_EV1(ENCRYPTED,  "%u");

        /* current playback position */

        PRINT_EV1(ANGLE,    "%u");
        PRINT_EV1(TITLE,    "%u");
        PRINT_EV1(PLAYLIST, "%05u.mpls");
        PRINT_EV1(PLAYITEM, "%u");
        PRINT_EV1(PLAYMARK, "%u");
        PRINT_EV1(CHAPTER,  "%u");
        PRINT_EV0(END_OF_TITLE);

        PRINT_EV1(STEREOSCOPIC_STATUS,  "%u");

        PRINT_EV1(SEEK,     "%u");
        PRINT_EV0(DISCONTINUITY);
        PRINT_EV0(PLAYLIST_STOP);

        /* Interactive */

        PRINT_EV1(STILL_TIME,           "%u");
        PRINT_EV1(STILL,                "%u");
        PRINT_EV1(SOUND_EFFECT,         "%u");
        PRINT_EV1(IDLE,                 "%u");
        PRINT_EV1(POPUP,                "%u");
        PRINT_EV1(MENU,                 "%u");
        PRINT_EV1(UO_MASK_CHANGED,      "0x%04x");
        PRINT_EV1(KEY_INTEREST_TABLE,   "0x%04x");

        /* stream selection */

        PRINT_EV1(PG_TEXTST,              "%u");
        PRINT_EV1(SECONDARY_AUDIO,        "%u");
        PRINT_EV1(SECONDARY_VIDEO,        "%u");
        PRINT_EV1(PIP_PG_TEXTST,          "%u");

        PRINT_EV1(AUDIO_STREAM,           "%u");
        PRINT_EV1(IG_STREAM,              "%u");
        PRINT_EV1(PG_TEXTST_STREAM,       "%u");
        PRINT_EV1(SECONDARY_AUDIO_STREAM, "%u");
        PRINT_EV1(SECONDARY_VIDEO_STREAM, "%u");
        PRINT_EV1(SECONDARY_VIDEO_SIZE,   "%u");
        PRINT_EV1(PIP_PG_TEXTST_STREAM,   "%u");
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

static void _overlay_cb(void *h, const struct bd_overlay_s * const ov)
{
    (void)h;

    if (ov) {
        printf("OVERLAY @%ld p%d %d: %d,%d %dx%d\n", (long)ov->pts, ov->plane, ov->cmd, ov->x, ov->y, ov->w, ov->h);

    } else {
        printf("OVERLAY CLOSE\n");
    }
}

static void _argb_overlay_cb(void *h, const struct bd_argb_overlay_s * const ov)
{
    (void)h;

    if (ov) {
      printf("ARGB OVERLAY @%ld p%d %d: %d,%d %dx%d\n", (long)ov->pts, ov->plane, ov->cmd, ov->x, ov->y, ov->w, ov->h);

    } else {
        printf("ARGB OVERLAY CLOSE\n");
    }
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

    bd_register_overlay_proc(bd, bd, _overlay_cb);
    bd_register_argb_overlay_proc(bd, bd, _argb_overlay_cb, NULL);

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

