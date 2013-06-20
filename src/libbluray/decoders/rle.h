/*
 * This file is part of libbluray
 * Copyright (C) 2013  Petri Hintukainen <phintuka@users.sourceforge.net>
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

#if !defined(_BD_RLE_H_)
#define _BD_RLE_H_

#include "overlay.h"

#include <util/attributes.h>

#include <stdint.h>

/*
 *
 */

typedef struct {
    BD_PG_RLE_ELEM *start;    /* first element */
    BD_PG_RLE_ELEM *elem;     /* current element */
    int             num_elem; /* allocated element count */
} RLE_ENC;

/*
 *
 */

#include "util/refcnt.h"
#include "util/macro.h"

static void rle_begin(RLE_ENC *p)
{
    p->num_elem = 1024;
    p->start = refcnt_realloc(NULL, p->num_elem * sizeof(BD_PG_RLE_ELEM));

    p->elem   = p->start;
    p->elem->len = 0;
    p->elem->color = 0xffff;
}

static void rle_end(RLE_ENC *p)
{
    bd_refcnt_dec(p->start);
    p->start = NULL;
}

static void _rle_grow(RLE_ENC *p)
{
    int count = (int)(p->elem - p->start) + 1;
    if (count >= p->num_elem) {
        /* realloc */
        p->num_elem = p->num_elem * 2;
        p->start = refcnt_realloc(p->start, p->num_elem * sizeof(BD_PG_RLE_ELEM));
    }

    p->elem = p->start + count;
    p->elem->len = 0;
}

static void rle_add_eol(RLE_ENC *p)
{
    if (p->elem->len) {
        _rle_grow(p);
    }
    p->elem->color = 0;

    _rle_grow(p);
    p->elem->color = 0xffff;
}

static void rle_add_bite(RLE_ENC *p, uint8_t color, int len)
{
    if (color == p->elem->color) {
        p->elem->len += len;
    } else {
        if (p->elem->len) {
            _rle_grow(p);
        }
        p->elem->color = color;
        p->elem->len = len;
    }
}

static void rle_compress_chunk(RLE_ENC *p, const uint8_t *mem, unsigned width)
{
    unsigned ii;
    for (ii = 0; ii < width; ii++) {
        rle_add_bite(p, mem[ii], 1);
    }
}

#endif /* _BD_RLE_H_ */
