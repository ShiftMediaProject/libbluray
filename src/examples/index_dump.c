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

#include "libbluray/bluray.h"

static void _indx_print_title(const BLURAY_TITLE *title, int normal_title)
{
    printf("    object type   : %s\n", title->bdj ? "BD-J" : "HDMV");
    printf("    playback type : %s\n", title->interactive ? "Interactive" : "Movie");
    printf(title->bdj ?
           "    name          : %05d.bdjo\n" :
           "    id_ref        : %u\n",
           title->id_ref);

    if (normal_title) {
        printf("    access type   : %s%s\n", title->accessible ? "Accessible" : "Prohibited", title->hidden ? ", Hidden" : "");
    }
}

static void _indx_print(const BLURAY_DISC_INFO *info)
{
    uint32_t i;

    printf("\nFirst playback:\n");
    if (info->first_play) {
        _indx_print_title(info->first_play, 0);
    } else {
        printf("    (not present)\n");
    }

    printf("\nTop menu:\n");
    if (info->top_menu) {
        _indx_print_title(info->titles[0], 0);
    } else {
        printf("    (not present)\n");
    }

    printf("\nTitles: %d\n", info->num_titles);
    for (i = 1; i <= info->num_titles; i++) {
        printf("%02d %s\n", i, info->titles[i]->name ? info->titles[i]->name : "");
        _indx_print_title(info->titles[i], 1);
    }
}

int main(int argc, const char *argv[])
{
    BLURAY *bd;
    const BLURAY_DISC_INFO *info;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <disc_root>\n", argv[0]);
        return 1;
    }

    bd = bd_open(argv[1], NULL);
    if (!bd) {
        fprintf(stderr, "error opening BD disc %s\n", argv[1]);
        return -1;
    }

    info = bd_get_disc_info(bd);

    if (!info->bluray_detected) {
        fprintf(stderr, "no BD disc detected in %s\n", argv[1]);
        bd_close(bd);
        return -1;
    }

    _indx_print(info);

    bd_close(bd);

    return 0;
}
