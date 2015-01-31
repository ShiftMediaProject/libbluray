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
 */

#include <stdio.h>
#include <stdlib.h>

#include "libbluray/bluray.h"


static void _sound_print(int sound_index, BLURAY_SOUND_EFFECT *data)
{
    printf("  Sound %d:\n", sound_index);
    printf("      bits per sample: %d\n", 16);
    printf("      sample rate: %d\n",     48000);
    printf("      channels: %d\n",        data->num_channels);
    printf("      audio frames: %d",      data->num_frames);
    printf(" (%d ms)\n", data->num_frames * 1000 / 48000);
}

static void _sound_dump(int sound_index, BLURAY_SOUND_EFFECT *data)
{
    size_t bytes;

    fprintf(stderr, "Sound %d: %d frames LPCM_LE, %dHz, %d bits, %s\n",
            sound_index,
            data->num_frames,
            48000,
            16,
            data->num_channels == 1 ? "mono" : "stereo");

    bytes = 2 * data->num_channels * data->num_frames;

    if (fwrite(data->samples, bytes, 1, stdout) != 1) {
        fprintf(stderr, "I/O error\n");
    }
}

int main(int argc, const char *argv[])
{
    BLURAY_SOUND_EFFECT effect;
    BLURAY *bd;
    int     sound_index = -1;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "usage: sound_dump [sound_index] <disc_root>\n");
        fprintf(stderr, "If sound index is given, raw LPCM data is dumped to stdout\n");
        return 1;
    }

    if (argc == 3) {
      sound_index = atoi(argv[1]);
    }

    /* open disc */
    bd = bd_open(argv[argc-1], NULL);
    if (!bd) {
        fprintf(stderr, "error opening disc %s\n", argv[argc-1]);
        return -1;
    }

    if (sound_index >= 0) {
        if (bd_get_sound_effect(bd, sound_index, &effect) <= 0) {
            fprintf(stderr, "Invalid sound index %d\n", sound_index);
        } else {
            _sound_dump(sound_index, &effect);
        }

    } else {

        while (bd_get_sound_effect(bd, ++sound_index, &effect) > 0) {
            _sound_print(sound_index, &effect);
        }
        if (sound_index == 0) {
            fprintf(stderr, "No sound effects\n");
        }
    }

    bd_close(bd);

    return 0;
}
