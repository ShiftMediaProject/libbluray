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
#include "util/strutl.h"
#include "file/file.h"
#include "file/mount.h"

#include <string.h>

struct bd_disc {
    char     *disc_root;
    char     *disc_device;

    BD_DEC   *dec;
};

static int _disc_have_file(BD_DISC *p, const char *dir, const char *file)
{
    BD_FILE_H *fp;
    char *full_path;

    full_path = str_printf("%s%s" DIR_SEP "%s", p->disc_root, dir, file );
    fp = file_open(full_path, "r");
    X_FREE(full_path);

    if (fp) {
        file_close(fp);
        return 1;
    }

    return 0;
}

/*
 *
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

        struct dec_dev dev = { p, (void *)_disc_have_file, (void*)disc_open_path, p->disc_root, p->disc_device };
        p->dec = dec_init(&dev, enc_info, keyfile_path, regs, psr_read, psr_write);
    }

    return p;
}

void disc_close(BD_DISC **pp)
{
    if (pp && *pp) {
        BD_DISC *p = *pp;

        dec_close(&p->dec);

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

BD_PRIVATE const char *disc_device(BD_DISC *p)
{
    return p->disc_device;
}

BD_FILE_H *disc_open_path(BD_DISC *p, const char *rel_path)
{
    BD_FILE_H *fp;
    char *path;

    path = str_printf("%s%s", p->disc_root, rel_path);
    fp = file_open(path, "r");

    if (!fp) {
        BD_DEBUG(DBG_FILE | DBG_CRIT, "error opening file %s\n", path);
    }

    X_FREE(path);

    return fp;
}

BD_FILE_H *disc_open_file(BD_DISC *p, const char *dir, const char *file)
{
    BD_FILE_H *fp;
    char *path;

    path = str_printf("%s%s%c%s", p->disc_root, dir, DIR_SEP_CHAR, file);
    fp = file_open(path, "r");

    if (!fp) {
        BD_DEBUG(DBG_FILE | DBG_CRIT, "error opening file %s\n", path);
    }

    X_FREE(path);

    return fp;
}

BD_DIR_H *disc_open_dir(BD_DISC *p, const char *dir)
{
    BD_DIR_H *dp;
    char *path;

    path = str_printf("%s%s", p->disc_root, dir);
    dp = dir_open(path);

    if (!dp) {
        BD_DEBUG(DBG_FILE, "error opening dir %s\n", path);
    }

    X_FREE(path);

    return dp;
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
 *
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

