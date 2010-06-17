/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  Obliter0n
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

#ifndef FILE_H_
#define FILE_H_

#include <stdint.h>

//#ifdef __LINUX__
#define file_open file_open_linux
#define DIR_SEP "/"
//#endif

#define file_close(X) X->close(X)
#define file_seek(X,Y,Z) X->seek(X,Y,Z)
#define file_tell(X) X->tell(X)
#define file_eof(X) X->eof(X)
#define file_read(X,Y,Z) X->read(X,Y,Z)
#define file_write(X,Y,Z) X->write(X,Y,Z)

typedef struct file FILE_H;
struct file
{
    void* internal;
    void (*close)(FILE_H *file);
    int64_t (*seek)(FILE_H *file, int64_t offset, int32_t origin);
    int64_t (*tell)(FILE_H *file);
    int (*eof)(FILE_H *file);
    int (*read)(FILE_H *file, uint8_t *buf, int64_t size);
    int (*write)(FILE_H *file, uint8_t *buf, int64_t size);
};

FILE_H *file_open_linux(const char* filename, const char *mode);

#endif /* FILE_H_ */
