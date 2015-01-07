/*
 * This file is part of libbluray
 * Copyright (C) 2014  Petri Hintukainen <phintuka@users.sourceforge.net>
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

#if !defined(_BD_DISC_H_)
#define _BD_DISC_H_

#include "util/attributes.h"

#include <stdint.h>

struct bd_file_s;
struct bd_dir_s;

/*
 * BluRay Virtual File System
 *
 * Map file access to BD-ROM file system or binding unit data area
 */

typedef struct bd_disc BD_DISC;

BD_PRIVATE BD_DISC *disc_open(const char *device_path);
BD_PRIVATE void     disc_close(BD_DISC **);

/* Get BD-ROM root path */
BD_PRIVATE const char *disc_root(BD_DISC *disc);

/* Get BD-ROM device path */
BD_PRIVATE const char *disc_device(BD_DISC *disc);

/* Check if file exists in BD-ROM disc (not VFS) */
BD_PRIVATE int disc_have_file(BD_DISC *p, const char *dir, const char *file);

/* Open VFS file (relative to disc root) */
BD_PRIVATE struct bd_file_s *disc_open_file(BD_DISC *disc, const char *dir, const char *file);
BD_PRIVATE struct bd_file_s *disc_open_path(BD_DISC *disc, const char *path);

/* Open VFS directory (relative to disc root) */
BD_PRIVATE struct bd_dir_s *disc_open_dir (BD_DISC *disc, const char *dir);

/* Read VFS file */
BD_PRIVATE int64_t disc_read_file(BD_DISC *disc, const char *dir, const char *file,
                                  uint8_t **data);

#endif /* _BD_DISC_H_ */
