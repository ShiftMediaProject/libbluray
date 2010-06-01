/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  cRTrn13
 * Copyright (C) 2009-2010  John Stebbins
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders of libbluray
 * gives permission to link the code of its release of libbluray with the
 * OpenSSL project's "OpenSSL" library (or with modified versions of it
 * that use the same license as the "OpenSSL" library), and distribute
 * the linked executables.  You must obey the GNU General Public License
 * in all respects for all of the code used other than "OpenSSL".  If you
 * modify this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to do
 * so, delete this exception statement from your version.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "dl.h"
#include "util/macro.h"
#include "util/logging.h"

#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

void   *dl_dlopen  ( const char* path )
{
    DEBUG(DBG_BDPLUS, "searching for library '%s' ...\n", path);
    void *result = dlopen(path, RTLD_LAZY);
    if (!result) {
        DEBUG(DBG_FILE | DBG_CRIT, "can't open library '%s': %s\n", path, dlerror());
    }
    return result;
}

void   *dl_dlsym   ( void* handle, const char* symbol )
{
    void *result = dlsym(handle, symbol);

    if (!result) {
      DEBUG(DBG_FILE | DBG_CRIT, "dlsym(%p, '%s') failed: %s\n", handle, symbol, dlerror());
    }

    return result;
}

int     dl_dlclose ( void* handle )
{
    return dlclose(handle);
}
