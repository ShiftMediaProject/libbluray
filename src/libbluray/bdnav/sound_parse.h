/*
 * This file is part of libbluray
 * Copyright (C) 2010  hpi1
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

#if !defined(_SOUND_PARSE_H_)
#define _SOUND_PARSE_H_

#include <stdint.h>

typedef struct {
  uint8_t    num_channels;
  uint32_t   sample_rate;
  uint8_t    bits_per_sample;

  uint32_t   num_frames;
  uint16_t  *samples;       /* LPCM, interleaved */
} SOUND_OBJECT;

typedef struct {
    uint16_t     num_sounds;
    SOUND_OBJECT sounds[];
} SOUND_DATA;


SOUND_DATA * sound_parse(const char *path); /* parse sound.bdmv */
void sound_free(SOUND_DATA *sound);

#endif // _MOBJ_PARSE_H_
