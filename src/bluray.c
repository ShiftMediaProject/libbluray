#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_MALLOC_H
#include <malloc.h>
#endif

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include <stdio.h>

#include "bluray.h"
#include "util/macro.h"
#include "util/logging.h"
#include "util/strutl.h"
#include "file/dl.h"

BLURAY *bd_open(const char* device_path, const char* keyfile_path)
{
    BLURAY *bd = calloc(1, sizeof(BLURAY));

    if (device_path) {

        bd->device_path = strdup(device_path);

        bd->aacs = NULL;
        bd->h_libaacs = NULL;
        bd->fp = NULL;

        if (keyfile_path) {
            //if ((bd->h_libaacs = dlopen("libaacs.so", RTLD_LAZY))) {
            if ((bd->h_libaacs = dl_dlopen("aacs"))) {
                fptr_p_void fptr;
                uint8_t *vid;

                DEBUG(DBG_BLURAY, "Downloaded libaacs (0x%08x)\n", bd->h_libaacs);

                fptr = dl_dlsym(bd->h_libaacs, "aacs_open");
                bd->aacs = fptr(device_path, keyfile_path);
            } else {
                DEBUG(DBG_BLURAY, "libaacs not found!\n");
            }
        } else {
            DEBUG(DBG_BLURAY | DBG_CRIT, "No keyfile provided. You will not be able to make use of crypto functionality (0x%08x)\n", bd);
        }


        // Take a quick stab to see if we want/need bdplus
        // we should fix this, and add various string functions.
        {
            uint8_t vid[16] = {
                0xC5,0x43,0xEF,0x2A,0x15,0x0E,0x50,0xC4,0xE2,0xCA,
                0x71,0x65,0xB1,0x7C,0xA7,0xCB}; // FIXME
            FILE_H *fd;
            char *tmp = NULL;
            tmp = str_printf("%s/BDSVM/00000.svm", bd->device_path);
            if ((fd = file_open(tmp, "rb"))) {
                file_close(fd);

                DEBUG(DBG_BDPLUS, "attempting to load libbdplus\n");
                if ((bd->h_libbdplus = dl_dlopen("bdplus"))) {
                    DEBUG(DBG_BLURAY, "Downloaded libbdplus (0x%08x)\n",
                          bd->h_libbdplus);

                    fptr_p_void bdplus_init = dl_dlsym(bd->h_libbdplus, "bdplus_init");
                    //bdplus_t *bdplus_init(path,configfile_path,*vid );
                    if (bdplus_init)
                        bd->bdplus = bdplus_init(device_path, keyfile_path, vid);

                    // Since we will call these functions a lot, we assign them
                    // now.
                    bd->bdplus_seek  = dl_dlsym(bd->h_libbdplus, "bdplus_seek");
                    bd->bdplus_fixup = dl_dlsym(bd->h_libbdplus, "bdplus_fixup");

                } // dlopen
            } // file_open
            X_FREE(tmp);
        }



        DEBUG(DBG_BLURAY, "BLURAY initialized! (0x%08x)\n", bd);
    } else {
        X_FREE(bd);

        DEBUG(DBG_BLURAY | DBG_CRIT, "No device path provided!\n");
    }

    return bd;
}

void bd_close(BLURAY *bd)
{
    if (bd->h_libaacs && bd->aacs) {
        fptr_p_void fptr = dl_dlsym(bd->h_libaacs, "aacs_close");
        fptr(bd->aacs);  // FIXME: NULL

        dl_dlclose(bd->h_libaacs);
    }

    if (bd->h_libbdplus && bd->bdplus) {
        fptr_p_void bdplus_free = dl_dlsym(bd->h_libbdplus, "bdplus_free");
        if (bdplus_free) bdplus_free(bd->bdplus);
        bd->bdplus = NULL;

        dl_dlclose(bd->h_libbdplus);
        bd->h_libbdplus = NULL;

        bd->bdplus_seek  = NULL;
        bd->bdplus_fixup = NULL;
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

        bd->int_buf_off = 6144;

        DEBUG(DBG_BLURAY, "Seek to %"PRIu64" (0x%08x)\n",
              bd->s_pos, bd);
        if (bd->bdplus_seek && bd->bdplus)
            bd->bdplus_seek(bd->bdplus, bd->s_pos);

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

                    if ((read_len = file_read(bd->fp, buf, len))) {
                        if (bd->h_libaacs && bd->aacs) {
                            // FIXME: calling dlsym for every read() call.
                            if ((bd->libaacs_decrypt_unit = dl_dlsym(bd->h_libaacs, "aacs_decrypt_unit"))) {
                                if (!bd->libaacs_decrypt_unit(bd->aacs, buf, len, bd->s_pos)) {
                                    DEBUG(DBG_BLURAY, "Unable decrypt unit! (0x%08x)\n", bd);

                                    return 0;
                                } // decrypt
                            } // dlsym
                        } // aacs

                        // bdplus fixup, if required.
                        if (bd->bdplus_fixup && bd->bdplus) {
                            int32_t numFixes;
                            numFixes = bd->bdplus_fixup(bd->bdplus,
                                                        len,
                                                        buf);
#if 0
                            if (numFixes) {
                                DEBUG(DBG_BLURAY,
                                      "BDPLUS did %u fixups\n", numFixes);
                            }
#endif
                        }

                        bd->s_pos += len;

                        DEBUG(DBG_BLURAY, "%d bytes read OK! (0x%08x)\n", len, bd);

                        return len;
                    } // read
                } // int_buf
                        bd->int_buf_off = 0;

            } // while
        } // s_size
    } // if ->fp
    DEBUG(DBG_BLURAY, "No valid title selected! (0x%08x)\n", bd->s_pos);

    return 0;
}

int bd_select_title(BLURAY *bd, uint32_t title)
{
    char f_name[100];

    memset(f_name, 0, sizeof(f_name));
    snprintf(f_name, 100, "%s/BDMV/STREAM/%05u.m2ts", bd->device_path, title);

    bd->s_size = 0;
    bd->s_pos = 0;

    if ((bd->fp = file_open(f_name, "rb"))) {
        file_seek(bd->fp, 0, SEEK_END);
        if ((bd->s_size = file_tell(bd->fp))) {

            bd_seek(bd, 0);

            DEBUG(DBG_BLURAY, "Title %s selected! (0x%08x)\n", f_name, bd);

            if (bd->h_libbdplus && bd->bdplus) {
                fptr_p_void bdplus_set_title;
                bdplus_set_title = dl_dlsym(bd->h_libbdplus, "bdplus_set_title");
                if (bdplus_set_title)
                    bdplus_set_title(bd->bdplus, title);
            }

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
