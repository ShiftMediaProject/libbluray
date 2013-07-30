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

#ifndef _BD_LIBAACS_H_
#define _BD_LIBAACS_H_

#include <util/attributes.h>

#include <stdint.h>


typedef struct bd_aacs BD_AACS;

BD_PRIVATE int  libaacs_required(const char *device_path);
BD_PRIVATE BD_AACS *libaacs_load(void);
BD_PRIVATE int  libaacs_open(BD_AACS *p, const char *device_path, const char *keyfile_path);
BD_PRIVATE void libaacs_unload(BD_AACS **p);

BD_PRIVATE uint32_t libaacs_get_mkbv(BD_AACS *p);
BD_PRIVATE const uint8_t *libaacs_get_disc_id(BD_AACS *p);
BD_PRIVATE const uint8_t *libaacs_get_vid(BD_AACS *p);
BD_PRIVATE const uint8_t *libaacs_get_pmsn(BD_AACS *p);
BD_PRIVATE const uint8_t *libaacs_get_device_binding_id(BD_AACS *p);

BD_PRIVATE void libaacs_select_title(BD_AACS *p, uint32_t title);
BD_PRIVATE int  libaacs_decrypt_unit(BD_AACS *p, uint8_t *buf);


#endif /* _BD_LIBAACS_H_ */
