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

#include "libbluray/bdnav/index_parse.h"

#include <stdio.h>

static void _indx_print_app_info(INDX_APP_INFO *app_info)
{
    const char video_format_str[16][8] = {
        "ignored", "480i", "576i", "480p", "1080i", "720p", "1080p", "576p"
    };

    const char frame_rate_str[16][16] = {
        "ignored",
        "23.976 Hz",
        "24 Hz",
        "25 Hz",
        "29.97 Hz",
        "reserved",
        "50 Hz",
        "59.94 Hz"
    };

    printf("  initial mode  : %s\n", app_info->initial_output_mode_preference ? "3D"  : "2D");
    printf("  content exists: %s\n", app_info->content_exist_flag             ? "Yes" : "No");
    printf("  video format  : %s (0x%x)\n", video_format_str[app_info->video_format], app_info->video_format);
    printf("  frame rate    : %s (0x%x)\n", frame_rate_str[app_info->frame_rate],     app_info->frame_rate);
    printf("  provider data : %32s\n",      app_info->user_data);
}

static void _indx_print_hdmv_obj(INDX_HDMV_OBJ *hdmv)
{
    const char *const playback_types[] = {"Movie", "Interactive", "???", "???"};

    printf("  object type     : HDMV\n");
    printf("    playback type : %s\n", playback_types[hdmv->playback_type]);
    printf("    id_ref        : %u\n", hdmv->id_ref);
}

static void _indx_print_bdj_obj(INDX_BDJ_OBJ *bdj)
{
    const char * const playback_types[] = {"???", "???", "Movie", "Interactive"};

    printf("  object type     : BD-J\n");
    printf("    playback type : %s\n", playback_types[bdj->playback_type]);
    printf("    name          : %s\n", bdj->name);
}

static void _indx_print_play_item(INDX_PLAY_ITEM *title)
{
    if (title->object_type == 1) {
        _indx_print_hdmv_obj(&title->hdmv);
    } else {
        _indx_print_bdj_obj(&title->bdj);
    }
}

static void _indx_print_title(INDX_TITLE *title)
{
    if (title->object_type == 1) {
        _indx_print_hdmv_obj(&title->hdmv);
    } else {
        _indx_print_bdj_obj(&title->bdj);
    }
    printf("    access type   : %d\n", title->access_type);
}

static void _indx_print(INDX_ROOT *index)
{
    uint32_t i;

    printf("Application info:\n");
    _indx_print_app_info(&index->app_info);

    printf("\nFirst playback:\n");
    _indx_print_play_item(&index->first_play);

    printf("\nTop menu:\n");
    _indx_print_play_item(&index->top_menu);

    printf("\nTitles: %d\n", index->num_titles);
    for (i = 0; i < index->num_titles; i++) {
      printf("%02d", i);
      _indx_print_title(&index->titles[i]);
    }
}

int main(int argc, const char *argv[])
{
    char       file[1024];
    INDX_ROOT *index;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <disc_root>\n", argv[0]);
        return 1;
    }

    sprintf(file, "%s/BDMV/index.bdmv", argv[1]);

    index = indx_parse(file);

    if (index) {
        _indx_print(index);

        indx_free(&index);
    }

    return 0;
}
