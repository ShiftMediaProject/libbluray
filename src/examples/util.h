/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  John Stebbins
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders of libbluray
 * gives permission to link the code of its release of libbluray with the
 * OpenSSL project's "OpenSSL" library (or with modified versions of it
 * that use the same license as the "OpenSSL" library), and distribute
 * the linked executables.  You must obey the GNU General Public License
 * in all respects for all of the code used other than "OpenSSL".  If you
 * modify this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to do
 * so, delete this exception statement from your version.
 */

#include <stdint.h>

#if defined( __MINGW32__ )
#   undef  lseek
#   define lseek  _lseeki64
#   undef  fseeko
#   define fseeko fseeko64
#   undef  ftello
#   define ftello ftello64
#   define flockfile(...)
#   define funlockfile(...)
#   define getc_unlocked getc
#   undef  off_t
#   define off_t off64_t
#   undef  stat
#   define stat  _stati64
#   define fstat _fstati64
#   define wstat _wstati64
#endif

void hex_dump(uint8_t *buf, int count);
void indent_printf(int level, const char *fmt, ...);

