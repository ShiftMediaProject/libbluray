/*
 * This file is part of libbluray
 * Copyright (C) 2015  Petri Hintukainen
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

#include "udf_fs.h"

#include "file/file.h"
#include "util/macro.h"
#include "util/mutex.h"
#include "util/logging.h"

#include "udfread.h"
#include "blockinput.h"

#include <stdlib.h>
#include <stdio.h>   // SEEK_SET
#include <string.h>
#include <inttypes.h>

/*
 * file access
 */

static void _file_close(BD_FILE_H *file)
{
    if (file) {
        udfread_file_close(file->internal);
        BD_DEBUG(DBG_FILE, "Closed UDF file (%p)\n", (void*)file);
        X_FREE(file);
    }
}

static int64_t _file_seek(BD_FILE_H *file, int64_t offset, int32_t origin)
{
    return udfread_file_seek(file->internal, offset, origin);
}

static int64_t _file_tell(BD_FILE_H *file)
{
    return udfread_file_tell(file->internal);
}

static int64_t _file_read(BD_FILE_H *file, uint8_t *buf, int64_t size)
{
    return udfread_file_read(file->internal, buf, size);
}

BD_FILE_H *udf_file_open(void *udf, const char *filename)
{
    BD_FILE_H *file = calloc(1, sizeof(BD_FILE_H));

    BD_DEBUG(DBG_FILE, "Opening UDF file %s... (%p)\n", filename, (void*)file);

    file->close = _file_close;
    file->seek  = _file_seek;
    file->read  = _file_read;
    file->write = NULL;
    file->tell  = _file_tell;
    file->eof   = NULL;

    file->internal = udfread_file_open(udf, filename);
    if (!file->internal) {
        BD_DEBUG(DBG_FILE, "Error opening file %s!\n", filename);
        X_FREE(file);
    }

    return file;
}

/*
 * directory access
 */

static void _dir_close(BD_DIR_H *dir)
{
    if (dir) {
        udfread_closedir(dir->internal);
        BD_DEBUG(DBG_DIR, "Closed UDF dir (%p)\n", (void*)dir);
        X_FREE(dir);
    }
}

static int _dir_read(BD_DIR_H *dir, BD_DIRENT *entry)
{
    struct udfread_dirent e;

    if (!udfread_readdir(dir->internal, &e)) {
        return -1;
    }

    strncpy(entry->d_name, e.d_name, sizeof(entry->d_name));
    entry->d_name[sizeof(entry->d_name) - 1] = 0;

    return 0;
}

BD_DIR_H *udf_dir_open(void *udf, const char* dirname)
{
    BD_DIR_H *dir = calloc(1, sizeof(BD_DIR_H));

    BD_DEBUG(DBG_DIR, "Opening UDF dir %s... (%p)\n", dirname, (void*)dir);

    dir->close = _dir_close;
    dir->read  = _dir_read;

    dir->internal = udfread_opendir(udf, dirname);
    if (!dir->internal) {
        BD_DEBUG(DBG_DIR, "Error opening %s\n", dirname);
        X_FREE(dir);
    }

    return dir;
}

/*
 * UDF image access
 */

typedef struct {
    struct udfread_block_input i;
    BD_FILE_H *fp;
    BD_MUTEX mutex;
} UDF_BI;

static int _bi_close(struct udfread_block_input *bi_gen)
{
    UDF_BI *bi = (UDF_BI *)bi_gen;
    file_close(bi->fp);
    bd_mutex_destroy(&bi->mutex);
    X_FREE(bi);
    return 0;
}

static uint32_t _bi_size(struct udfread_block_input *bi_gen)
{
    UDF_BI *bi = (UDF_BI *)bi_gen;
    return file_size(bi->fp) / UDF_BLOCK_SIZE;
}

static int _bi_read(struct udfread_block_input *bi_gen, uint32_t lba, void *buf, uint32_t nblocks, int flags)
{
    (void)flags;
    UDF_BI *bi = (UDF_BI *)bi_gen;
    int got = -1;

    /* seek + read must be atomic */
    bd_mutex_lock(&bi->mutex);

    if (file_seek(bi->fp, SEEK_SET, (int64_t)lba * UDF_BLOCK_SIZE) >= 0) {
        int64_t bytes = file_read(bi->fp, buf, (int64_t)nblocks * UDF_BLOCK_SIZE);
        if (bytes > 0) {
            got = bytes / UDF_BLOCK_SIZE;
        }
    }

    bd_mutex_unlock(&bi->mutex);

    return got;
}

static struct udfread_block_input *_block_input(const char *img)
{
    BD_FILE_H *fp = file_open(img, "rb");
    if (fp) {
        UDF_BI *bi = calloc(1, sizeof(*bi));
        if (bi) {
            bi->fp      = fp;
            bi->i.close = _bi_close;
            bi->i.read  = _bi_read;
            bi->i.size  = _bi_size;
            bd_mutex_init(&bi->mutex);
            return &bi->i;
        }
        file_close(fp);
    }
    return NULL;
}


void *udf_image_open(const char *img_path)
{
    udfread *udf = NULL;

    /* app handles file I/O ? */
    if (file_open != file_open_default()) {
        struct udfread_block_input *bi = _block_input(img_path);
        if (bi) {
            udf = udfread_open_input(bi);
            if (!udf) {
                bi->close(bi);
            }
        }
    }

    if (!udf) {
        udf = (void*)udfread_open(img_path);
    }

    return (void*)udf;
}

void udf_image_close(void *udf)
{
    udfread_close(udf);
}
