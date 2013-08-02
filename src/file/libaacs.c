/*
 * This file is part of libbluray
 * Copyright (C) 2013       VideoLAN
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

#include "libaacs.h"

#include "dl.h"
#include "file.h"
#include "util/logging.h"
#include "util/macro.h"
#include "util/strutl.h"

#include <stdlib.h>


struct bd_aacs {
    void           *h_libaacs;   /* library handle from dlopen */
    void           *aacs;        /* aacs handle from aacs_open() */

    const uint8_t *disc_id;
    uint32_t       mkbv;

    /* function pointers */
    fptr_int       decrypt_unit;

    fptr_p_void    get_vid;
    fptr_p_void    get_pmsn;
    fptr_p_void    get_device_binding_id;
};


static void _libaacs_close(BD_AACS *p)
{
    if (p->aacs) {
        DL_CALL(p->h_libaacs, aacs_close, p->aacs);
        p->aacs = NULL;
    }
}

void libaacs_unload(BD_AACS **p)
{
    if (p && *p) {
        _libaacs_close(*p);

        if ((*p)->h_libaacs) {
            dl_dlclose((*p)->h_libaacs);
        }

        X_FREE(*p);
    }
}

int libaacs_required(const char *device_path)
{
    BD_FILE_H *fd;
    char      *tmp;

    tmp = str_printf("%s/AACS/Unit_Key_RO.inf", device_path);
    fd = file_open(tmp, "rb");
    X_FREE(tmp);

    if (fd) {
        file_close(fd);

        BD_DEBUG(DBG_BLURAY, "AACS/Unit_Key_RO.inf found. Disc seems to be AACS protected.\n");
        return 1;
    }

    BD_DEBUG(DBG_BLURAY, "AACS/Unit_Key_RO.inf not found. No AACS protection.\n");
    return 0;
}

BD_AACS *libaacs_load(void)
{
    BD_AACS *p = calloc(1, sizeof(BD_AACS));

    p->h_libaacs = dl_dlopen("libaacs", "0");
    if (!p->h_libaacs) {
        BD_DEBUG(DBG_BLURAY | DBG_CRIT, "libaacs not found!\n");
        X_FREE(p);
        return NULL;
    }

    BD_DEBUG(DBG_BLURAY, "Loading libaacs (%p)\n", p->h_libaacs);

    *(void **)(&p->decrypt_unit) = dl_dlsym(p->h_libaacs, "aacs_decrypt_unit");
    *(void **)(&p->get_vid)      = dl_dlsym(p->h_libaacs, "aacs_get_vid");
    *(void **)(&p->get_pmsn)     = dl_dlsym(p->h_libaacs, "aacs_get_pmsn");
    *(void **)(&p->get_device_binding_id) = dl_dlsym(p->h_libaacs, "aacs_get_device_binding_id");

    if (!p->decrypt_unit) {
        BD_DEBUG(DBG_BLURAY | DBG_CRIT, "libaacs dlsym failed! (%p)\n", p->h_libaacs);
        libaacs_unload(&p);
        return NULL;
    }

    BD_DEBUG(DBG_BLURAY, "Loaded libaacs (%p)\n", p->h_libaacs);

    if (file_open != file_open_default()) {
        BD_DEBUG(DBG_BLURAY, "Registering libaacs filesystem handler %p (%p)\n", (void *)(intptr_t)file_open, p->h_libaacs);
        DL_CALL(p->h_libaacs, aacs_register_file, file_open);
    }

    return p;
}

int libaacs_open(BD_AACS *p, const char *device_path, const char *keyfile_path)
{
    int error_code = 0;

    fptr_p_void open;
    fptr_p_void open2;
    fptr_int    aacs_get_mkb_version;
    fptr_p_void aacs_get_disc_id;

    _libaacs_close(p);

    *(void **)(&open)  = dl_dlsym(p->h_libaacs, "aacs_open");
    *(void **)(&open2) = dl_dlsym(p->h_libaacs, "aacs_open2");
    *(void **)(&aacs_get_mkb_version) = dl_dlsym(p->h_libaacs, "aacs_get_mkb_version");
    *(void **)(&aacs_get_disc_id)     = dl_dlsym(p->h_libaacs, "aacs_get_disc_id");

    if (open2) {
        p->aacs = open2(device_path, keyfile_path, &error_code);
    } else if (open) {
        BD_DEBUG(DBG_BLURAY, "Using old aacs_open(), no verbose error reporting available\n");
        p->aacs = open(device_path, keyfile_path);
    } else {
        BD_DEBUG(DBG_BLURAY, "aacs_open() not found\n");
    }

    if (p->aacs) {
        if (aacs_get_mkb_version) {
            p->mkbv = aacs_get_mkb_version(p->aacs);
        }
        if (aacs_get_disc_id) {
            p->disc_id = (const uint8_t *)aacs_get_disc_id(p->aacs);
        }
        return error_code;
    }

    return error_code ? error_code : 1;
}

void libaacs_select_title(BD_AACS *p, uint32_t title)
{
    if (p && p->aacs) {
        DL_CALL(p->h_libaacs, aacs_select_title, p->aacs, title);
    }
}

int libaacs_decrypt_unit(BD_AACS *p, uint8_t *buf)
{
    if (p && p->aacs) {
        if (!p->decrypt_unit(p->aacs, buf)) {
            BD_DEBUG(DBG_AACS | DBG_CRIT, "Unable decrypt unit (AACS)!\n");

            return -1;
        } // decrypt
    } // aacs

    return 0;
}

const uint8_t *libaacs_get_vid(BD_AACS *p)
{
    if (!p || !p->aacs) {
        BD_DEBUG(DBG_BLURAY | DBG_CRIT, "bd_get_vid(): libaacs not initialized!\n");
        return NULL;
    }
    if (!p->get_vid) {
        BD_DEBUG(DBG_BLURAY | DBG_CRIT, "aacs_get_vid() dlsym failed!\n");
        return NULL;
    }

    return (const uint8_t*)p->get_vid(p->aacs);
}

const uint8_t *libaacs_get_pmsn(BD_AACS *p)
{
    if (!p || !p->aacs) {
        BD_DEBUG(DBG_BLURAY | DBG_CRIT, "libaacs_get_pmsn(): libaacs not initialized!\n");
        return NULL;
    }
    if (!p->get_pmsn) {
        BD_DEBUG(DBG_BLURAY | DBG_CRIT, "aacs_get_pmsn() dlsym failed!\n");
        return NULL;
    }

    return (const uint8_t*)p->get_pmsn(p->aacs);
}

const uint8_t *libaacs_get_device_binding_id(BD_AACS *p)
{
    if (!p || !p->aacs) {
        BD_DEBUG(DBG_BLURAY | DBG_CRIT, "bd_get_device_binding_id(): libaacs not initialized!\n");
        return NULL;
    }
    if (!p->get_device_binding_id) {
        BD_DEBUG(DBG_BLURAY | DBG_CRIT, "aacs_get_device_binding_id() dlsym failed!\n");
        return NULL;
    }

    return (const uint8_t*)p->get_device_binding_id(p->aacs);
}

const uint8_t *libaacs_get_disc_id(BD_AACS *p)
{
    return p ? p->disc_id : NULL;
}

uint32_t libaacs_get_mkbv(BD_AACS *p)
{
    return p ? p->mkbv : 0;
}
