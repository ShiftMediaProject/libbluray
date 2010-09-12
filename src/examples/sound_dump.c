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

#include "libbluray/bdnav/sound_parse.h"

#include <stdio.h>
#include <stdlib.h>


static void _sound_print(SOUND_DATA *data)
{
    uint32_t i;

    printf("Number of sounds: %d\n", data->num_sounds);

    for (i = 0; i < data->num_sounds; i++) {
        printf("  Sound %d:\n", i);
        printf("      bits per sample: %d\n", data->sounds[i].bits_per_sample);
        printf("      sample rate: %d\n",     data->sounds[i].sample_rate);
        printf("      channels: %d\n",        data->sounds[i].num_channels);
        printf("      audio frames: %d",      data->sounds[i].num_frames);
        printf(" (%d ms)\n", data->sounds[i].num_frames * 1000 / data->sounds[i].sample_rate);
    }
}

static void _sound_dump(SOUND_DATA *data, int sound)
{
    size_t bytes;

    if (sound < 0 || sound >= data->num_sounds) {
        fprintf(stderr, "Invalid sound index %d\n", sound);
        return;
    }

    fprintf(stderr, "Sound %d: %d frames LPCM_LE, %dHz, %d bits, %s\n",
            sound,
            data->sounds[sound].num_frames,
            data->sounds[sound].sample_rate,
            data->sounds[sound].bits_per_sample,
            data->sounds[sound].num_channels == 1 ? "mono" : "stereo");

    bytes  = data->sounds[sound].bits_per_sample/8 * data->sounds[sound].num_channels;
    bytes *= data->sounds[sound].num_frames;

    if (fwrite(data->sounds[sound].samples, bytes, 1, stdout) != bytes) {
        fprintf(stderr, "I/O error\n");
    }

}

int main(int argc, const char *argv[])
{
    char       file[1024];
    SOUND_DATA *data;
    int         sound_index = -1;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "usage: sound_dump [sound_index] <disc_root>\n");
        fprintf(stderr, "If sound index is given, raw LPCM data is dumped to stdout\n");
        return 1;
    }

    if (argc == 3) {
      sound_index = atoi(argv[1]);
    }

    sprintf(file, "%s/BDMV/AUXDATA/sound.bdmv", argv[argc-1]);

    data = sound_parse(file);

    if (data) {

        if (sound_index >= 0) {
            _sound_dump(data, sound_index);

        } else {
            _sound_print(data);
        }

        sound_free(&data);
    }

    return 0;
}
