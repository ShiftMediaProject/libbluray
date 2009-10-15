
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
        typedef void* (*fptr)();

        fptr fptr_s = dlsym(bd->libaacs_h, "aacs_close");
        fptr_s(bd->aacs);
    }

    X_FREE(bd);

    dlclose(bd->libaacs_h);
}

off_t bd_seek(BLURAY *bd, off_t pos)
{
    if (pos < bd->s_size) {
        bd->s_pos = pos - (pos % 6144);

        file_seek(bd->fp, bd->s_pos, SEEK_SET);
    }

    return bd->s_pos;
}

int bd_read(BLURAY *bd, unsigned char *buf, int len)
{
    if (len + bd->s_pos < bd->s_size) {
        int read;

        if ((read = file_read(bd->fp, buf, len))) {
            if (bd->libaacs_h) {
                typedef int* (*fptr)();

                fptr fptr_s = dlsym(bd->libaacs_h, "aacs_decrypt_unit");
                if (!fptr_s(bd->aacs, buf, len)) {
                    return 0;
                }
            }

            bd->s_pos += len;

            return read;
        }
    }

    return 0;
}
