/*
 * Copyright (C) 2000-2005 the xine project
 *
 * Copyright (C) 2009 Petri Hintukainen <phintuka@users.sourceforge.net>
 *
 * This file is part of xine, a free video player.
 *
 * xine is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * Input plugin for BluRay discs / images
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* asprintf: */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#include <libbluray/bluray.h>

#define LOG_MODULE "input_bluray"
#define LOG_VERBOSE

#define LOG

#define LOGMSG(x...)  xine_log (this->stream->xine, XINE_LOG_MSG, "input_bluray: " x);


#ifdef HAVE_CONFIG_H
# include "xine_internal.h"
# include "input_plugin.h"
#else
# include <xine/xine_internal.h>
# include <xine/input_plugin.h>
#endif

#ifndef EXPORTED
#  define EXPORTED __attribute__((visibility("default")))
#endif

#ifndef MIN
# define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
# define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define ALIGNED_UNIT_SIZE 6144
#define PKT_SIZE          192
#define TICKS_IN_MS       45

typedef struct {

  input_class_t   input_class;

  xine_t         *xine;

  /* config */
  char           *mountpoint;

} bluray_input_class_t;

typedef struct {
  input_plugin_t        input_plugin;

  xine_stream_t        *stream;
  bluray_input_class_t *class;
  char                 *mrl;
  char                 *disc_root;

  BLURAY               *bdh;

  int                num_titles;
  int                current_title;
  BLURAY_TITLE_INFO *title_info;
  int                current_clip;

} bluray_input_plugin_t;

static void update_stream_info(bluray_input_plugin_t *this)
{
  /* set stream info */

  _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_ANGLE_COUNT,    this->title_info->angle_count);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_ANGLE_NUMBER,   bd_get_current_angle(this->bdh));
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_HAS_CHAPTERS,       this->title_info->chapter_count > 0);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_CHAPTER_COUNT,  this->title_info->chapter_count);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_CHAPTER_NUMBER, bd_get_current_chapter(this->bdh) + 1);
}

static int open_title (bluray_input_plugin_t *this, int title)
{
  if (bd_select_title(this->bdh, title) <= 0) {
    LOGMSG("bd_select_title(%d) failed\n", title);
    return 0;
  }

  this->current_title = title;

  if (this->title_info)
    bd_free_title_info(this->title_info);
  this->title_info = bd_get_title_info(this->bdh, this->current_title);

  /* calculate and set stream rate */

  uint64_t rate = bd_get_title_size(this->bdh) * UINT64_C(8) // bits
                  * INT64_C(90000)
                  / (uint64_t)(this->title_info->duration);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_BITRATE, rate);

  /* set stream info */

  _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_TITLE_COUNT,  this->num_titles);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_TITLE_NUMBER, this->current_title);

  update_stream_info(this);

  return 1;
}

/*
 * xine plugin interface
 */

static uint32_t bluray_plugin_get_capabilities (input_plugin_t *this_gen)
{
  return INPUT_CAP_SEEKABLE  |
         INPUT_CAP_BLOCK     |
         INPUT_CAP_AUDIOLANG |
         INPUT_CAP_SPULANG;
}

static off_t bluray_plugin_read (input_plugin_t *this_gen, char *buf, off_t len)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  if (!this || !this->bdh || len < 0)
    return -1;

  off_t result = bd_read (this->bdh, (unsigned char *)buf, len);

  if (result < 0)
    LOGMSG("bd_read() failed: %s (%d of %d)\n", strerror(errno), (int)result, (int)len);

  return result;
}

static buf_element_t *bluray_plugin_read_block (input_plugin_t *this_gen, fifo_buffer_t *fifo, off_t todo)
{
  buf_element_t *buf = fifo->buffer_pool_alloc (fifo);

  if (todo > (off_t)buf->max_size)
    todo = buf->max_size;

  if (todo > ALIGNED_UNIT_SIZE)
    todo = ALIGNED_UNIT_SIZE;

  if (todo > 0) {
    bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

    buf->size = bluray_plugin_read(this_gen, (char*)buf->mem, todo);
    buf->type = BUF_DEMUX_BLOCK;

    if (buf->size > 0) {
      buf->extra_info->input_time = 0;
      buf->extra_info->total_time = this->title_info->duration / 90000;
      return buf;
    }
  }

  buf->free_buffer (buf);
  return NULL;
}

static off_t bluray_plugin_seek (input_plugin_t *this_gen, off_t offset, int origin)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  if (!this || !this->bdh)
    return -1;

  /* convert relative seeks to absolute */

  if (origin == SEEK_CUR) {
    offset = bd_tell(this->bdh) + offset;
  }
  else if (origin == SEEK_END) {
    if (offset < (off_t)bd_get_title_size(this->bdh))
      offset = bd_get_title_size(this->bdh) - offset;
    else
      offset = 0;
  }

  lprintf("bluray_plugin_seek() seeking to %lld\n", (long long)offset);

  return bd_seek (this->bdh, offset);
}

static off_t bluray_plugin_seek_time (input_plugin_t *this_gen, int time_offset, int origin)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  if (!this || !this->bdh || !this->title_info)
    return -1;

  /* convert relative seeks to absolute */

  if (origin == SEEK_CUR) {
    time_offset += this_gen->get_current_time(this_gen);
  }
  else if (origin == SEEK_END) {
    int duration = this->title_info->duration / 90;
    if (time_offset < duration)
      time_offset = duration - time_offset;
    else
      time_offset = 0;
  }

  lprintf("bluray_plugin_seek_time() seeking to %d.%03ds\n", time_offset / 1000, time_offset % 1000);

  return bd_seek_time(this->bdh, time_offset * INT64_C(90));
}

static off_t bluray_plugin_get_current_pos (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  return this->bdh ? bd_tell(this->bdh) : 0;
}

static int bluray_plugin_get_current_time (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  return this->bdh ? (int)(bd_tell_time(this->bdh) / UINT64_C(90)) : -1;
}

static off_t bluray_plugin_get_length (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  return this->bdh ? (off_t)bd_get_title_size(this->bdh) : (off_t)-1;
}

static uint32_t bluray_plugin_get_blocksize (input_plugin_t *this_gen)
{
  return ALIGNED_UNIT_SIZE;
}

static const char* bluray_plugin_get_mrl (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  return this->mrl;
}

static int bluray_plugin_get_optional_data (input_plugin_t *this_gen, void *data, int data_type)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  if (!this || !this->stream || !data)
    return INPUT_OPTIONAL_UNSUPPORTED;

  switch (data_type) {
    case INPUT_OPTIONAL_DATA_DEMUXER:
      *(const char **)data = "mpeg-ts";
      return INPUT_OPTIONAL_SUCCESS;

    /*
     * audio track language:
     * - channel number can be mpeg-ts PID (0x1100 ... 0x11ff)
     */
    case INPUT_OPTIONAL_DATA_AUDIOLANG:
      if (this->title_info) {
        int               channel = *((int *)data);
        BLURAY_CLIP_INFO *clip    = &this->title_info->clips[this->current_clip];

        if (channel < clip->audio_stream_count) {
          memcpy(data, clip->audio_streams[channel].lang, 4);
          lprintf("INPUT_OPTIONAL_DATA_AUDIOLANG: %02d [pid 0x%04x]: %s\n",
                  channel, clip->audio_streams[channel].pid, clip->audio_streams[channel].lang);

          return INPUT_OPTIONAL_SUCCESS;
        }
        /* search by pid */
        int i;
        for (i = 0; i < clip->audio_stream_count; i++) {
          if (channel == clip->audio_streams[i].pid) {
            memcpy(data, clip->audio_streams[i].lang, 4);
            lprintf("INPUT_OPTIONAL_DATA_AUDIOLANG: pid 0x%04x -> ch %d: %s\n",
                    channel, i, clip->audio_streams[i].lang);

            return INPUT_OPTIONAL_SUCCESS;
          }
        }
      }
      return INPUT_OPTIONAL_UNSUPPORTED;

    /*
     * SPU track language:
     * - channel number can be mpeg-ts PID (0x1200 ... 0x12ff)
     */
    case INPUT_OPTIONAL_DATA_SPULANG:
      if (this->title_info) {
        int               channel = *((int *)data);
        BLURAY_CLIP_INFO *clip    = &this->title_info->clips[this->current_clip];

        if (channel < clip->pg_stream_count) {
          memcpy(data, clip->pg_streams[channel].lang, 4);
          lprintf("INPUT_OPTIONAL_DATA_SPULANG: %02d [pid 0x%04x]: %s\n",
                  channel, clip->pg_streams[channel].pid, clip->pg_streams[channel].lang);

          return INPUT_OPTIONAL_SUCCESS;
        }
        /* search by pid */
        int i;
        for (i = 0; i < clip->pg_stream_count; i++) {
          if (channel == clip->pg_streams[i].pid) {
            memcpy(data, clip->pg_streams[i].lang, 4);
            lprintf("INPUT_OPTIONAL_DATA_SPULANG: pid 0x%04x -> ch %d: %s\n",
                    channel, i, clip->pg_streams[i].lang);

            return INPUT_OPTIONAL_SUCCESS;
          }
        }
      }
      return INPUT_OPTIONAL_UNSUPPORTED;

    default:
      return DEMUX_OPTIONAL_UNSUPPORTED;
    }

  return INPUT_OPTIONAL_UNSUPPORTED;
}

static void bluray_plugin_dispose (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  if (this->title_info)
    bd_free_title_info(this->title_info);

  if (this->bdh)
    bd_close(this->bdh);

  free (this->mrl);
  free (this->disc_root);

  free (this);
}

static int bluray_plugin_open (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this  = (bluray_input_plugin_t *) this_gen;
  int                    title = -1, chapter = 0;

  lprintf("bluray_plugin_open\n");

  /* validate mrl */

  if (strncasecmp (this->mrl, "bluray:", 7))
    return -1;

  if (!strcasecmp (this->mrl, "bluray:") ||
      !strcasecmp (this->mrl, "bluray:/") ||
      !strcasecmp (this->mrl, "bluray://") ||
      !strcasecmp (this->mrl, "bluray:///")) {

    this->disc_root = strdup(this->class->mountpoint);

  } else if (!strncasecmp (this->mrl, "bluray:/", 8)) {

    if (!strncasecmp (this->mrl, "bluray:///", 10))
      this->disc_root = strdup(this->mrl + 9);
    else if (!strncasecmp (this->mrl, "bluray://", 9))
      this->disc_root = strdup(this->mrl + 8);
    else
      this->disc_root = strdup(this->mrl + 7);

    _x_mrl_unescape(this->disc_root);

    if (this->disc_root[strlen(this->disc_root)-1] != '/') {
      char *end = strrchr(this->disc_root, '/');
      if (end && end[1])
        if (sscanf(end, "/%d.%d", &title, &chapter) < 1)
          title = -1;
      *end = 0;
    }

  } else {
    return -1;
  }

  /* open libbluray */
  if (! (this->bdh = bd_open (this->disc_root, NULL))) {
    LOGMSG("bd_open(\'%s\') failed: %s\n", this->disc_root, strerror(errno));
    return -1;
  }
  lprintf("bd_open(\'%s\') OK\n", this->disc_root);

  /* load title list */

  this->num_titles = bd_get_titles(this->bdh, TITLES_RELEVANT);
  LOGMSG("%d titles\n", this->num_titles);

  if (this->num_titles < 1)
    return -1;

  /* select title */

  /* if title was not in mrl, find the main title */
  if (title < 0) {
    uint64_t duration = 0;
    int i, playlist = 99999;
    for (i = 0; i < this->num_titles; i++) {
      BLURAY_TITLE_INFO *info = bd_get_title_info(this->bdh, i);
      if (info->duration > duration) {
        title    = i;
        duration = info->duration;
        playlist = info->playlist;
      }
      bd_free_title_info(info);
    }
    lprintf("main title: %d (%05d.mpls)\n", title, playlist);
  }

  if (open_title(this, title) <= 0 &&
      open_title(this, 0) <= 0)
    return -1;

  /* jump to chapter */

  if (chapter > 0) {
    chapter = MAX(0, MIN((int)this->title_info->chapter_count, chapter) - 1);
    bd_seek_chapter(this->bdh, chapter);
    _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_CHAPTER_NUMBER, chapter + 1);
  }

  return 1;
}

static input_plugin_t *bluray_class_get_instance (input_class_t *cls_gen, xine_stream_t *stream,
                                                  const char *mrl)
{
  bluray_input_plugin_t *this;

  lprintf("bluray_class_get_instance\n");

  if (strncasecmp (mrl, "bluray:", 7))
    return NULL;

  this = (bluray_input_plugin_t *) calloc(1, sizeof (bluray_input_plugin_t));

  this->stream = stream;
  this->class  = (bluray_input_class_t*)cls_gen;
  this->mrl    = strdup(mrl);

  this->input_plugin.open               = bluray_plugin_open;
  this->input_plugin.get_capabilities   = bluray_plugin_get_capabilities;
  this->input_plugin.read               = bluray_plugin_read;
  this->input_plugin.read_block         = bluray_plugin_read_block;
  this->input_plugin.seek               = bluray_plugin_seek;
  this->input_plugin.seek_time          = bluray_plugin_seek_time;
  this->input_plugin.get_current_pos    = bluray_plugin_get_current_pos;
  this->input_plugin.get_current_time   = bluray_plugin_get_current_time;
  this->input_plugin.get_length         = bluray_plugin_get_length;
  this->input_plugin.get_blocksize      = bluray_plugin_get_blocksize;
  this->input_plugin.get_mrl            = bluray_plugin_get_mrl;
  this->input_plugin.get_optional_data  = bluray_plugin_get_optional_data;
  this->input_plugin.dispose            = bluray_plugin_dispose;
  this->input_plugin.input_class        = cls_gen;

  return &this->input_plugin;
}

/*
 * plugin class
 */

static void mountpoint_change_cb(void *data, xine_cfg_entry_t *cfg)
{
  bluray_input_class_t *this = (bluray_input_class_t *) data;

  this->mountpoint = cfg->str_value;
}

static const char *bluray_class_get_description (input_class_t *this_gen)
{
  return _("BluRay input plugin");
}

static const char *bluray_class_get_identifier (input_class_t *this_gen)
{
  return "bluray";
}

static char **bluray_class_get_autoplay_list (input_class_t *this_gen, int *num_files)
{
  static char *autoplay_list[] = { "bluray:/", NULL };

  *num_files = 1;

  return autoplay_list;
}

static int bluray_class_eject_media (input_class_t *this_gen)
{
  return 1;
}

static void bluray_class_dispose (input_class_t *this_gen)
{
  bluray_input_class_t *this   = (bluray_input_class_t *) this_gen;
  config_values_t      *config = this->xine->config;

  config->unregister_callback(config, "media.bluray.mountpoint");

  free (this);
}

static void *bluray_init_plugin (xine_t *xine, void *data)
{
  config_values_t      *config = xine->config;
  bluray_input_class_t *this   = (bluray_input_class_t *) calloc(1, sizeof (bluray_input_class_t));

  this->xine = xine;

  this->input_class.get_instance       = bluray_class_get_instance;
  this->input_class.get_identifier     = bluray_class_get_identifier;
  this->input_class.get_description    = bluray_class_get_description;
  this->input_class.get_dir            = NULL;
  this->input_class.get_autoplay_list  = bluray_class_get_autoplay_list;
  this->input_class.dispose            = bluray_class_dispose;
  this->input_class.eject_media        = bluray_class_eject_media;

  this->mountpoint = config->register_filename(config, "media.bluray.mountpoint",
                                               "/mnt/bluray", XINE_CONFIG_STRING_IS_DIRECTORY_NAME,
                                               _("BluRay mount point"),
                                               _("Default mount location for BluRay discs."),
                                               0, mountpoint_change_cb, (void *) this);
  return this;
}

/*
 * exported plugin catalog entry
 */

const plugin_info_t xine_plugin_info[] EXPORTED = {
  /* type, API, "name", version, special_info, init_function */
  { PLUGIN_INPUT | PLUGIN_MUST_PRELOAD, 17, "BLURAY", XINE_VERSION_CODE, NULL, bluray_init_plugin },
  { PLUGIN_NONE, 0, "", 0, NULL, NULL }
};
