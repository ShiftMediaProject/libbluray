/*
 * This file is part of libbluray
 * Copyright (C) 2010  hpi1
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
 * the linked executables.  You must obey the GNU General Public License
 * in all respects for all of the code used other than "OpenSSL".  If you
 * modify this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to do
 * so, delete this exception statement from your version.
 */

#include "file/file.h"
#include "util/bits.h"
#include "util/logging.h"
#include "util/macro.h"
#include "sound_parse.h"

#include <stdlib.h>

#define BCLK_SIG1  ('B' << 24 | 'C' << 16 | 'L' << 8 | 'K')
#define BCLK_SIG2A ('0' << 24 | '2' << 16 | '0' << 8 | '0')
#define BCLK_SIG2B ('0' << 24 | '1' << 16 | '0' << 8 | '0')


static int _bclk_parse_header(BITSTREAM *bs, uint32_t *data_start, uint32_t *extension_data_start)
{
    uint32_t sig1, sig2;

    bs_seek_byte(bs, 0);

    sig1 = bs_read(bs, 32);
    sig2 = bs_read(bs, 32);

    if (sig1 != BCLK_SIG1 ||
       (sig2 != BCLK_SIG2A &&
        sig2 != BCLK_SIG2B)) {
     DEBUG(DBG_NAV, "sound.bdmv failed signature match: expected BCLK0100 got %8.8s\n", bs->buf);
     return 0;
    }

    *data_start           = bs_read(bs, 32);
    *extension_data_start = bs_read(bs, 32);

    return 1;
}

static int _sound_parse_attributes(BITSTREAM *bs, SOUND_OBJECT *obj)
{
    int i;

    switch (i = bs_read(bs, 4)) {
        default: DEBUG(DBG_NAV, "unknown channel configuration code %d\n", i);
        case 1:  obj->num_channels = 1;
                 break;
        case 3:  obj->num_channels = 2;
                 break;
    };
    switch (i = bs_read(bs, 4)) {
        default: DEBUG(DBG_NAV, "unknown sample rate code %d\n", i);
        case 1:  obj->sample_rate = 48000;
                 break;
    };
    switch (i = bs_read(bs, 2)) {
        default: DEBUG(DBG_NAV, "unknown bits per sample code %d\n", i);
        case 1:  obj->bits_per_sample = 16;
                 break;
    };

    bs_skip(bs, 6); /* padding */

    return 1;
}

static int _sound_read_samples(BITSTREAM *bs, uint32_t offset, SOUND_OBJECT *obj)
{
  uint32_t n;

  bs_seek_byte(bs, offset);

  obj->samples = malloc(obj->num_frames);

  for (n = 0; n < obj->num_frames; n += 2) {
    obj->samples[n] = bs_read(bs, 16);
  }

  obj->num_frames /= (obj->bits_per_sample / 8) * obj->num_channels;

  return 1;
}

void sound_free(SOUND_DATA *sounds)
{
    if (sounds) {

        int i;
        for (i = 0 ; i < sounds->num_sounds; i++) {
            X_FREE(sounds->sounds[i].samples);
        }

        X_FREE(sounds);
    }
}

SOUND_DATA *sound_parse(const char *file_name)
{
    BITSTREAM     bs;
    FILE_H       *fp;
    SOUND_DATA   *data;
    uint16_t      num_sounds;
    uint32_t      data_len;
    int           i;
    uint32_t      data_start, extension_data_start;
    uint32_t     *data_offsets;

    fp = file_open(file_name, "rb");
    if (!fp) {
      DEBUG(DBG_NAV | DBG_CRIT, "error opening %s\n", file_name);
      return NULL;
    }

    bs_init(&bs, fp);

    if (!_bclk_parse_header(&bs, &data_start, &extension_data_start)) {
        DEBUG(DBG_NAV | DBG_CRIT, "%s: invalid header\n", file_name);
        file_close(fp);
        return NULL;
    }

    bs_seek_byte(&bs, 40);

    data_len = bs_read(&bs, 32);
    bs_skip(&bs, 8); /* reserved */
    num_sounds = bs_read(&bs, 8);

    if (data_len < 1) {
        DEBUG(DBG_NAV | DBG_CRIT, "%s: empty database\n", file_name);
        file_close(fp);
        return NULL;
    }

    data_offsets = calloc(num_sounds, sizeof(uint32_t));
    data = calloc(1, sizeof(SOUND_DATA) + num_sounds * sizeof(SOUND_OBJECT));
    data->num_sounds = num_sounds;

    for (i = 0; i < data->num_sounds; i++) {
        if (!_sound_parse_attributes(&bs, &data->sounds[i])) {
            DEBUG(DBG_NAV | DBG_CRIT, "%s: error parsing sound %d attribues\n", file_name, i);
            sound_free(data);
            X_FREE(data_offsets);
            file_close(fp);
            return NULL;
        }
        data->sounds[i].num_frames = bs_read(&bs, 32);
        data_offsets[i]            = bs_read(&bs, 32);
    }

    for (i = 0; i < data->num_sounds; i++) {
      if (!_sound_read_samples(&bs, data_offsets[i], &data->sounds[i])) {
            DEBUG(DBG_NAV | DBG_CRIT, "%s: error reading samples for sound %d\n", file_name, i);
            sound_free(data);
            X_FREE(data_offsets);
            file_close(fp);
            return NULL;
        }
    }

    X_FREE(data_offsets);
    file_close(fp);

    return data;
}
