/*
 * This file is part of libbluray
 * Copyright (C) 2010-2017  Petri Hintukainen <phintuka@users.sourceforge.net>
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

#include "index_parse.h"
#include "extdata_parse.h"
#include "bdmv_parse.h"

#include "disc/disc.h"

#include "file/file.h"
#include "util/bits.h"
#include "util/logging.h"
#include "util/macro.h"
#include "util/strutl.h"

#include <stdlib.h>
#include <string.h>

static int _parse_hdmv_obj(BITSTREAM *bs, INDX_HDMV_OBJ *hdmv)
{
    hdmv->playback_type = bs_read(bs, 2);
    bs_skip(bs, 14);
    hdmv->id_ref = bs_read(bs, 16);
    bs_skip(bs, 32);

    if (hdmv->playback_type != indx_hdmv_playback_type_movie &&
        hdmv->playback_type != indx_hdmv_playback_type_interactive) {

        BD_DEBUG(DBG_NAV | DBG_CRIT, "index.bdmv: invalid HDMV playback type %d\n", hdmv->playback_type);
    }

    return 1;
}

static int _parse_bdj_obj(BITSTREAM *bs, INDX_BDJ_OBJ *bdj)
{
    bdj->playback_type = bs_read(bs, 2);
    bs_skip(bs, 14);
    bs_read_string(bs, bdj->name, 5);
    bs_skip(bs, 8);

    if (bdj->playback_type != indx_bdj_playback_type_movie &&
        bdj->playback_type != indx_bdj_playback_type_interactive) {

        BD_DEBUG(DBG_NAV | DBG_CRIT, "index.bdmv: invalid BD-J playback type %d\n", bdj->playback_type);
    }

    return 1;
}

static int _parse_playback_obj(BITSTREAM *bs, INDX_PLAY_ITEM *obj)
{
    obj->object_type = bs_read(bs, 2);
    bs_skip(bs, 30);

    switch (obj->object_type) {
        case indx_object_type_hdmv:
            return _parse_hdmv_obj(bs, &obj->hdmv);

        case indx_object_type_bdj:
            return _parse_bdj_obj(bs, &obj->bdj);
    }

    BD_DEBUG(DBG_NAV | DBG_CRIT, "index.bdmv: unknown object type %d\n", obj->object_type);
    return 0;
}

static int _parse_index(BITSTREAM *bs, INDX_ROOT *index)
{
    uint32_t index_len, i;

    index_len = bs_read(bs, 32);

    /* TODO: check if goes to extension data area */

    if ((bs_end(bs) - bs_pos(bs))/8 < (int64_t)index_len) {
        BD_DEBUG(DBG_NAV | DBG_CRIT, "index.bdmv: invalid index_len %d !\n", index_len);
        return 0;
    }

    if (!_parse_playback_obj(bs, &index->first_play) ||
        !_parse_playback_obj(bs, &index->top_menu)) {
        return 0;
    }

    index->num_titles = bs_read(bs, 16);
    if (!index->num_titles) {
        /* no "normal" titles - check for first play and top menu */
        if ((index->first_play.object_type == indx_object_type_hdmv && index->first_play.hdmv.id_ref == 0xffff) &&
            (index->top_menu.object_type == indx_object_type_hdmv && index->top_menu.hdmv.id_ref == 0xffff)) {

            BD_DEBUG(DBG_CRIT, "empty index\n");
            return 0;
        }
        return 1;
    }

    index->titles = calloc(index->num_titles, sizeof(INDX_TITLE));
    if (!index->titles) {
        BD_DEBUG(DBG_CRIT, "out of memory\n");
        return 0;
    }

    if (bs_avail(bs)/(12*8) < index->num_titles) {
        BD_DEBUG(DBG_HDMV|DBG_CRIT, "index.bdmv: unexpected EOF\n");
        return 0;
    }

    for (i = 0; i < index->num_titles; i++) {

        index->titles[i].object_type = bs_read(bs, 2);
        index->titles[i].access_type = bs_read(bs, 2);
        bs_skip(bs, 28);

        switch (index->titles[i].object_type) {
            case indx_object_type_hdmv:
                if (!_parse_hdmv_obj(bs, &index->titles[i].hdmv))
                    return 0;
                break;

            case indx_object_type_bdj:
                if (!_parse_bdj_obj(bs, &index->titles[i].bdj))
                    return 0;
                break;

            default:
                BD_DEBUG(DBG_NAV | DBG_CRIT, "index.bdmv: unknown object type %d (#%d)\n", index->titles[i].object_type, i);
                return 0;
        }
    }

    return 1;
}

static int _parse_app_info(BITSTREAM *bs, INDX_APP_INFO *app_info)
{
    uint32_t len;

    if (bs_seek_byte(bs, 40) < 0) {
        return 0;
    }

    len = bs_read(bs, 32);

    if (len != 34) {
        BD_DEBUG(DBG_NAV, "index.bdmv app_info length is %d, expected 34 !\n", len);
    }

    bs_skip(bs, 1);
    app_info->initial_output_mode_preference = bs_read(bs, 1);
    app_info->content_exist_flag             = bs_read(bs, 1);
    bs_skip(bs, 1);
    app_info->initial_dynamic_range_type     = bs_read(bs, 4);
    app_info->video_format                   = bs_read(bs, 4);
    app_info->frame_rate                     = bs_read(bs, 4);

    bs_read_bytes(bs, app_info->user_data, 32);

    return 1;
}

#define INDX_SIG1  ('I' << 24 | 'N' << 16 | 'D' << 8 | 'X')

static int _parse_header(BITSTREAM *bs,
                         uint32_t *index_start, uint32_t *extension_data_start,
                         uint32_t *indx_version)
{
    if (!bdmv_parse_header(bs, INDX_SIG1, indx_version)) {
        return 0;
    }

    *index_start          = bs_read(bs, 32);
    *extension_data_start = bs_read(bs, 32);

    return 1;
}

static int _parse_indx_extension_hevc(BITSTREAM *bs, INDX_ROOT *index)
{
  uint32_t len;
  unsigned unk0, unk1, unk2, unk3, unk4, unk5;

  len = bs_read(bs, 32);
  if (len < 8) {
      BD_DEBUG(DBG_NAV | DBG_CRIT, "index.bdmv: unsupported extension 3.1 length (%d)\n", len);
      return 0;
  }

  index->disc_type     = bs_read(bs, 4);
  unk0                 = bs_read(bs, 3);
  index->exist_4k_flag = bs_read(bs, 1);
  unk1                 = bs_read(bs, 8);
  unk2                 = bs_read(bs, 3);
  index->hdrplus_flag  = bs_read(bs, 1);
  unk3                 = bs_read(bs, 1);
  index->dv_flag       = bs_read(bs, 1);
  index->hdr_flags     = bs_read(bs, 2);
  unk4                 = bs_read(bs, 8);
  unk5                 = bs_read(bs, 32);

  BD_DEBUG(DBG_NAV, "UHD disc type: %d, 4k: %d, HDR: %d, HDR10+: %d, Dolby Vision: %d\n",
           index->disc_type, index->exist_4k_flag, index->hdr_flags, index->hdrplus_flag, index->dv_flag);

  if (unk0 | unk1 | unk2 | unk3 | unk4 | unk5) {
      BD_DEBUG(DBG_CRIT|DBG_NAV,
               "index.bdmv: unknown data in extension 3.1: "
               "0x%01x 0x%02x 0x%01x 0x%01x 0x%02x 0x%08x\n", unk0, unk1, unk2, unk3, unk4, unk5);
  }

  return 1;
}

static int
_parse_indx_extension(BITSTREAM *bits, int id1, int id2, void *handle)
{
    INDX_ROOT *index = (INDX_ROOT *)handle;
    (void)bits;

    if (id1 == 3) {
        if (id2 == 1) {
            return _parse_indx_extension_hevc(bits, index);
        }
    }

    BD_DEBUG(DBG_NAV | DBG_CRIT, "_parse_indx_extension(): unknown extension %d.%d\n", id1, id2);

    return 0;
}

static INDX_ROOT *_indx_parse(BD_FILE_H *fp)
{
    BITSTREAM  bs;
    INDX_ROOT *index;
    uint32_t   indexes_start, extension_data_start;

    if (bs_init(&bs, fp) < 0) {
        BD_DEBUG(DBG_NAV, "index.bdmv: read error\n");
        return NULL;
    }

    index = calloc(1, sizeof(INDX_ROOT));
    if (!index) {
        BD_DEBUG(DBG_CRIT, "out of memory\n");
        return NULL;
    }

    if (!_parse_header(&bs, &indexes_start, &extension_data_start, &index->indx_version) ||
        !_parse_app_info(&bs, &index->app_info)) {

        indx_free(&index);
        return NULL;
    }

    if (bs_seek_byte(&bs, indexes_start) < 0) {
        indx_free(&index);
        return NULL;
    }

    if (!_parse_index(&bs, index)) {
        indx_free(&index);
        return NULL;
    }

    if (extension_data_start) {
        bdmv_parse_extension_data(&bs,
                                  extension_data_start,
                                  _parse_indx_extension,
                                  index);
    }

    return index;
}

static INDX_ROOT *_indx_get(BD_DISC *disc, const char *path)
{
    BD_FILE_H *fp;
    INDX_ROOT *index;

    fp = disc_open_path(disc, path);
    if (!fp) {
        return NULL;
    }

    index = _indx_parse(fp);
    file_close(fp);
    return index;
}

INDX_ROOT *indx_get(BD_DISC *disc)
{
    INDX_ROOT *index;

    index = _indx_get(disc, "BDMV" DIR_SEP "index.bdmv");

    if (!index) {
        /* try backup */
        index = _indx_get(disc, "BDMV" DIR_SEP "BACKUP" DIR_SEP "index.bdmv");
    }

    return index;
}

void indx_free(INDX_ROOT **p)
{
    if (p && *p) {
        X_FREE((*p)->titles);
        X_FREE(*p);
    }
}
