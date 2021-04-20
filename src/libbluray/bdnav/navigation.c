/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  John Stebbins
 * Copyright (C) 2010-2019  Petri Hintukainen <phintuka@users.sourceforge.net>
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

#include "navigation.h"

#include "clpi_parse.h"
#include "clpi_data.h"
#include "mpls_parse.h"
#include "mpls_data.h"
#include "bdparse.h"

#include "disc/disc.h"

#include "util/macro.h"
#include "util/logging.h"
#include "util/strutl.h"
#include "file/file.h"

#include <stdlib.h>
#include <string.h>

/*
 * Utils
 */

static uint32_t
_pl_duration(const MPLS_PL *pl)
{
    unsigned ii;
    uint32_t duration = 0;
    const MPLS_PI *pi;

    for (ii = 0; ii < pl->list_count; ii++) {
        pi = &pl->play_item[ii];
        duration += pi->out_time - pi->in_time;
    }
    return duration;
}

static uint32_t
_pl_chapter_count(const MPLS_PL *pl)
{
    unsigned ii, chapters = 0;

    // Count the number of "entry" marks (skipping "link" marks)
    // This is the the number of chapters
    for (ii = 0; ii < pl->mark_count; ii++) {
        if (pl->play_mark[ii].mark_type == BD_MARK_ENTRY) {
            chapters++;
        }
    }
    return chapters;
}

static uint32_t
_pl_streams_score(const MPLS_PL *pl)
{
    const MPLS_PI *pi;
    uint32_t i_num_audio = 0;
    uint32_t i_num_pg = 0;

    for (int ii = 0; ii < pl->list_count; ii++) {
        pi = &pl->play_item[ii];
        if(pi->stn.num_audio > i_num_audio)
            i_num_audio= pi->stn.num_audio;

        if(pi->stn.num_pg > i_num_pg)
            i_num_pg = pi->stn.num_pg;
    }

    return i_num_audio * 2 + i_num_pg;
}

/*
 * Check if two playlists are the same
 */

static int _stream_cmp(const MPLS_STREAM *a, const MPLS_STREAM *b)
{
    if (a->stream_type == b->stream_type &&
        a->coding_type == b->coding_type &&
        a->pid         == b->pid         &&
        a->subpath_id  == b->subpath_id  &&
        a->subclip_id  == b->subclip_id  &&
        a->format      == b->format      &&
        a->rate        == b->rate        &&
        a->char_code   == b->char_code   &&
        memcmp(a->lang, b->lang, 4) == 0) {
        return 0;
    }
    return 1;
}

static int _streams_cmp(const MPLS_STREAM *s1, const MPLS_STREAM *s2, unsigned count)
{
    unsigned ii;
    for (ii = 0; ii < count; ii++) {
        if (_stream_cmp(&s1[ii], &s2[ii])) {
          return 1;
        }
    }
    return 0;
}

static int _pi_cmp(const MPLS_PI *pi1, const MPLS_PI *pi2)
{
    if (memcmp(pi1->clip[0].clip_id, pi2->clip[0].clip_id, 5) != 0 ||
        pi1->in_time != pi2->in_time ||
        pi1->out_time != pi2->out_time) {
        return 1;
    }

    if (pi1->stn.num_video           != pi2->stn.num_video  ||
        pi1->stn.num_audio           != pi2->stn.num_audio  ||
        pi1->stn.num_pg              != pi2->stn.num_pg   ||
        pi1->stn.num_ig              != pi2->stn.num_ig   ||
        pi1->stn.num_secondary_audio != pi2->stn.num_secondary_audio ||
        pi1->stn.num_secondary_video != pi2->stn.num_secondary_video) {
        return 1;
    }

    if (_streams_cmp(pi1->stn.video,           pi2->stn.video,           pi1->stn.num_video) ||
        _streams_cmp(pi1->stn.audio,           pi2->stn.audio,           pi1->stn.num_audio) ||
        _streams_cmp(pi1->stn.pg,              pi2->stn.pg,              pi1->stn.num_pg) ||
        _streams_cmp(pi1->stn.ig,              pi2->stn.ig,              pi1->stn.num_ig) ||
        _streams_cmp(pi1->stn.secondary_audio, pi2->stn.secondary_audio, pi1->stn.num_secondary_audio) ||
        _streams_cmp(pi1->stn.secondary_video, pi2->stn.secondary_video, pi1->stn.num_secondary_video)) {
        return 1;
    }

    return 0;
}

static int _pm_cmp(const MPLS_PLM *pm1, const MPLS_PLM *pm2)
{
    if (pm1->mark_type     == pm2->mark_type     &&
        pm1->play_item_ref == pm2->play_item_ref &&
        pm1->time          == pm2->time          &&
        pm1->entry_es_pid  == pm2->entry_es_pid  &&
        pm1->duration      == pm2->duration ) {
        return 0;
    }

    return 1;
}

static int _pl_cmp(const MPLS_PL *pl1, const MPLS_PL *pl2)
{
    unsigned ii;

    if (pl1->list_count != pl2->list_count) {
        return 1;
    }
    if (pl1->mark_count != pl2->mark_count) {
        return 1;
    }
    if (pl1->sub_count != pl2->sub_count) {
        return 1;
    }
    if (pl1->ext_sub_count != pl2->ext_sub_count) {
        return 1;
    }

    for (ii = 0; ii < pl1->mark_count; ii++) {
        if (_pm_cmp(&pl1->play_mark[ii], &pl2->play_mark[ii])) {
            return 1;
        }
    }
    for (ii = 0; ii < pl1->list_count; ii++) {
        if (_pi_cmp(&pl1->play_item[ii], &pl2->play_item[ii])) {
            return 1;
        }
    }

    return 0;
}

/*
 * Playlist filtering
 */

/* return 0 if duplicate playlist */
static int _filter_dup(MPLS_PL *pl_list[], unsigned count, const MPLS_PL *pl)
{
    unsigned ii;

    for (ii = 0; ii < count; ii++) {
        if (!_pl_cmp(pl, pl_list[ii])) {
            return 0;
        }
    }
    return 1;
}

static unsigned int
_find_repeats(const MPLS_PL *pl, const char *m2ts, uint32_t in_time, uint32_t out_time)
{
    unsigned ii, count = 0;

    for (ii = 0; ii < pl->list_count; ii++) {
        const MPLS_PI *pi;

        pi = &pl->play_item[ii];
        // Ignore titles with repeated segments
        if (strcmp(pi->clip[0].clip_id, m2ts) == 0 &&
            pi->in_time  == in_time &&
            pi->out_time == out_time) {
          count++;
        }
    }
    return count;
}

static int
_filter_repeats(const MPLS_PL *pl, unsigned repeats)
{
    unsigned ii;

    for (ii = 0; ii < pl->list_count; ii++) {
      const MPLS_PI *pi;

      pi = &pl->play_item[ii];
      // Ignore titles with repeated segments
      if (_find_repeats(pl, pi->clip[0].clip_id, pi->in_time, pi->out_time) > repeats) {
          return 0;
      }
    }
    return 1;
}

/*
 * find main movie playlist
 */

#define DBG_MAIN_PL DBG_NAV

static void _video_props(const MPLS_STN *s, int *format, int *codec)
{
    unsigned ii;
    *codec = 0;
    *format = 0;
    for (ii = 0; ii < s->num_video; ii++) {
        if (s->video[ii].coding_type > 4) {
            if (*codec < 1) {
                *codec = 1;
            }
        }
        if (s->video[ii].coding_type == BD_STREAM_TYPE_VIDEO_HEVC) {
            *codec = 2;
        }
        if (s->video[ii].format == BD_VIDEO_FORMAT_1080I || s->video[ii].format == BD_VIDEO_FORMAT_1080P) {
            if (*format < 1) {
                *format = 1;
            }
        }
        if (s->video[ii].format == BD_VIDEO_FORMAT_2160P) {
            *format = 2;
        }
    }
}

static void _audio_props(const MPLS_STN *s, int *hd_audio)
{
    unsigned ii;
    *hd_audio = 0;
    for (ii = 0; ii < s->num_audio; ii++) {
        if (s->audio[ii].format == BD_STREAM_TYPE_AUDIO_LPCM || s->audio[ii].format >= BD_STREAM_TYPE_AUDIO_TRUHD) {
            *hd_audio = 1;
        }
    }
}

static int _cmp_video_props(const MPLS_PL *p1, const MPLS_PL *p2)
{
    const MPLS_STN *s1 = &p1->play_item[0].stn;
    const MPLS_STN *s2 = &p2->play_item[0].stn;
    int format1, format2, codec1, codec2;

    _video_props(s1, &format1, &codec1);
    _video_props(s2, &format2, &codec2);

    /* prefer UHD over FHD over HD/SD */
    if (format1 != format2)
        return format2 - format1;

    /* prefer H.265 over H.264/VC1 over MPEG1/2 */
    return codec2 - codec1;
}

static int _cmp_audio_props(const MPLS_PL *p1, const MPLS_PL *p2)
{
    const MPLS_STN *s1 = &p1->play_item[0].stn;
    const MPLS_STN *s2 = &p2->play_item[0].stn;
    int hda1, hda2;

    _audio_props(s1, &hda1);
    _audio_props(s2, &hda2);

    /* prefer HD audio formats */
    return hda2 - hda1;
}

static int _pl_guess_main_title(const MPLS_PL *p1, const MPLS_PL *p2,
                                const char *mpls_id1, const char *mpls_id2,
                                const char *known_mpls_ids)
{
    uint32_t d1 = _pl_duration(p1);
    uint32_t d2 = _pl_duration(p2);

    /* if both longer than 30 min */
    if (d1 > 30*60*45000 && d2 > 30*60*45000) {

        /* prefer many chapters over no chapters */
        int chap1 = _pl_chapter_count(p1);
        int chap2 = _pl_chapter_count(p2);
        int chap_diff = chap2 - chap1;
        if ((chap1 < 2 || chap2 < 2) && (chap_diff < -5 || chap_diff > 5)) {
            /* chapter count differs by more than 5 */
            BD_DEBUG(DBG_MAIN_PL, "main title (%s,%s): chapter count difference %d\n",
                     mpls_id1, mpls_id2, chap_diff);
            return chap_diff;
        }

        /* Check video: prefer HD over SD, H.264/VC1 over MPEG1/2 */
        int vid_diff = _cmp_video_props(p1, p2);
        if (vid_diff) {
            BD_DEBUG(DBG_MAIN_PL, "main title (%s,%s): video properties difference %d\n",
                     mpls_id1, mpls_id2, vid_diff);
            return vid_diff;
        }

        /* compare audio: prefer HD audio */
        int aud_diff = _cmp_audio_props(p1, p2);
        if (aud_diff) {
            BD_DEBUG(DBG_MAIN_PL, "main title (%s,%s): audio properties difference %d\n",
                     mpls_id1, mpls_id2, aud_diff);
            return aud_diff;
        }

        /* prefer "known good" playlists */
        if (known_mpls_ids) {
            int known1 = !!str_strcasestr(known_mpls_ids, mpls_id1);
            int known2 = !!str_strcasestr(known_mpls_ids, mpls_id2);
            int known_diff = known2 - known1;
            if (known_diff) {
                BD_DEBUG(DBG_MAIN_PL, "main title (%s,%s): prefer \"known\" playlist %s\n",
                         mpls_id1, mpls_id2, known_diff < 0 ? mpls_id1 : mpls_id2);
                return known_diff;
            }
        }
    }

    /* compare playlist duration, select longer playlist */
    if (d1 < d2) {
        return 1;
    }
    if (d1 > d2) {
        return -1;
    }

    /* prefer playlist with higher number of tracks */
    int sc1 = _pl_streams_score(p1);
    int sc2 = _pl_streams_score(p2);
    return sc2 - sc1;
}

/*
 * title list
 */

NAV_TITLE_LIST* nav_get_title_list(BD_DISC *disc, uint32_t flags, uint32_t min_title_length)
{
    BD_DIR_H *dir;
    BD_DIRENT ent;
    MPLS_PL **pl_list = NULL;
    MPLS_PL *pl = NULL;
    unsigned int ii, pl_list_size = 0;
    int res;
    NAV_TITLE_LIST *title_list = NULL;
    unsigned int title_info_alloc = 100;
    char *known_mpls_ids;

    dir = disc_open_dir(disc, "BDMV" DIR_SEP "PLAYLIST");
    if (dir == NULL) {
        return NULL;
    }

    title_list = calloc(1, sizeof(NAV_TITLE_LIST));
    if (!title_list) {
        dir_close(dir);
        return NULL;
    }
    title_list->title_info = calloc(title_info_alloc, sizeof(NAV_TITLE_INFO));
    if (!title_list->title_info) {
        X_FREE(title_list);
        dir_close(dir);
        return NULL;
    }

    known_mpls_ids = disc_property_get(disc, DISC_PROPERTY_MAIN_FEATURE);
    if (!known_mpls_ids) {
        known_mpls_ids = disc_property_get(disc, DISC_PROPERTY_PLAYLISTS);
    }

    ii = 0;
    for (res = dir_read(dir, &ent); !res; res = dir_read(dir, &ent)) {

        if (ent.d_name[0] == '.') {
            continue;
        }
        if (ii >= pl_list_size) {
            MPLS_PL **tmp = NULL;

            pl_list_size += 100;
            tmp = realloc(pl_list, pl_list_size * sizeof(MPLS_PL*));
            if (tmp == NULL) {
                break;
            }
            pl_list = tmp;
        }
        pl = mpls_get(disc, ent.d_name);
        if (pl != NULL) {
            if ((flags & TITLES_FILTER_DUP_TITLE) &&
                !_filter_dup(pl_list, ii, pl)) {
                mpls_free(&pl);
                continue;
            }
            if ((flags & TITLES_FILTER_DUP_CLIP) && !_filter_repeats(pl, 2)) {
                mpls_free(&pl);
                continue;
            }
            if (min_title_length > 0 &&
                _pl_duration(pl) < min_title_length*45000) {
                mpls_free(&pl);
                continue;
            }
            if (ii >= title_info_alloc) {
                NAV_TITLE_INFO *tmp = NULL;
                title_info_alloc += 100;

                tmp = realloc(title_list->title_info,
                              title_info_alloc * sizeof(NAV_TITLE_INFO));
                if (tmp == NULL) {
                    break;
                }
                title_list->title_info = tmp;
            }
            pl_list[ii] = pl;

            /* main title guessing */
            if (_filter_dup(pl_list, ii, pl) &&
                _filter_repeats(pl, 2)) {

                if (_pl_guess_main_title(pl_list[ii], pl_list[title_list->main_title_idx],
                                         ent.d_name,
                                         title_list->title_info[title_list->main_title_idx].name,
                                         known_mpls_ids) <= 0) {
                    title_list->main_title_idx = ii;
                }
            }

            memcpy(title_list->title_info[ii].name, ent.d_name, 10);
            title_list->title_info[ii].name[10] = '\0';
            title_list->title_info[ii].ref = ii;
            title_list->title_info[ii].mpls_id  = atoi(ent.d_name);
            title_list->title_info[ii].duration = _pl_duration(pl_list[ii]);
            ii++;
        }
    }
    dir_close(dir);

    title_list->count = ii;
    for (ii = 0; ii < title_list->count; ii++) {
        mpls_free(&pl_list[ii]);
    }
    X_FREE(known_mpls_ids);
    X_FREE(pl_list);
    return title_list;
}

void nav_free_title_list(NAV_TITLE_LIST **title_list)
{
    if (*title_list) {
        X_FREE((*title_list)->title_info);
        X_FREE((*title_list));
    }
}

/*
 *
 */

uint8_t nav_clip_lookup_aspect(const NAV_CLIP *clip, int pid)
{
    const CLPI_PROG *progs;
    int ii, jj;

    if (clip->cl == NULL) {
        return 0;
    }

    progs = clip->cl->program.progs;
    for (ii = 0; ii < clip->cl->program.num_prog; ii++) {
        const CLPI_PROG_STREAM *ps = progs[ii].streams;
        for (jj = 0; jj < progs[ii].num_streams; jj++) {
            if (ps[jj].pid == pid)
            {
                return ps[jj].aspect;
            }
        }
    }
    return 0;
}

static void
_fill_mark(const NAV_TITLE *title, NAV_MARK *mark, int entry)
{
    const MPLS_PL *pl = title->pl;
    const MPLS_PLM *plm;
    const MPLS_PI *pi;
    const NAV_CLIP *clip;

    plm = &pl->play_mark[entry];

    mark->mark_type = plm->mark_type;
    mark->clip_ref = plm->play_item_ref;
    clip = &title->clip_list.clip[mark->clip_ref];
    if (clip->cl != NULL && mark->clip_ref < title->pl->list_count) {
        mark->clip_pkt = clpi_lookup_spn(clip->cl, plm->time, 1,
            title->pl->play_item[mark->clip_ref].clip[title->angle].stc_id);
    } else {
        mark->clip_pkt = clip->start_pkt;
    }
    mark->title_pkt = clip->title_pkt + mark->clip_pkt - clip->start_pkt;
    mark->clip_time = plm->time;

    // Calculate start of mark relative to beginning of playlist
    if (plm->play_item_ref < title->clip_list.count) {
        clip = &title->clip_list.clip[plm->play_item_ref];
        pi = &pl->play_item[plm->play_item_ref];
        mark->title_time = clip->title_time + plm->time - pi->in_time;
    }
}

static void
_extrapolate_title(NAV_TITLE *title)
{
    uint32_t duration = 0;
    uint32_t pkt = 0;
    unsigned ii, jj;
    const MPLS_PL *pl = title->pl;
    const MPLS_PI *pi;
    const MPLS_PLM *plm;
    NAV_MARK *mark, *prev = NULL;
    NAV_CLIP *clip;

    for (ii = 0; ii < title->clip_list.count; ii++) {
        clip = &title->clip_list.clip[ii];
        pi = &pl->play_item[ii];
        if (pi->angle_count > title->angle_count) {
            title->angle_count = pi->angle_count;
        }

        clip->title_time = duration;
        clip->duration = pi->out_time - pi->in_time;
        clip->title_pkt = pkt;
        duration += clip->duration;
        pkt += clip->end_pkt - clip->start_pkt;
    }
    title->duration = duration;
    title->packets = pkt;

    for (ii = 0, jj = 0; ii < pl->mark_count; ii++) {
        plm = &pl->play_mark[ii];
        if (plm->mark_type == BD_MARK_ENTRY) {

            mark = &title->chap_list.mark[jj];
            _fill_mark(title, mark, ii);
            mark->number = jj;

            // Calculate duration of "entry" marks (chapters)
            if (plm->duration != 0) {
                mark->duration = plm->duration;
            } else if (prev != NULL) {
                if (prev->duration == 0) {
                    prev->duration = mark->title_time - prev->title_time;
                }
            }
            prev = mark;
            jj++;
        }
        mark = &title->mark_list.mark[ii];
        _fill_mark(title, mark, ii);
        mark->number = ii;
    }
    title->chap_list.count = jj;
    if (prev != NULL && prev->duration == 0) {
        prev->duration = title->duration - prev->title_time;
    }
}

static void _fill_clip(NAV_TITLE *title,
                       const MPLS_CLIP *mpls_clip,
                       uint8_t connection_condition, uint32_t in_time, uint32_t out_time,
                       unsigned pi_angle_count, unsigned still_mode, unsigned still_time,
                       NAV_CLIP *clip,
                       unsigned ref, uint32_t *pos, uint32_t *time)

{
    char *file;

    clip->title = title;
    clip->ref   = ref;
    clip->still_mode = still_mode;
    clip->still_time = still_time;

    if (title->angle >= pi_angle_count) {
        clip->angle = 0;
    } else {
        clip->angle = title->angle;
    }

    memcpy(clip->name, mpls_clip[clip->angle].clip_id, 5);
    if (!memcmp(mpls_clip[clip->angle].codec_id, "FMTS", 4))
        memcpy(&clip->name[5], ".fmts", 6);
    else
        memcpy(&clip->name[5], ".m2ts", 6);
    clip->clip_id = atoi(mpls_clip[clip->angle].clip_id);

    clpi_unref(&clip->cl);

    file = str_printf("%s.clpi", mpls_clip[clip->angle].clip_id);
    if (file) {
        clip->cl = clpi_get(title->disc, file);
        X_FREE(file);
    }
    if (clip->cl == NULL) {
        clip->start_pkt = 0;
        clip->end_pkt = 0;
        return;
    }

    switch (connection_condition) {
        case 5:
        case 6:
            clip->start_pkt = 0;
            clip->connection = CONNECT_SEAMLESS;
            break;
        default:
            if (ref) {
                clip->start_pkt = clpi_lookup_spn(clip->cl, in_time, 1,
                                              mpls_clip[clip->angle].stc_id);
            } else {
                clip->start_pkt = 0;
            }
            clip->connection = CONNECT_NON_SEAMLESS;
            break;
    }
    clip->end_pkt = clpi_lookup_spn(clip->cl, out_time, 0,
                                    mpls_clip[clip->angle].stc_id);
    clip->in_time = in_time;
    clip->out_time = out_time;
    clip->title_pkt = *pos;
    *pos += clip->end_pkt - clip->start_pkt;
    clip->title_time = *time;
    *time += clip->out_time - clip->in_time;

    clip->stc_spn = clpi_find_stc_spn(clip->cl, mpls_clip[clip->angle].stc_id);
}

static
void _nav_title_close(NAV_TITLE *title)
{
    unsigned ii, ss;

    if (title->sub_path) {
        for (ss = 0; ss < title->sub_path_count; ss++) {
            if (title->sub_path[ss].clip_list.clip) {
                for (ii = 0; ii < title->sub_path[ss].clip_list.count; ii++) {
                    clpi_unref(&title->sub_path[ss].clip_list.clip[ii].cl);
                }
                X_FREE(title->sub_path[ss].clip_list.clip);
            }
        }
        X_FREE(title->sub_path);
    }

    if (title->clip_list.clip) {
        for (ii = 0; ii < title->clip_list.count; ii++) {
            clpi_unref(&title->clip_list.clip[ii].cl);
        }
        X_FREE(title->clip_list.clip);
    }

    mpls_free(&title->pl);
    X_FREE(title->chap_list.mark);
    X_FREE(title->mark_list.mark);
    X_FREE(title);
}

void nav_title_close(NAV_TITLE **title)
{
    if (*title) {
        _nav_title_close(*title);
        *title = NULL;
    }
}

NAV_TITLE* nav_title_open(BD_DISC *disc, const char *playlist, unsigned angle)
{
    NAV_TITLE *title = NULL;
    unsigned ii, ss;
    uint32_t pos = 0;
    uint32_t time = 0;

    title = calloc(1, sizeof(NAV_TITLE));
    if (title == NULL) {
        return NULL;
    }
    title->disc = disc;
    strncpy(title->name, playlist, 11);
    title->name[10] = '\0';
    title->angle_count = 0;
    title->angle = angle;
    title->pl = mpls_get(disc, playlist);
    if (title->pl == NULL) {
        BD_DEBUG(DBG_NAV, "Fail: Playlist parse %s\n", playlist);
        X_FREE(title);
        return NULL;
    }

    // Find length in packets and end_pkt for each clip
    if (title->pl->list_count) {
        title->clip_list.count = title->pl->list_count;
        title->clip_list.clip = calloc(title->pl->list_count, sizeof(NAV_CLIP));
        if (!title->clip_list.clip) {
            _nav_title_close(title);
            return NULL;
        }
        title->packets = 0;
        for (ii = 0; ii < title->pl->list_count; ii++) {
            const MPLS_PI *pi;
            NAV_CLIP *clip;

            pi = &title->pl->play_item[ii];

            clip = &title->clip_list.clip[ii];

            _fill_clip(title, pi->clip, pi->connection_condition, pi->in_time, pi->out_time, pi->angle_count,
                       pi->still_mode, pi->still_time, clip, ii, &pos, &time);
        }
    }

    // sub paths
    // Find length in packets and end_pkt for each clip
    if (title->pl->sub_count > 0) {
        title->sub_path_count = title->pl->sub_count;
        title->sub_path       = calloc(title->sub_path_count, sizeof(NAV_SUB_PATH));
        if (!title->sub_path) {
          _nav_title_close(title);
          return NULL;
        }

        for (ss = 0; ss < title->sub_path_count; ss++) {
            NAV_SUB_PATH *sub_path = &title->sub_path[ss];

            sub_path->type            = title->pl->sub_path[ss].type;
            sub_path->clip_list.count = title->pl->sub_path[ss].sub_playitem_count;
            if (!sub_path->clip_list.count)
              continue;

            sub_path->clip_list.clip  = calloc(sub_path->clip_list.count, sizeof(NAV_CLIP));
            if (!sub_path->clip_list.clip) {
              _nav_title_close(title);
              return NULL;
            }

            pos = time = 0;
            for (ii = 0; ii < sub_path->clip_list.count; ii++) {
                const MPLS_SUB_PI *pi   = &title->pl->sub_path[ss].sub_play_item[ii];
                NAV_CLIP    *clip = &sub_path->clip_list.clip[ii];

                _fill_clip(title, pi->clip, pi->connection_condition, pi->in_time, pi->out_time, 0,
                           0, 0, clip, ii, &pos, &time);
            }
        }
    }

    title->chap_list.count = _pl_chapter_count(title->pl);
    if (title->chap_list.count) {
        title->chap_list.mark = calloc(title->chap_list.count, sizeof(NAV_MARK));
    }
    title->mark_list.count = title->pl->mark_count;
    if (title->mark_list.count) {
        title->mark_list.mark = calloc(title->pl->mark_count, sizeof(NAV_MARK));
    }

    _extrapolate_title(title);

    if (title->angle >= title->angle_count) {
        title->angle = 0;
    }

    return title;
}

// Search for random access point closest to the requested packet
// Packets are 192 byte TS packets
const NAV_CLIP* nav_chapter_search(const NAV_TITLE *title, unsigned chapter,
                                   uint32_t *clip_pkt, uint32_t *out_pkt)
{
    const NAV_CLIP *clip;

    if (chapter > title->chap_list.count) {
        clip = &title->clip_list.clip[0];
        *clip_pkt = clip->start_pkt;
        *out_pkt = clip->title_pkt;
        return clip;
    }
    clip = &title->clip_list.clip[title->chap_list.mark[chapter].clip_ref];
    *clip_pkt = title->chap_list.mark[chapter].clip_pkt;
    *out_pkt = clip->title_pkt + *clip_pkt - clip->start_pkt;
    return clip;
}

uint32_t nav_chapter_get_current(const NAV_TITLE * title, uint32_t title_pkt)
{
    const NAV_MARK * mark;
    uint32_t ii;

    if (title == NULL) {
        return 0;
    }
    for (ii = 0; ii < title->chap_list.count; ii++) {
        mark = &title->chap_list.mark[ii];
        if (mark->title_pkt <= title_pkt) {
            if ( ii == title->chap_list.count - 1 ) {
                return ii;
            }
            mark = &title->chap_list.mark[ii+1];
            if (mark->title_pkt > title_pkt) {
                return ii;
            }
        }
    }
    return 0;
}

// Search for random access point closest to the requested packet
// Packets are 192 byte TS packets
const NAV_CLIP* nav_mark_search(const NAV_TITLE *title, unsigned mark,
                                uint32_t *clip_pkt, uint32_t *out_pkt)
{
    const NAV_CLIP *clip;

    if (mark > title->mark_list.count) {
        clip = &title->clip_list.clip[0];
        *clip_pkt = clip->start_pkt;
        *out_pkt = clip->title_pkt;
        return clip;
    }
    clip = &title->clip_list.clip[title->mark_list.mark[mark].clip_ref];
    *clip_pkt = title->mark_list.mark[mark].clip_pkt;
    *out_pkt = clip->title_pkt + *clip_pkt - clip->start_pkt;
    return clip;
}

void nav_clip_packet_search(const NAV_CLIP *clip, uint32_t pkt,
                            uint32_t *clip_pkt, uint32_t *clip_time)
{
    *clip_time = clip->in_time;
    if (clip->cl != NULL) {
        *clip_pkt = clpi_access_point(clip->cl, pkt, 0, 0, clip_time);
        if (*clip_pkt < clip->start_pkt) {
            *clip_pkt = clip->start_pkt;
        }
        if (*clip_time && *clip_time < clip->in_time) {
            /* EP map does not store lowest 8 bits of timestamp */
            *clip_time = clip->in_time;
        }

    } else {
        *clip_pkt = clip->start_pkt;
    }
}

// Search for random access point closest to the requested packet
// Packets are 192 byte TS packets
// pkt is relative to the beginning of the title
// out_pkt and out_time is relative to the the clip which the packet falls in
const NAV_CLIP* nav_packet_search(const NAV_TITLE *title, uint32_t pkt,
                                  uint32_t *clip_pkt, uint32_t *out_pkt, uint32_t *out_time)
{
    const NAV_CLIP *clip;
    uint32_t pos, len;
    unsigned ii;

    *out_time = 0;
    pos = 0;
    for (ii = 0; ii < title->pl->list_count; ii++) {
        clip = &title->clip_list.clip[ii];
        len = clip->end_pkt - clip->start_pkt;
        if (pkt < pos + len)
            break;
        pos += len;
    }
    if (ii == title->pl->list_count) {
        clip = &title->clip_list.clip[ii-1];
        *out_time = clip->duration + clip->in_time;
        *clip_pkt = clip->end_pkt;
    } else {
        clip = &title->clip_list.clip[ii];
        nav_clip_packet_search(clip, pkt - pos + clip->start_pkt, clip_pkt, out_time);
    }
    if(*out_time < clip->in_time)
        *out_time = 0;
    else
        *out_time -= clip->in_time;
    *out_pkt = clip->title_pkt + *clip_pkt - clip->start_pkt;
    return clip;
}

// Search for the nearest random access point after the given pkt
// which is an angle change point.
// Packets are 192 byte TS packets
// pkt is relative to the clip
// time is the clip relative time where the angle change point occurs
// returns a packet number
//
// To perform a seamless angle change, perform the following sequence:
// 1. Find the next angle change point with nav_angle_change_search.
// 2. Read and process packets until the angle change point is reached.
//    This may mean progressing to the next play item if the angle change
//    point is at the end of the current play item.
// 3. Change angles with nav_set_angle. Changing angles means changing
//    m2ts files. The new clip information is returned from nav_set_angle.
// 4. Close the current m2ts file and open the new one returned 
//    from nav_set_angle.
// 4. If the angle change point was within the time period of the current
//    play item (i.e. the angle change point is not at the end of the clip),
//    Search to the timestamp obtained from nav_angle_change_search using
//    nav_clip_time_search. Otherwise start at the start_pkt defined 
//    by the clip.
uint32_t nav_clip_angle_change_search(const NAV_CLIP *clip, uint32_t pkt, uint32_t *time)
{
    if (clip->cl == NULL) {
        return pkt;
    }
    return clpi_access_point(clip->cl, pkt, 1, 1, time);
}

// Search for random access point closest to the requested time
// Time is in 45khz ticks
const NAV_CLIP* nav_time_search(const NAV_TITLE *title, uint32_t tick,
                                uint32_t *clip_pkt, uint32_t *out_pkt)
{
    uint32_t pos, len;
    const MPLS_PI *pi = NULL;
    NAV_CLIP *clip;
    unsigned ii;

    if (!title->pl) {
        BD_DEBUG(DBG_NAV | DBG_CRIT, "Time search failed (title not opened)\n");
        return NULL;
    }
    if (title->pl->list_count < 1) {
        BD_DEBUG(DBG_NAV | DBG_CRIT, "Time search failed (empty playlist)\n");
        return NULL;
    }

    pos = 0;
    for (ii = 0; ii < title->pl->list_count; ii++) {
        pi = &title->pl->play_item[ii];
        len = pi->out_time - pi->in_time;
        if (tick < pos + len)
            break;
        pos += len;
    }
    if (ii == title->pl->list_count) {
        clip = &title->clip_list.clip[ii-1];
        *clip_pkt = clip->end_pkt;
    } else {
        clip = &title->clip_list.clip[ii];
        nav_clip_time_search(clip, tick - pos + pi->in_time, clip_pkt, out_pkt);
    }
    *out_pkt = clip->title_pkt + *clip_pkt - clip->start_pkt;
    return clip;
}

// Search for random access point closest to the requested time
// Time is in 45khz ticks, between clip in_time and out_time.
void nav_clip_time_search(const NAV_CLIP *clip, uint32_t tick, uint32_t *clip_pkt, uint32_t *out_pkt)
{
    if (tick >= clip->out_time) {
        *clip_pkt = clip->end_pkt;
    } else {
        if (clip->cl != NULL) {
            *clip_pkt = clpi_lookup_spn(clip->cl, tick, 1,
               clip->title->pl->play_item[clip->ref].clip[clip->angle].stc_id);
            if (*clip_pkt < clip->start_pkt) {
                *clip_pkt = clip->start_pkt;
            }

        } else {
            *clip_pkt = clip->start_pkt;
        }
    }
    if (out_pkt) {
        *out_pkt = clip->title_pkt + *clip_pkt - clip->start_pkt;
    }
}

/*
 * Input Parameters:
 * title     - title struct obtained from nav_title_open
 *
 * Return value:
 * Pointer to NAV_CLIP struct
 * NULL - End of clip list
 */
const NAV_CLIP* nav_next_clip(const NAV_TITLE *title, const NAV_CLIP *clip)
{
    if (clip == NULL) {
        return &title->clip_list.clip[0];
    }
    if (clip->ref >= title->clip_list.count - 1) {
        return NULL;
    }
    return &title->clip_list.clip[clip->ref + 1];
}

void nav_set_angle(NAV_TITLE *title, unsigned angle)
{
    int ii;
    uint32_t pos = 0;
    uint32_t time = 0;

    if (title == NULL) {
        return;
    }
    if (angle > 8) {
        // invalid angle
        return;
    }
    if (angle == title->angle) {
        // no change
        return;
    }

    title->angle = angle;
    // Find length in packets and end_pkt for each clip
    title->packets = 0;
    for (ii = 0; ii < title->pl->list_count; ii++) {
        const MPLS_PI *pi;
        NAV_CLIP *cl;

        pi = &title->pl->play_item[ii];
        cl = &title->clip_list.clip[ii];

        _fill_clip(title, pi->clip, pi->connection_condition, pi->in_time, pi->out_time, pi->angle_count,
                   pi->still_mode, pi->still_time, cl, ii, &pos, &time);
    }
    _extrapolate_title(title);
}

char *nav_clip_textst_font(const NAV_CLIP *clip, int index)
{
    char *file;

    if (index < 0 || index >= clip->cl->clip.font_info.font_count)
        return NULL;

    file = str_printf("%s.otf", clip->cl->clip.font_info.font[index].file_id);
    return file;
}
