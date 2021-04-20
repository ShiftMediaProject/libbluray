/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  Obliter0n
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_DLADDR
#  define _GNU_SOURCE  /* dladdr */
#endif

#include "dl.h"
#include "util/macro.h"
#include "util/logging.h"
#include "util/strutl.h"

#ifdef __APPLE__
// Required to make dladdr available
#    define _DARWIN_C_SOURCE
#endif

#if defined(HAVE_DLFCN_H)
#   include <dlfcn.h>
#elif defined(HAVE_SYS_DL_H)
#   include <sys/dl.h>
#endif

#include <string.h>

static void *_dl_dlopen(const char *path)
{
    void *result;

    result = dlopen(path, RTLD_LAZY);

    if (!result) {
        BD_DEBUG(DBG_FILE, "can't open library '%s': %s\n", path, dlerror());
    } else {
        BD_DEBUG(DBG_FILE, "opened library '%s'\n", path);
    }

    return result;
}

void *dl_dlopen(const char *path, const char *version)
{
    char *name;
    void *dll;
    int i;

#if defined(__APPLE__)
    static const char ext[] = ".dylib";
    /*
      Search for the library in several locations:
       ""               - default search path (including DYLD_LIBRARY_PATH)
       @loader_path     - location of current library/binary (ex. libbluray.dylib)
       @executable_path - location of running binary (ex. /Applications/Some.app/Contents/MacOS)
       @rpath           - search rpaths of running binary (man install_name_path)
       /usr/local/lib/  - explicitly added path, as runtime hardened programs ignore DYLD_FALLBACK_PATH now
    */
    static const char *search_paths[] = {"", "@loader_path/lib/", "@loader_path/", "@executable_path/",
                                         "@executable_path/lib/", "@executable_path/../lib/",
                                         "@executable_path/../Resources/", "@rpath/", "/usr/local/lib/", NULL};
    version = NULL;
#else
    static const char ext[] = ".so";
    static const char *search_paths[] = {"", NULL};
#endif

    for (i = 0 ; search_paths[i] ; ++i) {
        if (version) {
            name = str_printf("%s%s%s.%s", search_paths[i], path, ext, version);
        } else {
            name = str_printf("%s%s%s", search_paths[i], path, ext);
        }

        if (!name) {
            BD_DEBUG(DBG_FILE | DBG_CRIT, "out of memory\n");
            continue;
        }

        dll = _dl_dlopen (name);
        X_FREE(name);
        if (dll) {
            return dll;
        }
    }

    return NULL;
}

void *dl_dlsym(void *handle, const char *symbol)
{
    void *result = dlsym(handle, symbol);

    if (!result) {
        BD_DEBUG(DBG_FILE, "dlsym(%p, '%s') failed: %s\n", handle, symbol, dlerror());
    }

    return result;
}

int dl_dlclose(void *handle)
{
    return dlclose(handle);
}

#define PATH_SEPARATOR '/'
const char *dl_get_path(void)
{
    static char *lib_path    = NULL;
    static int   initialized = 0;

    if (!initialized) {
        initialized = 1;

#if defined(__APPLE__) || defined(HAVE_DLADDR)
        Dl_info dl_info;
        int ret = dladdr((void *)dl_get_path, &dl_info);
        if (ret != 0) {
            lib_path = strdup(dl_info.dli_fname);

            /* cut library name from path */
            char *p = strrchr(lib_path, PATH_SEPARATOR);
            if (p) {
                *(p+1) = 0;
            }
            BD_DEBUG(DBG_FILE, "library file is %s\n", lib_path);
        } else {
            BD_DEBUG(DBG_FILE, "Can't determine libbluray.so install path\n");
        }
#else
        BD_DEBUG(DBG_FILE, "Can't determine libbluray.so install path\n");
#endif
    }

    return lib_path;
}
