/*
 * This file is part of libbluray
 * Copyright (C) 2012-2014  libbluray
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

#if !defined(_BLURAY_INTERNAL_H_)
#define _BLURAY_INTERNAL_H_

#include "util/attributes.h"

#include "bluray.h"

#include <stdint.h>

BD_PRIVATE const uint8_t *bd_get_aacs_data(BLURAY *bd, int type);

/* bd_set_bdj_uo_mask() */
#define BDJ_MENU_CALL_MASK     0x01
#define BDJ_TITLE_SEARCH_MASK  0x02

BD_PRIVATE uint64_t bd_get_uo_mask(BLURAY *bd);
BD_PRIVATE void     bd_set_bdj_uo_mask(struct bluray *bd, unsigned mask);

BD_PRIVATE int bd_play_title_internal(BLURAY *bd, unsigned title);

BD_PRIVATE uint32_t bd_reg_read(BLURAY *bd, int psr, int reg);
BD_PRIVATE int bd_reg_write(BLURAY *bd, int psr, int reg, uint32_t value, uint32_t psr_value_mask);

enum bd_select_rate_reason {
    BDJ_RATE_SET       = 0,
    BDJ_PLAYBACK_START = 1,
    BDJ_PLAYBACK_STOP  = 2,
};
BD_PRIVATE void bd_select_rate(BLURAY *bd, float rate, int reason);

BD_PRIVATE int bd_play_playlist_at(BLURAY *bd, int playlist, int playitem, int playmark, int64_t time);

/* BD-J overlay */

struct bd_argb_buffer_s;
BD_PRIVATE struct bd_argb_buffer_s *bd_lock_osd_buffer(struct bluray *bd);
BD_PRIVATE void                     bd_unlock_osd_buffer(struct bluray *bd);

BD_PRIVATE void  bd_bdj_osd_cb(struct bluray *bd, const unsigned *img, int w, int h,
                               int x0, int y0, int x1, int y1);

#endif  /* _BLURAY_INTERNAL_H_ */
