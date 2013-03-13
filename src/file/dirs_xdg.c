/*
 * This file is part of libbluray
 * Copyright (C) 2013  Petri Hintukainen <phintuka@users.sourceforge.net>
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "file.h"

#include "util/strutl.h"
#include "util/logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Based on XDG Base Directory Specification
 * http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
 */

#define USER_CACHE_DIR ".cache"
#define USER_DATA_DIR  ".local/share"


const char *file_get_data_home(void)
{
    static char *dir       = NULL;
    static int   init_done = 0;

    if (!init_done) {
        init_done = 1;

        const char *xdg_home = getenv("XDG_DATA_HOME");
        if (xdg_home && *xdg_home) {
            return dir = str_printf("%s", xdg_home);
        }

        const char *user_home = getenv("HOME");
        if (user_home && *user_home) {
            return dir = str_printf("%s/%s", user_home, USER_DATA_DIR);
        }

        BD_DEBUG(DBG_FILE, "Can't find user home directory ($HOME) !\n");
    }

    return dir;
}

const char *file_get_cache_home(void)
{
    static char *dir       = NULL;
    static int   init_done = 0;

    if (!init_done) {
        init_done = 1;

        const char *xdg_cache = getenv("XDG_CACHE_HOME");
        if (xdg_cache && *xdg_cache) {
            return dir = str_printf("%s", xdg_cache);
        }

        const char *user_home = getenv("HOME");
        if (user_home && *user_home) {
            return dir = str_printf("%s/%s", user_home, USER_CACHE_DIR);
        }

        BD_DEBUG(DBG_FILE, "Can't find user home directory ($HOME) !\n");
    }

    return dir;
}
