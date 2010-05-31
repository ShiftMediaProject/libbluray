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
