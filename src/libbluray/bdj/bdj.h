/*
 * This file is part of libbluray
 * Copyright (C) 2010  William Hahne
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

#ifndef BDJ_H_
#define BDJ_H_

#include "common.h"

typedef struct bdjava_s BDJAVA;

struct bluray;
struct bd_registers_s;
struct indx_root_s;

BDJAVA* bdj_open(const char *path, struct bluray *bd,
                 struct bd_registers_s *registers, struct indx_root_s *index);
int bdj_start(BDJAVA *bdjava, const char *start);
void bdj_stop(BDJAVA *bdjava);
void bdj_close(BDJAVA *bdjava);

void bdj_send_event(BDJAVA *bdjava, int type, int keyCode);

#endif
