/*
 * This file is part of libbluray
 * Copyright (C) 2014  Petri Hintukainen
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

#include "file.h"

#include <stdio.h>

int64_t file_size(BD_FILE_H *fp)
{
    int64_t pos    = file_tell(fp);
    int64_t res1   = file_seek(fp, 0, SEEK_END);
    int64_t length = file_tell(fp);
    int64_t res2   = file_seek(fp, pos, SEEK_SET);

    if (res1 < 0 || res2 < 0 || pos < 0 || length < 0) {
        return -1;
    }

    return length;
}
