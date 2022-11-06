/*
 * This file is part of libbluray
 * Copyright (C) 2014  VideoLAN
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

#include "mount.h"

#include "util/strutl.h"

#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_GETFSSTAT
#include <sys/mount.h>
#else
#include <sys/statvfs.h>
#endif

char *mount_get_mountpoint(const char *device_path)
{
    struct stat st;
#ifdef HAVE_GETFSSTAT
    struct statfs *mbuf;
#else
    struct statvfs *mbuf;
#endif
    int fs_count;

    if (stat (device_path, &st) ) {
        return str_dup(device_path);
    }

    /* If it's a directory, all is good */
    if (S_ISDIR(st.st_mode)) {
        return str_dup(device_path);
    }

#ifdef HAVE_GETFSSTAT
    fs_count = getfsstat (NULL, 0, MNT_NOWAIT);
#else
    fs_count = getvfsstat (NULL, 0, ST_NOWAIT);
#endif
    if (fs_count == -1) {
        return str_dup(device_path);
    }

#ifdef HAVE_GETFSSTAT
    mbuf = calloc (fs_count, sizeof(struct statfs));
#else
    mbuf = calloc (fs_count, sizeof(struct statvfs));
#endif
    if (!mbuf) {
        return str_dup(device_path);
    }

#ifdef HAVE_GETFSSTAT
    fs_count = getfsstat (mbuf, fs_count * sizeof(struct statfs), MNT_NOWAIT);
#else
    fs_count = getvfsstat (mbuf, fs_count * sizeof(struct statvfs), ST_NOWAIT);
#endif

    char *result = NULL;

    for (int i = 0; i < fs_count; ++i) {
        if (!strcmp (mbuf[i].f_mntfromname, device_path)) {
            result = str_dup(mbuf[i].f_mntonname);
            break;
        }
    }

    free (mbuf);
    return (result) ? result : str_dup(device_path);
}
