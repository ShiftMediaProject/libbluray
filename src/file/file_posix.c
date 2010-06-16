/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  cRTrn13
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
 *
 * In addition, as a special exception, the copyright holders of libbluray
 * gives permission to link the code of its release of libbluray with the
 * OpenSSL project's "OpenSSL" library (or with modified versions of it
 * that use the same license as the "OpenSSL" library), and distribute
 * the linked library.  You must obey the GNU Lesser General Public
 * License in all respects for all of the code used other than "OpenSSL".
 * If you modify this file, you may extend this exception to your
 * version of the file, but you are not obligated to do so.  If you do
 * not wish to do so, delete this exception statement from your version.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "file.h"
#include "util/macro.h"
#include "util/logging.h"

#include <stdlib.h>

void file_close_linux(FILE_H *file);
int64_t file_seek_linux(FILE_H *file, int64_t offset, int32_t origin);
int64_t file_tell_linux(FILE_H *file);
int file_eof_linux(FILE_H *file);
int file_read_linux(FILE_H *file, uint8_t *buf, int64_t size);
int file_write_linux(FILE_H *file, uint8_t *buf, int64_t size);

void file_close_linux(FILE_H *file)
{
    if (file) {
        fclose((FILE *)file->internal);

        DEBUG(DBG_FILE, "Closed LINUX file (%p)\n", file);

        X_FREE(file);
    }
}

int64_t file_seek_linux(FILE_H *file, int64_t offset, int32_t origin)
{
    return fseeko((FILE *)file->internal, offset, origin);
}

int64_t file_tell_linux(FILE_H *file)
{
    return ftello((FILE *)file->internal);
}

int file_eof_linux(FILE_H *file)
{
    return feof((FILE *)file->internal);
}

int file_read_linux(FILE_H *file, uint8_t *buf, int64_t size)
{
    return fread(buf, 1, size, (FILE *)file->internal);
}

int file_write_linux(FILE_H *file, uint8_t *buf, int64_t size)
{
    return fwrite(buf, 1, size, (FILE *)file->internal);
}

FILE_H *file_open_linux(const char* filename, const char *mode)
{
    FILE *fp = NULL;
    FILE_H *file = malloc(sizeof(FILE_H));

    DEBUG(DBG_FILE, "Opening LINUX file %s... (%p)\n", filename, file);
    file->close = file_close_linux;
    file->seek = file_seek_linux;
    file->read = file_read_linux;
    file->write = file_write_linux;
    file->tell = file_tell_linux;
    file->eof = file_eof_linux;

    if ((fp = fopen(filename, mode))) {
        file->internal = fp;

        return file;
    }

    DEBUG(DBG_FILE, "Error opening file! (%p)\n", file);

    X_FREE(file);

    return NULL;
}



