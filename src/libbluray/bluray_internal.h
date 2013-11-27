/*
 * This file is part of libbluray
 * Copyright (C) 2012  libbluray
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

#include <util/attributes.h>

#include "bluray.h"

#include <stdint.h>

BD_PRIVATE const uint8_t *bd_get_aacs_data(BLURAY *bd, int type);

BD_PRIVATE uint64_t bd_get_uo_mask(BLURAY *bd);
BD_PRIVATE int bd_play_title_internal(BLURAY *bd, unsigned title);

BD_PRIVATE uint32_t bd_reg_read(BLURAY *bd, int psr, int reg);
BD_PRIVATE int bd_reg_write(BLURAY *bd, int psr, int reg, uint32_t value);

#endif  /* _BLURAY_INTERNAL_H_ */
