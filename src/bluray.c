#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_MALLOC_H
#include <malloc.h>
#endif

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <dlfcn.h>
#include <stdio.h>

#include "bluray.h"
#include "util/macro.h"
#include "util/logging.h"

BLURAY *bd_open(const char* device_path, const char* keyfile_path)
{
    BLURAY *bd = calloc(1, sizeof(BLURAY));

    if (device_path) {
        bd->device_path = strdup(device_path);

        bd->aacs = NULL;
        bd->h_libaacs = NULL;
        bd->fp = NULL;

        if (keyfile_path) {
            if ((bd->h_libaacs = dlopen("libaacs.so", RTLD_LAZY))) {
                fptr_p_void fptr;
                uint8_t *vid;

                DEBUG(DBG_BLURAY, "Downloaded libaacs (0x%08x)\n", bd->h_libaacs);

                if ((fptr = dlsym(bd->h_libaacs, "aacs_open"))) {
                    if ((bd->aacs = fptr(device_path, keyfile_path))) {
                        if ((fptr = dlsym(bd->h_libaacs, "aacs_get_vid"))) {
                            vid = fptr(bd->aacs);
                        }

                        if ((bd->h_libbdplus = dlopen("libbdplus.so", RTLD_LAZY))) {
                            DEBUG(DBG_BLURAY, "Downloaded libbdplus (0x%08x)\n", bd->h_libbdplus);

                            if ((fptr = dlsym(bd->h_libbdplus, "bdplus_open"))) {
                                if (!(bd->bdplus = fptr(device_path, keyfile_path, vid))) {
                                    DEBUG(DBG_BLURAY, "libbdplus ret NULL!\n");
                                }
                            }
                        } else {
                            DEBUG(DBG_BLURAY, "libbdplus not found!\n");
                        }
                    } else {
                        DEBUG(DBG_BLURAY | DBG_CRIT, "libaacs failed to initialize! If this disc is encrypted, you will not be able to play it\n");
                    }
                }
            } else {
                DEBUG(DBG_BLURAY, "libaacs not found!\n");
            }
        } else {
            DEBUG(DBG_BLURAY | DBG_CRIT, "No keyfile provided. You will not be able to make use of crypto functionality (0x%08x)\n", bd);
        }

        bd->int_buf_off = 6144;

        DEBUG(DBG_BLURAY, "BLURAY initialized! (0x%08x)\n", bd);
    } else {
        X_FREE(bd);

        DEBUG(DBG_BLURAY | DBG_CRIT, "No device path provided!\n");
    }

    return bd;
}

void bd_close(BLURAY *bd)
{
    if (bd->h_libaacs) {
        if (bd->aacs) {
            fptr_p_void fptr = dlsym(bd->h_libaacs, "aacs_close");
            fptr(bd->aacs);
        }

        dlclose(bd->h_libaacs);
    }

    if (bd->h_libbdplus) {
        if (bd->bdplus) {
            fptr_p_void fptr = dlsym(bd->h_libbdplus, "bdplus_close");
            fptr(bd->bdplus);
        }

        dlclose(bd->h_libaacs);
    }

    if (bd->fp) {
        file_close(bd->fp);
    }

    X_FREE(bd->device_path);

    DEBUG(DBG_BLURAY, "BLURAY destroyed! (0x%08x)\n", bd);

    X_FREE(bd);
}

uint64_t bd_seek(BLURAY *bd, uint64_t pos)
{
    if (pos < bd->s_size) {
        bd->s_pos = pos - (pos % 6144);

        file_seek(bd->fp, bd->s_pos, SEEK_SET);

        if (bd->h_libbdplus && bd->bdplus) {
            fptr_p_void fptr = dlsym(bd->h_libbdplus, "bdplus_seek");
            fptr(bd->bdplus, bd->s_pos);
        }

        bd->int_buf_off = 6144;

        DEBUG(DBG_BLURAY, "Seek to %ld (0x%08x)\n", bd->s_pos, bd);
    }

    return bd->s_pos;
}

int bd_read(BLURAY *bd, unsigned char *buf, int len)
{
    if (bd->fp) {
        int out_len = 0;

        DEBUG(DBG_BLURAY, "Reading unit [%d bytes] at %ld... (0x%08x)\n", len, bd->s_pos, bd);

        if (len + bd->s_pos <= bd->s_size) {
            while (out_len < len) {
                if (bd->int_buf_off == 6144) {
                    int read_len;

                    if ((read_len = file_read(bd->fp, bd->int_buf, 6144)) == 6144) {
                        if (bd->h_libaacs && bd->aacs) {
                            if ((bd->libaacs_decrypt_unit = dlsym(bd->h_libaacs, "aacs_decrypt_unit"))) {
                                if (!bd->libaacs_decrypt_unit(bd->aacs, bd->int_buf, read_len, 0)) {
                                    DEBUG(DBG_BLURAY, "Unable decrypt unit! (0x%08x)\n", bd);

                                    return 0;
                                }

                                if (bd->h_libbdplus && bd->bdplus) {
                                    fptr_p_void fptr = dlsym(bd->h_libbdplus, "bdplus_fixup");
                                    fptr(bd->bdplus, bd->int_buf, read_len);
                                }
                            }
                        }
                    }
                    bd->int_buf_off = 0;
                }

                buf[out_len++] = bd->int_buf[bd->int_buf_off++];
            }

            bd->s_pos += len;

            DEBUG(DBG_BLURAY, "%d bytes read OK! (0x%08x)\n", len, bd);

            return len;
        }
    }

    DEBUG(DBG_BLURAY, "No valid title selected! (0x%08x)\n", bd->s_pos);

    return 0;
}

int bd_select_title(BLURAY *bd, uint64_t title)
{
    char f_name[100];

    memset(f_name, 0, sizeof(f_name));
    snprintf(f_name, 100, "%s/BDMV/STREAM/%05ld.m2ts", bd->device_path, title);

    bd->s_size = 0;
    bd->s_pos = 0;

    if ((bd->fp = file_open(f_name, "rb"))) {
        file_seek(bd->fp, 0, SEEK_END);
        if ((bd->s_size = file_tell(bd->fp))) {
            if (bd->h_libbdplus && bd->bdplus) {
                fptr_p_void fptr = dlsym(bd->h_libbdplus, "bdplus_set_title");
                fptr(bd->bdplus, title);
            }

            bd_seek(bd, 0);

            DEBUG(DBG_BLURAY, "Title %s selected! (0x%08x)\n", f_name, bd);

            return 1;
        }

        DEBUG(DBG_BLURAY, "Title %s empty! (0x%08x)\n", f_name, bd);
    }

    DEBUG(DBG_BLURAY | DBG_CRIT, "Unable to select title %s! (0x%08x)\n", f_name, bd);

    return 0;
}

uint64_t bd_get_title_size(BLURAY *bd)
{
    return bd->s_size;
}

uint64_t bd_tell(BLURAY *bd)
{
    return bd->s_pos;
}
