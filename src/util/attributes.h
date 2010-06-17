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

#ifndef LIBBLURAY_ATTRIBUTES_H_
#define LIBBLURAY_ATTRIBUTES_H_

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3 )
#  define BD_ATTR_FORMAT_PRINTF(format,var) __attribute__((__format__(__printf__,format,var)))
#  define BD_ATTR_MALLOC                    __attribute__((__malloc__))
#  define BD_ATTR_PACKED                    __attribute__((packed))
#else
#  define BD_ATTR_FORMAT_PRINTF(format,var)
#  define BD_ATTR_MALLOC
#  define BD_ATTR_PACKED
#endif

#endif /* LIBBLURAY_ATTRIBUTES_H_ */
