/*
 * This file is part of libbluray
 * Copyright (C) 2014  Petri Hintukainen <phintuka@users.sourceforge.net>
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

#include "disc.h"

#include "dec.h"

#include "util/logging.h"
#include "util/macro.h"
#include "util/mutex.h"
#include "util/strutl.h"
#include "file/file.h"
#include "file/mount.h"

#include <string.h>

struct bd_disc {
    BD_MUTEX  ovl_mutex;     /* protect access to overlay root */

    char     *disc_root;     /* disc filesystem root (if disc is mounted) */
    char     *disc_device;   /* disc device (if using real device) */
    char     *overlay_root;  /* overlay filesystem root (if set) */

    BD_DEC   *dec;
};

/*
 * BD-ROM filesystem
 */

static BD_FILE_H *_bdrom_open_path(void *p, const char *rel_path)
{
    BD_DISC *disc = (BD_DISC *)p;
    BD_FILE_H *fp;
    char *abs_path;

    abs_path = str_printf("%s%s", disc->disc_root, rel_path);
    fp = file_open(abs_path, "rb");
    X_FREE(abs_path);

    return fp;
}

static BD_DIR_H *_bdrom_open_dir(BD_DISC *p, const char *dir)
{
    BD_DIR_H *dp;
    char *path;

    path = str_printf("%s%s", p->disc_root, dir);
    dp = dir_open(path);
    X_FREE(path);

    return dp;
}

/*
 * overlay filesystem
 */

static BD_FILE_H *_overlay_open_path(BD_DISC *p, const char *rel_path)
{
    BD_FILE_H *fp = NULL;

    bd_mutex_lock(&p->ovl_mutex);

    if (p->overlay_root) {
        char *abs_path = str_printf("%s%s", p->overlay_root, rel_path);
        fp = file_open_default()(abs_path, "rb");
        X_FREE(abs_path);
    }

    bd_mutex_unlock(&p->ovl_mutex);

    return fp;
}

static BD_DIR_H *_overlay_open_dir(BD_DISC *p, const char *dir)
{
    BD_DIR_H *dp = NULL;

    bd_mutex_lock(&p->ovl_mutex);

    if (p->overlay_root) {
        char *abs_path = str_printf("%s%s", p->disc_root, dir);
        dp = dir_open_default()(abs_path);
        X_FREE(abs_path);
    }

    bd_mutex_unlock(&p->ovl_mutex);

    return dp;
}

/*
 * directory combining
 */

typedef struct {
    unsigned int count;
    unsigned int pos;
    BD_DIRENT    entry[1]; /* VLA */
} COMB_DIR;

static void _comb_dir_close(BD_DIR_H *dp)
{
    X_FREE(dp->internal);
    X_FREE(dp);
}

static int _comb_dir_read(BD_DIR_H *dp, BD_DIRENT *entry)
{
    COMB_DIR *priv = (COMB_DIR *)dp->internal;
    if (priv->pos < priv->count) {
        strcpy(entry->d_name, priv->entry[priv->pos++].d_name);
        return 0;
    }
    return 1;
}

static void _comb_dir_append(BD_DIR_H *dp, BD_DIRENT *entry)
{
    COMB_DIR *priv = (COMB_DIR *)dp->internal;
    unsigned int i;

    if (!priv) {
        return;
    }

    /* no duplicates */
    for (i = 0; i < priv->count; i++) {
        if (!strcmp(priv->entry[i].d_name, entry->d_name)) {
            return;
        }
    }

    /* append */
    priv = realloc(priv, sizeof(*priv) + priv->count * sizeof(BD_DIRENT));
    if (!priv) {
        return;
    }
    strcpy(priv->entry[priv->count].d_name, entry->d_name);
    priv->count++;
    dp->internal = (void*)priv;
}

static BD_DIR_H *_combine_dirs(BD_DIR_H *ovl, BD_DIR_H *rom)
{
    BD_DIR_H *dp = calloc(1, sizeof(BD_DIR_H));
    BD_DIRENT entry;

    if (dp) {
        dp->read     = _comb_dir_read;
        dp->close    = _comb_dir_close;
        dp->internal = calloc(1, sizeof(COMB_DIR));

        while (!dir_read(ovl, &entry)) {
            _comb_dir_append(dp, &entry);
        }
        while (!dir_read(rom, &entry)) {
            _comb_dir_append(dp, &entry);
        }
    }
    dir_close(ovl);
    dir_close(rom);

    return dp;
}

/*
 * disc open / close
 */

BD_DISC *disc_open(const char *device_path,
                   struct bd_enc_info *enc_info,
                   const char *keyfile_path,
                   void *regs, void *psr_read, void *psr_write)
{
    BD_DISC *p = calloc(1, sizeof(BD_DISC));

    if (p) {

        char *disc_root = mount_get_mountpoint(device_path);

        p->disc_device = str_dup(device_path);

        /* make sure path ends to slash */
        if (disc_root[0] && disc_root[strlen(disc_root) - 1] == DIR_SEP_CHAR) {
            p->disc_root = disc_root;
        } else {
            p->disc_root = str_printf("%s%c", disc_root, DIR_SEP_CHAR);
            X_FREE(disc_root);
        }

        bd_mutex_init(&p->ovl_mutex);

        struct dec_dev dev = { p, _bdrom_open_path, (file_openFp)disc_open_path, p->disc_root, p->disc_device };
        p->dec = dec_init(&dev, enc_info, keyfile_path, regs, psr_read, psr_write);
    }

    return p;
}

void disc_close(BD_DISC **pp)
{
    if (pp && *pp) {
        BD_DISC *p = *pp;

        dec_close(&p->dec);

        bd_mutex_destroy(&p->ovl_mutex);

        X_FREE(p->disc_root);
        X_FREE(p->disc_device);
        X_FREE(*pp);
    }
}

/*
 *
 */

BD_PRIVATE const char *disc_root(BD_DISC *p)
{
    return p->disc_root;
}

/*
 * VFS
 */

BD_FILE_H *disc_open_path(BD_DISC *p, const char *rel_path)
{
    BD_FILE_H *fp;

    /* search file from overlay */
    fp = _overlay_open_path(p, rel_path);

    /* if not found, try BD-ROM */
    if (!fp) {
        fp = _bdrom_open_path((void *)p, rel_path);

        if (!fp) {
            BD_DEBUG(DBG_FILE | DBG_CRIT, "error opening file %s\n", rel_path);
        }
    }

    return fp;
}

BD_FILE_H *disc_open_file(BD_DISC *p, const char *dir, const char *file)
{
    BD_FILE_H *fp;
    char *path;

    path = str_printf("%s" DIR_SEP "%s", dir, file);
    fp = disc_open_path(p, path);
    X_FREE(path);

    return fp;
}

BD_DIR_H *disc_open_dir(BD_DISC *p, const char *dir)
{
    BD_DIR_H *dp_rom;
    BD_DIR_H *dp_ovl;

    dp_rom = _bdrom_open_dir(p, dir);
    dp_ovl = _overlay_open_dir(p, dir);

    if (!dp_ovl) {
        if (!dp_rom) {
            BD_DEBUG(DBG_FILE, "error opening dir %s\n", dir);
        }
        return dp_rom;
    }
    if (!dp_rom) {
        return dp_ovl;
    }

    return _combine_dirs(dp_ovl, dp_rom);
}

int64_t disc_read_file(BD_DISC *disc, const char *dir, const char *file,
                       uint8_t **data)
{
    BD_FILE_H *fp;
    int64_t    size;

    *data = NULL;

    fp = disc_open_file(disc, dir, file);
    if (!fp) {
        return -1;
    }

    size = file_size(fp);
    if (size > 0) {
        *data = malloc(size);
        if (*data) {
            int64_t got = file_read(fp, *data, size);
            if (got != size) {
                BD_DEBUG(DBG_FILE | DBG_CRIT, "Error reading file %s from %s\n", file, dir);
                X_FREE(*data);
                size = -1;
            }
        } else {
          size = -1;
        }
    }

    file_close(fp);
    return size;
}

/*
 * filesystem update
 */

void disc_update(BD_DISC *p, const char *overlay_root)
{
    bd_mutex_lock(&p->ovl_mutex);

    X_FREE(p->overlay_root);
    if (overlay_root) {
        p->overlay_root = str_dup(overlay_root);
    }

    bd_mutex_unlock(&p->ovl_mutex);
}

/*
 * streams
 */

BD_FILE_H *disc_open_stream(BD_DISC *disc, const char *file)
{
  BD_FILE_H *fp = disc_open_file(disc, "BDMV" DIR_SEP "STREAM", file);
  if (!fp) {
      return NULL;
  }

  if (disc->dec) {
      BD_FILE_H *st = dec_open_stream(disc->dec, fp, atoi(file));
      if (st) {
          return st;
      }
  }

  return fp;
}

const uint8_t *disc_get_data(BD_DISC *disc, int type)
{
    if (disc->dec) {
        return dec_data(disc->dec, type);
    }
    return NULL;
}

void disc_event(BD_DISC *disc, uint32_t event, uint32_t param)
{
    if (disc->dec) {
        switch (event) {
            case DISC_EVENT_START:
                dec_start(disc->dec, param);
                return;
            case DISC_EVENT_TITLE:
                dec_title(disc->dec, param);
                return;
            case DISC_EVENT_APPLICATION:
                dec_application(disc->dec, param);
                return;
        }
    }
}

