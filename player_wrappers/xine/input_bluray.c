/*
 * Copyright (C) 2000-2005 the xine project
 *
 * Copyright (C) 2009-2011 Petri Hintukainen <phintuka@users.sourceforge.net>
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
#include <pthread.h>

#include <libbluray/bluray.h>
#include <libbluray/keys.h>
#include <libbluray/overlay.h>
#include <libbluray/meta_data.h>

#define LOG_MODULE "input_bluray"
#define LOG_VERBOSE

#define LOG

#define LOGMSG(x...)  xine_log (this->stream->xine, XINE_LOG_MSG, "input_bluray: " x);

#define XINE_ENGINE_INTERNAL  // stream->demux_plugin

#ifdef HAVE_CONFIG_H
# include "xine_internal.h"
# include "input_plugin.h"
#else
# include <xine/xine_internal.h>
# include <xine/input_plugin.h>
#endif

#ifndef XINE_VERSION_CODE
# error XINE_VERSION_CODE undefined !
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

#define MIN_TITLE_LENGTH  180

typedef struct {

  input_class_t   input_class;

  xine_t         *xine;

  /* config */
  char           *mountpoint;
  char           *language;
  char           *country;
  int             region;
  int             parental;
} bluray_input_class_t;

typedef struct {
  input_plugin_t        input_plugin;

  xine_stream_t        *stream;
  xine_event_queue_t   *event_queue;
  xine_osd_t           *osd[2];

  bluray_input_class_t *class;

  char                 *mrl;
  char                 *disc_root;
  char                 *disc_name;

  BLURAY               *bdh;

  const BLURAY_DISC_INFO *disc_info;
  const META_DL          *meta_dl; /* disc library meta data */

  int                num_title_idx;     /* number of relevant playlists */
  int                current_title_idx;
  int                num_titles;        /* navigation mode, number of titles in disc index */
  int                current_title;     /* navigation mode, title from disc index */
  BLURAY_TITLE_INFO *title_info;
  pthread_mutex_t    title_info_mutex;  /* lock this when accessing title_info outside of input/demux thread */
  unsigned int       current_clip;
  time_t             still_end_time;
  int                error;
  int                menu_open;
  int                stream_flushed;
  int                pg_enable;
  int                pg_stream;
  int                mouse_inside_button;

  uint32_t           cap_seekable;
  uint8_t            nav_mode;

} bluray_input_plugin_t;

static void send_num_buttons(bluray_input_plugin_t *this, int n)
{
  xine_event_t   event;
  xine_ui_data_t data;

  event.type = XINE_EVENT_UI_NUM_BUTTONS;
  event.data = &data;
  event.data_length = sizeof(data);
  data.num_buttons = n;

  xine_event_send(this->stream, &event);
}

static xine_osd_t *get_overlay(bluray_input_plugin_t *this, int plane)
{
  if (!this->osd[plane]) {
    this->osd[plane] = xine_osd_new(this->stream, 0, 0, 1920, 1080);
  }
  if (!this->pg_enable) {
    _x_select_spu_channel(this->stream, -1);
  }
  return this->osd[plane];
}

static void close_overlay(bluray_input_plugin_t *this, int plane)
{
  if (plane < 0) {
    close_overlay(this, 0);
    close_overlay(this, 1);
    return;
  }

  if (plane < 2 && this->osd[plane]) {
    xine_osd_free(this->osd[plane]);
    this->osd[plane] = NULL;
    if (plane == 1) {
      send_num_buttons(this, 0);
      this->menu_open = 0;
    }
  }
}

static void open_overlay(bluray_input_plugin_t *this, const BD_OVERLAY * const ov)
{
  if (!this->osd[ov->plane]) {
    this->osd[ov->plane] = xine_osd_new(this->stream, ov->x, ov->y, ov->w, ov->h);
  }
}

static void draw_bitmap(xine_osd_t *osd, const BD_OVERLAY * const ov)
{
  unsigned i;

  /* convert and set palette */
  if (ov->palette) {
    uint32_t color[256];
    uint8_t  trans[256];
    for(i = 0; i < 256; i++) {
      trans[i] = ov->palette[i].T;
      color[i] = (ov->palette[i].Y << 16) | (ov->palette[i].Cr << 8) | ov->palette[i].Cb;
    }

    xine_osd_set_palette(osd, color, trans);
  }

  /* uncompress and draw bitmap */
  if (ov->img) {
    const BD_PG_RLE_ELEM *rlep = ov->img;
    uint8_t *img = malloc(ov->w * ov->h);
    unsigned pixels = ov->w * ov->h;

    for (i = 0; i < pixels; i += rlep->len, rlep++) {
      memset(img + i, rlep->color, rlep->len);
    }

    xine_osd_draw_bitmap(osd, img, ov->x, ov->y, ov->w, ov->h, NULL);

    free(img);
  }
}

static void overlay_proc(void *this_gen, const BD_OVERLAY * const ov)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  if (!this) {
    return;
  }

  if (!ov) {
    /* hide OSD */
    close_overlay(this, -1);
    return;
  }

  if (ov->plane > 1) {
    return;
  }

#if defined(BD_OVERLAY_INTERFACE_VERSION) && BD_OVERLAY_INTERFACE_VERSION >= 2

  switch (ov->cmd) {
    case BD_OVERLAY_INIT:    /* init overlay plane. Size of full plane in x,y,w,h */
      open_overlay(this, ov);
      return;
    case BD_OVERLAY_CLOSE:   /* close overlay */
      close_overlay(this, ov->plane);
      return;
  }

  xine_osd_t *osd = get_overlay(this, ov->plane);

  switch (ov->cmd) {
    case BD_OVERLAY_DRAW:    /* draw bitmap (x,y,w,h,img,palette) */
      draw_bitmap(osd, ov);
      return;

    case BD_OVERLAY_WIPE:    /* clear area (x,y,w,h) */
      xine_osd_draw_rect(osd, ov->x, ov->y, ov->x + ov->w - 1, ov->y + ov->h - 1, 0xff, 1);
      return;

    case BD_OVERLAY_CLEAR:   /* clear plane */
      xine_osd_draw_rect(osd, 0, 0, osd->osd.width - 1, osd->osd.height - 1, 0xff, 1);
      xine_osd_clear(osd);
      xine_osd_hide(osd, 0);
      return;

    case BD_OVERLAY_FLUSH:   /* all changes have been done, flush overlay to display at given pts */
      xine_osd_show(osd, 0);

      if (ov->plane == 1) {
        this->menu_open = 1;
        send_num_buttons(this, 1);
      }
      return;

    default:
      LOGMSG("unknown overlay command %d", ov->cmd);
      return;
  }

#else

  xine_osd_t *osd = get_overlay(this, ov->plane);

  if (ov->img) {
    draw_bitmap(osd, ov);

  } else {

    if (ov->x == 0 && ov->y == 0 && ov->w == 1920 && ov->h == 1080) {
      /* Nothing to display, close OSD */
      close_overlay(this, ov->plane);
      return;
    }

    /* wipe rect */
    xine_osd_draw_rect(osd, ov->x, ov->y, ov->x + ov->w - 1, ov->y + ov->h - 1, 0xff, 1);
  }

  /* display */

  xine_osd_show(osd, 0);

  if (ov->plane == 1) {
    this->menu_open = 1;
    send_num_buttons(this, 1);
  }
#endif
}

/*
 * stream info
 */

static void update_stream_info(bluray_input_plugin_t *this)
{
  if (this->title_info) {
    /* set stream info */
    _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_ANGLE_COUNT,    this->title_info->angle_count);
    _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_ANGLE_NUMBER,   bd_get_current_angle(this->bdh));
    _x_stream_info_set(this->stream, XINE_STREAM_INFO_HAS_CHAPTERS,       this->title_info->chapter_count > 0);
    _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_CHAPTER_COUNT,  this->title_info->chapter_count);
    _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_CHAPTER_NUMBER, bd_get_current_chapter(this->bdh) + 1);
  }
}

static void update_title_name(bluray_input_plugin_t *this)
{
  char           title_name[64] = "";
  xine_ui_data_t udata;
  xine_event_t   uevent = {
    .type        = XINE_EVENT_UI_SET_TITLE,
    .stream      = this->stream,
    .data        = &udata,
    .data_length = sizeof(udata)
  };

  /* check disc library metadata */
  if (this->meta_dl) {
    unsigned i;
    for (i = 0; i < this->meta_dl->toc_count; i++)
      if (this->meta_dl->toc_entries[i].title_number == (unsigned)this->current_title)
        if (this->meta_dl->toc_entries[i].title_name)
          if (strlen(this->meta_dl->toc_entries[i].title_name) > 2)
            strncpy(title_name, this->meta_dl->toc_entries[i].title_name, sizeof(title_name));
  }

  /* title name */
  if (title_name[0]) {
  } else if (this->current_title == BLURAY_TITLE_TOP_MENU) {
    strcpy(title_name, "Top Menu");
  } else if (this->current_title == BLURAY_TITLE_FIRST_PLAY) {
    strcpy(title_name, "First Play");
  } else if (this->nav_mode) {
    snprintf(title_name, sizeof(title_name), "Title %d/%d (PL %d/%d)",
             this->current_title, this->num_titles,
             this->current_title_idx + 1, this->num_title_idx);
  } else {
    snprintf(title_name, sizeof(title_name), "Title %d/%d",
             this->current_title_idx + 1, this->num_title_idx);
  }

  /* disc name */
  if (this->disc_name && this->disc_name[0]) {
    udata.str_len = snprintf(udata.str, sizeof(udata.str), "%s, %s",
                             this->disc_name, title_name);
  } else {
    udata.str_len = snprintf(udata.str, sizeof(udata.str), "%s",
                             title_name);
  }

  _x_meta_info_set(this->stream, XINE_META_INFO_TITLE, udata.str);

  xine_event_send(this->stream, &uevent);
}

static void update_title_info(bluray_input_plugin_t *this, int playlist_id)
{
  /* update title_info */

  pthread_mutex_lock(&this->title_info_mutex);

  if (this->title_info)
    bd_free_title_info(this->title_info);

  if (playlist_id < 0)
    this->title_info = bd_get_title_info(this->bdh, this->current_title_idx, 0);
  else
    this->title_info = bd_get_playlist_info(this->bdh, playlist_id, 0);

  pthread_mutex_unlock(&this->title_info_mutex);

  if (!this->title_info) {
    LOGMSG("bd_get_title_info(%d) failed\n", this->current_title_idx);
    return;
  }

  /* calculate and set stream rate */

  uint64_t rate = bd_get_title_size(this->bdh) * UINT64_C(8) // bits
                  * INT64_C(90000)
                  / (uint64_t)(this->title_info->duration);
  _x_stream_info_set(this->stream, XINE_STREAM_INFO_BITRATE, rate);

  /* set stream info */

  if (this->nav_mode) {
    _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_TITLE_COUNT,  this->num_titles);
    _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_TITLE_NUMBER, this->current_title);
  } else {
    _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_TITLE_COUNT,  this->num_title_idx);
    _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_TITLE_NUMBER, this->current_title_idx + 1);
  }

  update_stream_info(this);

  /* set title */
  update_title_name(this);
}

static int open_title (bluray_input_plugin_t *this, int title_idx)
{
  if (bd_select_title(this->bdh, title_idx) <= 0) {
    LOGMSG("bd_select_title(%d) failed\n", title_idx);
    return 0;
  }

  this->current_title_idx = title_idx;

  update_title_info(this, -1);

  return 1;
}

#ifndef DEMUX_OPTIONAL_DATA_FLUSH
#  define DEMUX_OPTIONAL_DATA_FLUSH 0x10000
#endif

static void stream_flush(bluray_input_plugin_t *this)
{
  if (this->stream_flushed)
    return;

  lprintf("Stream flush\n");

  this->stream_flushed = 1;

  int tmp = 0;
  if (DEMUX_OPTIONAL_SUCCESS !=
      this->stream->demux_plugin->get_optional_data(this->stream->demux_plugin, &tmp, DEMUX_OPTIONAL_DATA_FLUSH)) {
    LOGMSG("stream flush not supported by the demuxer !\n");
    return;
  }
}

static void stream_reset(bluray_input_plugin_t *this)
{
  if (!this || !this->stream || !this->stream->demux_plugin)
    return;

  lprintf("Stream reset\n");

  this->cap_seekable = 0;

  _x_set_fine_speed(this->stream, XINE_FINE_SPEED_NORMAL);
  this->stream->demux_plugin->seek(this->stream->demux_plugin, 0, 0, 1);
  _x_demux_control_start(this->stream);

  this->cap_seekable = INPUT_CAP_SEEKABLE;
}

static void wait_secs(bluray_input_plugin_t *this, unsigned seconds)
{
  stream_flush(this);

  if (this->still_end_time) {
    if (time(NULL) >= this->still_end_time) {
      lprintf("pause end\n");
      this->still_end_time = 0;
      bd_read_skip_still(this->bdh);
      stream_reset(this);
      return;
    }
  }

  else if (seconds) {
    if (seconds > 300) {
      seconds = 300;
    }

    lprintf("still image, pause for %d seconds\n", seconds);
    this->still_end_time = time(NULL) + seconds;
  }

  xine_usec_sleep(40*1000);
}

static void update_spu_channel(bluray_input_plugin_t *this, int channel)
{
  if (this->stream->video_fifo) {
    buf_element_t *buf = this->stream->video_fifo->buffer_pool_alloc(this->stream->video_fifo);
    buf->type = BUF_CONTROL_SPU_CHANNEL;
    buf->decoder_info[0] = channel;
    buf->decoder_info[1] = channel;
    buf->decoder_info[2] = channel;

    this->stream->video_fifo->put(this->stream->video_fifo, buf);
  }
}

static void update_audio_channel(bluray_input_plugin_t *this, int channel)
{
  if (this->stream->audio_fifo) {
    buf_element_t *buf = this->stream->audio_fifo->buffer_pool_alloc(this->stream->audio_fifo);
    buf->type = BUF_CONTROL_AUDIO_CHANNEL;
    buf->decoder_info[0] = channel;

    this->stream->audio_fifo->put(this->stream->audio_fifo, buf);
  }
}

static void handle_libbluray_event(bluray_input_plugin_t *this, BD_EVENT ev)
{
    switch (ev.event) {

      case BD_EVENT_ERROR:
        this->error = 1;
        return;

      case BD_EVENT_READ_ERROR:
        LOGMSG("m2ts file read error");
        /*stream_flush(this); leave error detection and handling for upper layer */
        return;

      case BD_EVENT_ENCRYPTED:
        lprintf("BD_EVENT_ENCRYPTED\n");
        _x_message (this->stream, XINE_MSG_ENCRYPTED_SOURCE,
                    "Media stream scrambled/encrypted", NULL);
        this->error = 1;
        return;

      /* playback control */

      case BD_EVENT_SEEK:
        lprintf("BD_EVENT_SEEK\n");
        this->still_end_time = 0;
        stream_reset(this);
        break;

      case BD_EVENT_STILL_TIME:
        lprintf("BD_EVENT_STILL_TIME %d\n", ev.param);
        wait_secs(this, ev.param);
        break;

      case BD_EVENT_STILL:
        lprintf("BD_EVENT_STILL %d\n", ev.param);
        int paused = _x_get_fine_speed(this->stream) == XINE_SPEED_PAUSE;
        if (paused && !ev.param) {
          _x_set_fine_speed(this->stream, XINE_FINE_SPEED_NORMAL);
        }
        if (!paused && ev.param) {
          _x_set_fine_speed(this->stream, XINE_SPEED_PAUSE);
        }
        break;

      /* playback position */

      case BD_EVENT_ANGLE:
        lprintf("BD_EVENT_ANGLE_NUMBER %d\n", ev.param);
        _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_ANGLE_NUMBER, ev.param);
        break;

      case BD_EVENT_END_OF_TITLE:
        lprintf("BD_EVENT_END_OF_TITLE\n");
        stream_flush(this);
        break;

      case BD_EVENT_TITLE:
        this->current_title = ev.param;
        break;

      case BD_EVENT_PLAYLIST:
        lprintf("BD_EVENT_PLAYLIST %d\n", ev.param);
        this->current_title_idx = bd_get_current_title(this->bdh);
        this->current_clip = 0;
        update_title_info(this, ev.param);
        stream_reset(this);
        break;

      case BD_EVENT_PLAYITEM:
        lprintf("BD_EVENT_PLAYITEM %d\n", ev.param);
        this->current_clip = ev.param;
        this->still_end_time = 0;
        break;

      case BD_EVENT_CHAPTER:
        lprintf("BD_EVENT_CHAPTER %d\n", ev.param);
        _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_CHAPTER_NUMBER, ev.param);
        break;

      /* stream selection */

      case BD_EVENT_AUDIO_STREAM:
        lprintf("BD_EVENT_AUDIO_STREAM %d\n", ev.param);
        update_audio_channel(this, ev.param - 1);
        break;

      case BD_EVENT_PG_TEXTST:
        lprintf("BD_EVENT_PG_TEXTST %s\n", ev.param ? "ON" : "OFF");
        this->pg_enable = ev.param;
        update_spu_channel(this, this->pg_enable ? this->pg_stream : -1);
        break;

      case BD_EVENT_PG_TEXTST_STREAM:
        lprintf("BD_EVENT_PG_TEXTST_STREAM %d\n", ev.param);
        this->pg_stream = ev.param - 1;
        if (this->pg_enable) {
          update_spu_channel(this, this->pg_stream);
        }
        break;

      case BD_EVENT_IG_STREAM:
      case BD_EVENT_SECONDARY_AUDIO:
      case BD_EVENT_SECONDARY_AUDIO_STREAM:
      case BD_EVENT_SECONDARY_VIDEO:
      case BD_EVENT_SECONDARY_VIDEO_SIZE:
      case BD_EVENT_SECONDARY_VIDEO_STREAM:

      case BD_EVENT_NONE:
        break;

      default:
        LOGMSG("unhandled libbluray event %d [param %d]\n", ev.event, ev.param);
        break;
    }
}

static void handle_libbluray_events(bluray_input_plugin_t *this)
{
  BD_EVENT ev;
  while (bd_get_event(this->bdh, &ev)) {
    handle_libbluray_event(this, ev);
    if (this->error || ev.event == BD_EVENT_NONE || ev.event == BD_EVENT_ERROR)
      break;
  }
}

static void send_mouse_enter_leave_event(bluray_input_plugin_t *this, int direction)
{
  if (direction != this->mouse_inside_button) {
    xine_event_t        event;
    xine_spu_button_t   spu_event;

    spu_event.direction = direction;
    spu_event.button    = 1;

    event.type        = XINE_EVENT_SPU_BUTTON;
    event.stream      = this->stream;
    event.data        = &spu_event;
    event.data_length = sizeof(spu_event);
    xine_event_send(this->stream, &event);

    this->mouse_inside_button = direction;
  }
}

static void handle_events(bluray_input_plugin_t *this)
{
  if (!this->event_queue)
    return;

  xine_event_t *event;
  while (NULL != (event = xine_event_get(this->event_queue))) {

    if (!this->bdh || !this->title_info) {
      xine_event_free(event);
      return;
    }

    int64_t pts = xine_get_current_vpts(this->stream) -
      this->stream->metronom->get_option(this->stream->metronom, METRONOM_VPTS_OFFSET);

    if (this->menu_open) {
      switch (event->type) {
        case XINE_EVENT_INPUT_LEFT:      bd_user_input(this->bdh, pts, BD_VK_LEFT);  break;
        case XINE_EVENT_INPUT_RIGHT:     bd_user_input(this->bdh, pts, BD_VK_RIGHT); break;
      }
    } else {
      switch (event->type) {

        case XINE_EVENT_INPUT_LEFT:
          lprintf("XINE_EVENT_INPUT_LEFT: previous title\n");
          if (!this->nav_mode) {
            open_title(this, MAX(0, this->current_title_idx - 1));
          } else {
            bd_play_title(this->bdh, MAX(1, this->current_title - 1));
          }
          stream_reset(this);
          break;

        case XINE_EVENT_INPUT_RIGHT:
          lprintf("XINE_EVENT_INPUT_RIGHT: next title\n");
          if (!this->nav_mode) {
            open_title(this, MIN(this->num_title_idx - 1, this->current_title_idx + 1));
          } else {
            bd_play_title(this->bdh, MIN(this->num_titles, this->current_title + 1));
          }
          stream_reset(this);
          break;
      }
    }

    switch (event->type) {

      case XINE_EVENT_INPUT_MOUSE_BUTTON: {
        xine_input_data_t *input = event->data;
        lprintf("mouse click: button %d at (%d,%d)\n", input->button, input->x, input->y);
        if (input->button == 1) {
          bd_mouse_select(this->bdh, pts, input->x, input->y);
          bd_user_input(this->bdh, pts, BD_VK_MOUSE_ACTIVATE);
          send_mouse_enter_leave_event(this, 0);
        }
        break;
      }

      case XINE_EVENT_INPUT_MOUSE_MOVE: {
        xine_input_data_t *input = event->data;
        if (bd_mouse_select(this->bdh, pts, input->x, input->y) > 0) {
          send_mouse_enter_leave_event(this, 1);
        } else {
          send_mouse_enter_leave_event(this, 0);
        }
        break;
      }

      case XINE_EVENT_INPUT_MENU1:
        if (!this->disc_info->top_menu_supported) {
          _x_message (this->stream, XINE_MSG_GENERAL_WARNING,
                      "Can't open Top Menu",
                      "Top Menu title not supported", NULL);
        }
        bd_menu_call(this->bdh, pts);
        break;

      case XINE_EVENT_INPUT_MENU2:     bd_user_input(this->bdh, pts, BD_VK_POPUP); break;
      case XINE_EVENT_INPUT_UP:        bd_user_input(this->bdh, pts, BD_VK_UP);    break;
      case XINE_EVENT_INPUT_DOWN:      bd_user_input(this->bdh, pts, BD_VK_DOWN);  break;
      case XINE_EVENT_INPUT_SELECT:    bd_user_input(this->bdh, pts, BD_VK_ENTER); break;
      case XINE_EVENT_INPUT_NUMBER_0:  bd_user_input(this->bdh, pts, BD_VK_0); break;
      case XINE_EVENT_INPUT_NUMBER_1:  bd_user_input(this->bdh, pts, BD_VK_1); break;
      case XINE_EVENT_INPUT_NUMBER_2:  bd_user_input(this->bdh, pts, BD_VK_2); break;
      case XINE_EVENT_INPUT_NUMBER_3:  bd_user_input(this->bdh, pts, BD_VK_3); break;
      case XINE_EVENT_INPUT_NUMBER_4:  bd_user_input(this->bdh, pts, BD_VK_4); break;
      case XINE_EVENT_INPUT_NUMBER_5:  bd_user_input(this->bdh, pts, BD_VK_5); break;
      case XINE_EVENT_INPUT_NUMBER_6:  bd_user_input(this->bdh, pts, BD_VK_6); break;
      case XINE_EVENT_INPUT_NUMBER_7:  bd_user_input(this->bdh, pts, BD_VK_7); break;
      case XINE_EVENT_INPUT_NUMBER_8:  bd_user_input(this->bdh, pts, BD_VK_8); break;
      case XINE_EVENT_INPUT_NUMBER_9:  bd_user_input(this->bdh, pts, BD_VK_9); break;

      case XINE_EVENT_INPUT_NEXT: {
        cfg_entry_t* entry = this->class->xine->config->lookup_entry(this->class->xine->config,
                                                                     "media.bluray.skip_behaviour");
        switch (entry->num_value) {
          case 0: /* skip by chapter */
            bd_seek_chapter(this->bdh, bd_get_current_chapter(this->bdh) + 1);
            update_stream_info(this);
            break;
          case 1: /* skip by title */
            if (!this->nav_mode) {
              open_title(this, MIN(this->num_title_idx - 1, this->current_title_idx + 1));
            } else {
              bd_play_title(this->bdh, MIN(this->num_titles, this->current_title + 1));
            }
            break;
        }
        stream_reset(this);
        break;
      }

      case XINE_EVENT_INPUT_PREVIOUS: {
        cfg_entry_t* entry = this->class->xine->config->lookup_entry(this->class->xine->config,
                                                                     "media.bluray.skip_behaviour");
        switch (entry->num_value) {
          case 0: /* skip by chapter */
            bd_seek_chapter(this->bdh, MAX(0, ((int)bd_get_current_chapter(this->bdh)) - 1));
            update_stream_info(this);
            break;
          case 1: /* skip by title */
            if (!this->nav_mode) {
              open_title(this, MAX(0, this->current_title_idx - 1));
            } else {
              bd_play_title(this->bdh, MAX(1, this->current_title - 1));
            }
            break;
        }
        stream_reset(this);
        break;
      }

      case XINE_EVENT_INPUT_ANGLE_NEXT: {
        unsigned curr_angle = bd_get_current_angle(this->bdh);
        unsigned angle      = MIN(8, curr_angle + 1);
        lprintf("XINE_EVENT_INPUT_ANGLE_NEXT: set angle %d --> %d\n", curr_angle, angle);
        bd_seamless_angle_change(this->bdh, angle);
        _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_ANGLE_NUMBER, bd_get_current_angle(this->bdh));
        break;
      }

      case XINE_EVENT_INPUT_ANGLE_PREVIOUS: {
        unsigned curr_angle = bd_get_current_angle(this->bdh);
        unsigned angle      = curr_angle ? curr_angle - 1 : 0;
        lprintf("XINE_EVENT_INPUT_ANGLE_PREVIOUS: set angle %d --> %d\n", curr_angle, angle);
        bd_seamless_angle_change(this->bdh, angle);
        _x_stream_info_set(this->stream, XINE_STREAM_INFO_DVD_ANGLE_NUMBER, bd_get_current_angle(this->bdh));
        break;
      }
    }

    xine_event_free(event);
  }
}

/*
 * xine plugin interface
 */

static uint32_t bluray_plugin_get_capabilities (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;
  return this->cap_seekable  |
         INPUT_CAP_BLOCK     |
         INPUT_CAP_AUDIOLANG |
         INPUT_CAP_SPULANG   |
         INPUT_CAP_CHAPTERS;
}

#if XINE_VERSION_CODE >= 10190
static off_t bluray_plugin_read (input_plugin_t *this_gen, void *buf, off_t len)
#else
static off_t bluray_plugin_read (input_plugin_t *this_gen, char *buf, off_t len)
#endif
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;
  off_t result;

  if (!this || !this->bdh || len < 0 || this->error)
    return -1;

  handle_events(this);

  if (this->nav_mode) {
    do {
      BD_EVENT ev;
      result = bd_read_ext (this->bdh, (unsigned char *)buf, len, &ev);
      handle_libbluray_event(this, ev);
      if (result == 0) {
        handle_events(this);
        if (ev.event == BD_EVENT_NONE) {
          if (_x_action_pending(this->stream)) {
            break;
          }
        }
      }
    } while (!this->error && result == 0);
  } else {
    result = bd_read (this->bdh, (unsigned char *)buf, len);
    handle_libbluray_events(this);
  }

  if (result < 0)
    LOGMSG("bd_read() failed: %s (%d of %d)\n", strerror(errno), (int)result, (int)len);

  this->stream_flushed = 0;

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
  if (this->still_end_time)
    return offset;

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

  if (!this || !this->bdh)
    return -1;

  if (this->still_end_time)
    return bd_tell(this->bdh);

  /* convert relative seeks to absolute */

  if (origin == SEEK_CUR) {
    time_offset += this_gen->get_current_time(this_gen);
  }
  else if (origin == SEEK_END) {

    pthread_mutex_lock(&this->title_info_mutex);

    if (!this->title_info) {
      pthread_mutex_unlock(&this->title_info_mutex);
      return -1;
    }

    int duration = this->title_info->duration / 90;
    if (time_offset < duration)
      time_offset = duration - time_offset;
    else
      time_offset = 0;

    pthread_mutex_unlock(&this->title_info_mutex);
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
  (void)this_gen;

  return ALIGNED_UNIT_SIZE;
}

static const char* bluray_plugin_get_mrl (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  return this->mrl;
}

static int get_optional_data_impl (bluray_input_plugin_t *this, void *data, int data_type)
{
  unsigned int current_clip = this->current_clip;

  switch (data_type) {
    case INPUT_OPTIONAL_DATA_DEMUXER:
      *(const char **)data = "mpeg-ts";
      return INPUT_OPTIONAL_SUCCESS;

    /*
     * audio track language:
     * - channel number can be mpeg-ts PID (0x1100 ... 0x11ff)
     */
    case INPUT_OPTIONAL_DATA_AUDIOLANG:
      if (this->title_info && this->title_info->clip_count < current_clip) {
        int               channel = *((int *)data);
        BLURAY_CLIP_INFO *clip    = &this->title_info->clips[current_clip];

        if (channel >= 0 && channel < clip->audio_stream_count) {
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
      if (this->title_info && this->title_info->clip_count < current_clip) {
        int               channel = *((int *)data);
        BLURAY_CLIP_INFO *clip    = &this->title_info->clips[current_clip];

        if (channel >= 0 && channel < clip->pg_stream_count) {
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

static int bluray_plugin_get_optional_data (input_plugin_t *this_gen, void *data, int data_type)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;
  int r = INPUT_OPTIONAL_UNSUPPORTED;

  if (this && this->stream && data) {
    pthread_mutex_lock(&this->title_info_mutex);
    r = get_optional_data_impl(this, data, data_type);
    pthread_mutex_unlock(&this->title_info_mutex);
  }

  return r;
}

static void bluray_plugin_dispose (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this = (bluray_input_plugin_t *) this_gen;

  if (this->bdh)
    bd_register_overlay_proc(this->bdh, NULL, NULL);

  close_overlay(this, -1);

  if (this->event_queue)
    xine_event_dispose_queue(this->event_queue);

  pthread_mutex_lock(&this->title_info_mutex);
  if (this->title_info)
    bd_free_title_info(this->title_info);
  this->title_info = NULL;
  pthread_mutex_unlock(&this->title_info_mutex);

  pthread_mutex_destroy(&this->title_info_mutex);

  if (this->bdh)
    bd_close(this->bdh);

  free (this->mrl);
  free (this->disc_root);
  free (this->disc_name);

  free (this);
}

static int parse_mrl(const char *mrl_in, char **path, int *title, int *chapter)
{
  int skip = 0;

  if (!strncasecmp(mrl_in, "bluray:", 7))
    skip = 7;
  else if (!strncasecmp(mrl_in, "bd:", 3))
    skip = 3;
  else
    return -1;

  char *mrl = strdup(mrl_in + skip);

  /* title[.chapter] given ? parse and drop it */
  if (mrl[strlen(mrl)-1] != '/') {
    char *end = strrchr(mrl, '/');
    if (end && end[1]) {
      if (sscanf(end, "/%d.%d", title, chapter) < 1)
        *title = -1;
      else
        *end = 0;
    }
  }
  lprintf(" -> title %d, chapter %d, mrl \'%s\'\n", *title, *chapter, mrl);

  if ((mrl[0] == 0) ||
      (mrl[1] == 0 && mrl[0] == '/') ||
      (mrl[2] == 0 && mrl[1] == '/' && mrl[0] == '/') ||
      (mrl[3] == 0 && mrl[2] == '/' && mrl[1] == '/' && mrl[0] == '/')){

    /* default device */
    *path = NULL;

  } else if (*mrl == '/') {

    /* strip extra slashes */
    char *start = mrl;
    while (start[0] == '/' && start[1] == '/')
      start++;

    *path = strdup(start);

    _x_mrl_unescape(*path);

    lprintf("non-defaut mount point \'%s\'\n", *path);

  } else {
    lprintf("invalid mrl \'%s\'\n", mrl_in);
    free(mrl);
    return 0;
  }

  free(mrl);

  return 1;
}

static int get_disc_info(bluray_input_plugin_t *this)
{
  const BLURAY_DISC_INFO *disc_info;

  disc_info = bd_get_disc_info(this->bdh);

  if (!disc_info) {
    LOGMSG("bd_get_disc_info() failed\n");
    return -1;
  }

  if (!disc_info->bluray_detected) {
    LOGMSG("bd_get_disc_info(): BluRay not detected\n");
    this->nav_mode = 0;
    return 0;
  }

  if (disc_info->aacs_detected && !disc_info->aacs_handled) {
    if (!disc_info->libaacs_detected)
      _x_message (this->stream, XINE_MSG_ENCRYPTED_SOURCE,
                  "Media stream scrambled/encrypted with AACS",
                  "libaacs not installed", NULL);
    else
      _x_message (this->stream, XINE_MSG_ENCRYPTED_SOURCE,
                  "Media stream scrambled/encrypted with AACS", NULL);
    return -1;
  }

  if (disc_info->bdplus_detected && !disc_info->bdplus_handled) {
    if (!disc_info->libbdplus_detected)
      _x_message (this->stream, XINE_MSG_ENCRYPTED_SOURCE,
                  "Media scrambled/encrypted with BD+",
                  "libbdplus not installed.", NULL);
    else
      _x_message (this->stream, XINE_MSG_ENCRYPTED_SOURCE,
                  "Media stream scrambled/encrypted with BD+", NULL);
    return -1;
  }

  if (this->nav_mode && !disc_info->first_play_supported) {
    _x_message (this->stream, XINE_MSG_GENERAL_WARNING,
                "Can't play disc in HDMV navigation mode",
                "First Play title not supported", NULL);
    this->nav_mode = 0;
  }

  if (this->nav_mode && disc_info->num_unsupported_titles > 0) {
    _x_message (this->stream, XINE_MSG_GENERAL_WARNING,
                "Unsupported titles found",
                "Some titles can't be played in navigation mode", NULL);
  }

  this->num_titles = disc_info->num_hdmv_titles + disc_info->num_bdj_titles;
  this->disc_info  = disc_info;

  return 1;
}

static int bluray_plugin_open (input_plugin_t *this_gen)
{
  bluray_input_plugin_t *this    = (bluray_input_plugin_t *) this_gen;
  int                    title   = -1;
  int                    chapter = 0;

  lprintf("bluray_plugin_open\n");

  /* validate and parse mrl */
  if (!parse_mrl(this->mrl, &this->disc_root, &title, &chapter))
    return -1;

  if (!strncasecmp(this->mrl, "bd:", 3))
    this->nav_mode = 1;

  if (!this->disc_root)
    this->disc_root = strdup(this->class->mountpoint);

  /* open libbluray */

  if (! (this->bdh = bd_open (this->disc_root, NULL))) {
    LOGMSG("bd_open(\'%s\') failed: %s\n", this->disc_root, strerror(errno));
    return -1;
  }
  lprintf("bd_open(\'%s\') OK\n", this->disc_root);

  if (get_disc_info(this) < 0) {
    return -1;
  }

  /* load title list */

  this->num_title_idx = bd_get_titles(this->bdh, TITLES_RELEVANT, MIN_TITLE_LENGTH);
  LOGMSG("%d titles\n", this->num_title_idx);

  if (this->num_title_idx < 1)
    return -1;

  /* select title */

  /* if title was not in mrl, find the main title */
  if (title < 0) {
    uint64_t duration = 0;
    int i, playlist = 99999;
    for (i = 0; i < this->num_title_idx; i++) {
      BLURAY_TITLE_INFO *info = bd_get_title_info(this->bdh, i, 0);
      if (info->duration > duration) {
        title    = i;
        duration = info->duration;
        playlist = info->playlist;
      }
      bd_free_title_info(info);
    }
    lprintf("main title: %d (%05d.mpls)\n", title, playlist);
  }

  /* update player settings */

  bd_set_player_setting    (this->bdh, BLURAY_PLAYER_SETTING_REGION_CODE,  this->class->region);
  bd_set_player_setting    (this->bdh, BLURAY_PLAYER_SETTING_PARENTAL,     this->class->parental);
  bd_set_player_setting_str(this->bdh, BLURAY_PLAYER_SETTING_AUDIO_LANG,   this->class->language);
  bd_set_player_setting_str(this->bdh, BLURAY_PLAYER_SETTING_PG_LANG,      this->class->language);
  bd_set_player_setting_str(this->bdh, BLURAY_PLAYER_SETTING_MENU_LANG,    this->class->language);
  bd_set_player_setting_str(this->bdh, BLURAY_PLAYER_SETTING_COUNTRY_CODE, this->class->country);

  /* init event queue */
  bd_get_event(this->bdh, NULL);

  /* get disc name */

  this->meta_dl = bd_get_meta(this->bdh);

  if (this->meta_dl && this->meta_dl->di_name && strlen(this->meta_dl->di_name) > 1) {
    this->disc_name = strdup(this->meta_dl->di_name);
  }
  else if (strcmp(this->disc_root, this->class->mountpoint)) {
    char *t = strrchr(this->disc_root, '/');
    if (!t[1])
      while (t > this->disc_root && t[-1] != '/') t--;
    else
      while (t[0] == '/') t++;
    this->disc_name = strdup(t);
    char *end = this->disc_name + strlen(this->disc_name) - 1;
    if (*end ==  '/')
      *end = 0;
  }

  /* register overlay (graphics) handler */

  bd_register_overlay_proc(this->bdh, this, overlay_proc);

  /* open */
  this->current_title = -1;
  this->current_title_idx = -1;

  if (this->nav_mode) {
    if (bd_play(this->bdh) <= 0) {
      LOGMSG("bd_play() failed\n");
      return -1;
    }

  } else {
    if (open_title(this, title) <= 0 &&
        open_title(this, 0) <= 0)
      return -1;
  }

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

  if (strncasecmp(mrl, "bluray:", 7) && strncasecmp(mrl, "bd:", 3))
    return NULL;

  this = (bluray_input_plugin_t *) calloc(1, sizeof (bluray_input_plugin_t));

  this->stream = stream;
  this->class  = (bluray_input_class_t*)cls_gen;
  this->mrl    = strdup(mrl);

  this->cap_seekable = INPUT_CAP_SEEKABLE;

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

  this->event_queue = xine_event_new_queue (this->stream);

  pthread_mutex_init(&this->title_info_mutex, NULL);

  this->pg_stream = -1;

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

static void language_change_cb(void *data, xine_cfg_entry_t *cfg)
{
  bluray_input_class_t *this = (bluray_input_class_t *) data;

  this->language = cfg->str_value;
}

static void country_change_cb(void *data, xine_cfg_entry_t *cfg)
{
  bluray_input_class_t *this = (bluray_input_class_t *) data;

  this->country = cfg->str_value;
}

static void region_change_cb(void *data, xine_cfg_entry_t *cfg)
{
  bluray_input_class_t *this = (bluray_input_class_t *) data;

  this->region = cfg->num_value;
}

static void parental_change_cb(void *data, xine_cfg_entry_t *cfg)
{
  bluray_input_class_t *this = (bluray_input_class_t *) data;

  this->parental = cfg->num_value;
}

#if INPUT_PLUGIN_IFACE_VERSION < 18
static const char *bluray_class_get_description (input_class_t *this_gen)
{
  (void)this_gen;

  return _("BluRay input plugin");
}
#endif

#if INPUT_PLUGIN_IFACE_VERSION < 18
static const char *bluray_class_get_identifier (input_class_t *this_gen)
{
  (void)this_gen;

  return "bluray";
}
#endif

static char **bluray_class_get_autoplay_list (input_class_t *this_gen, int *num_files)
{
  (void)this_gen;

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
  config->unregister_callback(config, "media.bluray.region");
  config->unregister_callback(config, "media.bluray.language");
  config->unregister_callback(config, "media.bluray.country");
  config->unregister_callback(config, "media.bluray.parental");

  free (this);
}

static void *bluray_init_plugin (xine_t *xine, void *data)
{
  (void)data;

  static char *skip_modes[] = {"skip chapter", "skip title", NULL};

  config_values_t      *config = xine->config;
  bluray_input_class_t *this   = (bluray_input_class_t *) calloc(1, sizeof (bluray_input_class_t));

  this->xine = xine;

  this->input_class.get_instance       = bluray_class_get_instance;
#if INPUT_PLUGIN_IFACE_VERSION < 18
  this->input_class.get_identifier     = bluray_class_get_identifier;
  this->input_class.get_description    = bluray_class_get_description;
#else
  this->input_class.identifier         = "bluray";
  this->input_class.description        = _("BluRay input plugin");
#endif
  this->input_class.get_dir            = NULL;
  this->input_class.get_autoplay_list  = bluray_class_get_autoplay_list;
  this->input_class.dispose            = bluray_class_dispose;
  this->input_class.eject_media        = bluray_class_eject_media;

  this->mountpoint = config->register_filename(config, "media.bluray.mountpoint",
                                               "/mnt/bluray", XINE_CONFIG_STRING_IS_DIRECTORY_NAME,
                                               _("BluRay mount point"),
                                               _("Default mount location for BluRay discs."),
                                               0, mountpoint_change_cb, (void *) this);

  /* Player settings */
  this->language =
    config->register_string(config, "media.bluray.language",
                            "eng",
                            _("default language for BluRay playback"),
                            _("xine tries to use this language as a default for BluRay playback. "
                              "As far as the BluRay supports it, menus and audio tracks will be presented "
                              "in this language.\nThe value must be a three character"
                              "ISO639-2 language code."),
                            0, language_change_cb, this);
  this->country =
    config->register_string(config, "media.bluray.country",
                            "en",
                            _("BluRay player country code"),
                            _("The value must be a two character ISO3166-1 country code."),
                            0, country_change_cb, this);
  this->region =
    config->register_num(config, "media.bluray.region",
                         7,
                         _("BluRay player region code (1=A, 2=B, 4=C)"),
                         _("This only needs to be changed if your BluRay jumps to a screen "
                           "complaining about a wrong region code. It has nothing to do with "
                           "the region code set in BluRay drives, this is purely software."),
                         0, region_change_cb, this);
  this->parental =
    config->register_num(config, "media.bluray.parental",
                         99,
                         _("parental control age limit (1-99)"),
                         _("Prevents playback of BluRay titles where parental "
                           "control age limit is higher than this limit"),
                         0, parental_change_cb, this);

  /* */
  config->register_enum(config, "media.bluray.skip_behaviour", 0,
                        skip_modes,
                        _("unit for the skip action"),
                        _("You can configure the behaviour when issuing a skip command (using the skip "
                          "buttons for example)."),
                        20, NULL, NULL);

  return this;
}

/*
 * exported plugin catalog entry
 */

const plugin_info_t xine_plugin_info[] EXPORTED = {
  /* type, API, "name", version, special_info, init_function */
#if INPUT_PLUGIN_IFACE_VERSION <= 17
  { PLUGIN_INPUT | PLUGIN_MUST_PRELOAD, 17, "BLURAY", XINE_VERSION_CODE, NULL, bluray_init_plugin },
  { PLUGIN_INPUT | PLUGIN_MUST_PRELOAD, 17, "BD",     XINE_VERSION_CODE, NULL, bluray_init_plugin },
#elif INPUT_PLUGIN_IFACE_VERSION >= 18
  { PLUGIN_INPUT | PLUGIN_MUST_PRELOAD, 18, "BLURAY", XINE_VERSION_CODE, NULL, bluray_init_plugin },
  { PLUGIN_INPUT | PLUGIN_MUST_PRELOAD, 18, "BD",     XINE_VERSION_CODE, NULL, bluray_init_plugin },
#endif
  { PLUGIN_NONE, 0, "", 0, NULL, NULL }
};
