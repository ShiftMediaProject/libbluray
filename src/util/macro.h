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

#ifndef MACRO_H_
#define MACRO_H_

#include <stdio.h>

#define HEX_PRINT(X,Y) { int zz; for(zz = 0; zz < Y; zz++) fprintf(stderr, "%02X", X[zz]); fprintf(stderr, "\n"); }
#define MKINT_BE16(X) ( (X)[0] << 8 | (X)[1] )
#define MKINT_BE24(X) ( (X)[0] << 16 | (X)[1] << 8 | (X)[2] )
#define MKINT_BE32(X) ( (X)[0] << 24 | (X)[1] << 16 |  (X)[2] << 8 | (X)[3] )
#define X_FREE(X) do { if (X) free(X); X = NULL; } while(0)

#endif /* MACRO_H_ */
