/*
 * Bluray stream playback
 * by Doom9 libbluray Team 2010
 *
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <unistd.h>
#include <dlfcn.h>

#include "libmpdemux/demuxer.h"
#include "libavutil/intreadwrite.h"

#include "m_struct.h"
#include "m_option.h"

#include "stream.h"

typedef int (*fptr_int)();
typedef uint64_t (*fptr_uint64)();
typedef void* (*fptr_p_void)();

static struct stream_priv_s {
    int title;
    char* device;
} stream_priv_dflts = {
    1,
    NULL
};

static struct bd_obj_s {
    void *handle;
    void *bd;
};

// BD://117/media/THE_MUMMY/
#define ST_OFF(f) M_ST_OFF(struct stream_priv_s,f)
static const m_option_t stream_opts_fields[] = {
    { "hostname", ST_OFF(title),  CONF_TYPE_INT, M_OPT_RANGE, 0, 99999, NULL},
    { "filename", ST_OFF(device), CONF_TYPE_STRING, 0, 0 ,0, NULL},
    { NULL, NULL, 0, 0, 0, 0,  NULL }
};

static const struct m_struct_st stream_opts = {
    "bd",
    sizeof(struct stream_priv_s),
    &stream_priv_dflts,
    stream_opts_fields
};

static void bd_stream_close(stream_t *s)
{
    struct bd_obj_s *bd_s = s->priv;

    fptr_p_void fptr = dlsym(bd_s->handle, "bd_close");
    fptr(bd_s->bd);

    dlclose(bd_s->handle);

    free(bd_s);
}


static int bd_stream_seek(stream_t *s, off_t pos)
{
    struct bd_obj_s *bd_s = s->priv;
    fptr_uint64 fptr_ui64 = dlsym(bd_s->handle, "bd_seek");

    if (pos > s->end_pos)
        return 0;

    s->pos = fptr_ui64(bd_s->bd, pos);

    return 1;
}

static int bd_stream_fill_buffer(stream_t *s, char *buf, int len)
{
    struct bd_obj_s *bd_s = s->priv;

    fptr_int fptr_i = dlsym(bd_s->handle, "bd_read");
    fptr_uint64 fptr_ui64 = dlsym(bd_s->handle, "bd_tell");

    if (s->pos > s->end_pos)
        return 0;

    int read_len;

    read_len = fptr_i(bd_s->bd, buf, len);

    s->pos = fptr_ui64(bd_s->bd);

    return read_len;
}

static int bd_stream_open(stream_t *s, int mode, void* opts, int* file_format)
{
    char device[200];

    struct stream_priv_s *p = opts;
    struct bd_obj_s *bd_s = calloc(1, sizeof(struct bd_obj_s));

    memset(device, 0, sizeof(device));

    if (p->device[0] != '/') {
        device[0] = '/';
        strcat(device, p->device);
    } else
        strncpy(device, p->device, 199);

    if (!(bd_s->handle = dlopen("libbluray.so", RTLD_LAZY))) return -1;  /* if no libbluray installed, fail */

    fptr_p_void fptr = dlsym(bd_s->handle, "bd_open");
    bd_s->bd = fptr(device, "keys.bin");

    fptr_int fptr_i = dlsym(bd_s->handle, "bd_select_title");
    fptr_i(bd_s->bd, p->title);

    s->sector_size = 6144;
    s->flags = STREAM_READ | MP_STREAM_SEEK;
    s->fill_buffer = bd_stream_fill_buffer;

    s->seek = bd_stream_seek;

    s->close = bd_stream_close;

    s->start_pos = 0;
    s->priv = bd_s;
    s->type = STREAMTYPE_BD;
    s->url = strdup("bd://");

    fptr_uint64 fptr_ui64 = dlsym(bd_s->handle, "bd_get_title_size");

    s->end_pos = fptr_ui64(bd_s->bd);

    return STREAM_OK;
}


const stream_info_t stream_info_bd = {
    "Bluray",
    "bd",
    "Obliter0n",
    "",
    bd_stream_open,
    { "bd", NULL },
    &stream_opts,
    1
};
