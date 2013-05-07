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

#if !defined(_BD_UO_MASK_TABLE_H_)
#define _BD_UO_MASK_TABLE_H_

#include <stdint.h>

typedef struct bd_uo_mask_table_s
{
    unsigned int menu_call : 1;
    unsigned int title_search : 1;
    unsigned int chapter_search : 1;
    unsigned int time_search : 1;
    unsigned int skip_to_next_point : 1;
    unsigned int skip_to_prev_point : 1;
    unsigned int play_firstplay : 1;
    unsigned int stop : 1;
    unsigned int pause_on : 1;
    unsigned int pause_off : 1;
    unsigned int still : 1;
    unsigned int forward : 1;
    unsigned int backward : 1;
    unsigned int resume : 1;
    unsigned int move_up : 1;
    unsigned int move_down : 1;
    unsigned int move_left : 1;
    unsigned int move_right : 1;
    unsigned int select : 1;
    unsigned int activate : 1;
    unsigned int select_and_activate : 1;
    unsigned int primary_audio_change : 1;
    unsigned int reserved0 : 1;
    unsigned int angle_change : 1;
    unsigned int popup_on : 1;
    unsigned int popup_off : 1;
    unsigned int pg_enable_disable : 1;
    unsigned int pg_change : 1;
    unsigned int secondary_video_enable_disable : 1;
    unsigned int secondary_video_change : 1;
    unsigned int secondary_audio_enable_disable : 1;
    unsigned int secondary_audio_change : 1;
    unsigned int reserved1 : 1;
    unsigned int pip_pg_change : 1;
} BD_UO_MASK;

static inline BD_UO_MASK bd_uo_mask_combine(BD_UO_MASK a, BD_UO_MASK b)
{
    BD_UO_MASK o;
    unsigned int   *pa = (unsigned int*)&a;
    unsigned int   *pb = (unsigned int*)&b;
    unsigned int   *po = (unsigned int*)&o;
    unsigned   i;

    for (i = 0; i < sizeof(BD_UO_MASK) / sizeof(unsigned int); i++) {
        po[i] = pa[i] | pb[i];
    }

    return o;
}

#endif // _BD_UO_MASK_TABLE_H_
