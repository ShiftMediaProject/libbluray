
#include <dlfcn.h>

#include "bluray.h"
#include "util/macro.h"

BLURAY *bd_open(const char* device_path, const char* keyfile_path)
{
    BLURAY *bd = malloc(sizeof(BLURAY));

    // open aacs decryptor if present
    bd->aacs = NULL;
    if ((bd->libaacs_h = dlopen("libaacs", RTLD_NOW))) {
        typedef AACS_KEYS* (*fptr)();

        fptr fptr_s = dlsym(bd->libaacs_h, "aacs_open");
        bd->aacs = fptr_s(device_path, keyfile_path);
    }

    return bd;
}

void bd_close(BLURAY *bd)
{
    if (bd->libaacs_h) {
        aacs_close(bd->aacs);
    }

    X_FREE(bd);

    dlclose(bd->libbdplus_h);
    dlclose(bd->libaacs_h);
}
