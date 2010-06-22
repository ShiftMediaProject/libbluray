/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  Obliter0n
 * Copyright (C) 2009-2010  John Stebbins
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
 * the linked library.  You must obey the GNU Lesser General Public
 * License in all respects for all of the code used other than "OpenSSL".
 * If you modify this file, you may extend this exception to your
 * version of the file, but you are not obligated to do so.  If you do
 * not wish to do so, delete this exception statement from your version.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "bluray.h"
#include "register.h"
#include "util/macro.h"
#include "util/logging.h"
#include "util/strutl.h"
#include "bdnav/navigation.h"
#include "bdnav/index_parse.h"
#include "file/file.h"
#ifdef USING_DLOPEN
#include "file/dl.h"
#endif

#ifndef USING_DLOPEN
#include <libaacs/aacs.h>
#include <libbdplus/bdplus.h>
#endif
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

static int _open_m2ts(BLURAY *bd)
{
    char *f_name;

    f_name = str_printf("%s" DIR_SEP "BDMV" DIR_SEP "STREAM" DIR_SEP "%s",
                        bd->device_path, bd->clip->name);

    bd->clip_pos = (uint64_t)bd->clip->start_pkt * 192;
    bd->clip_block_pos = (bd->clip_pos / 6144) * 6144;

    if (bd->fp != NULL) {
        file_close(bd->fp);
    }
    if ((bd->fp = file_open(f_name, "rb"))) {
        file_seek(bd->fp, 0, SEEK_END);
        if ((bd->clip_size = file_tell(bd->fp))) {
            file_seek(bd->fp, bd->clip_block_pos, SEEK_SET);
            bd->int_buf_off = 6144;
            X_FREE(f_name);

            if (bd->h_libbdplus && bd->bdplus) {
#ifdef USING_DLOPEN
                fptr_p_void bdplus_set_title;
                bdplus_set_title = dl_dlsym(bd->h_libbdplus, "bdplus_set_title");
                if (bdplus_set_title)
                    bdplus_set_title(bd->bdplus, bd->clip->clip_id);
#else
                bdplus_set_title(bd->bdplus, bd->clip->clip_id);
#endif
            }
            return 1;
        }

        DEBUG(DBG_BLURAY, "Clip %s empty! (%p)\n", f_name, bd);
    }

    DEBUG(DBG_BLURAY | DBG_CRIT, "Unable to open clip %s! (%p)\n",
          f_name, bd);

    X_FREE(f_name);
    return 0;
}

static int _libaacs_open(BLURAY *bd, const char *keyfile_path)
{
#ifdef USING_DLOPEN
#ifdef __APPLE__
    if ((bd->h_libaacs = dl_dlopen("libaacs.dylib"))) {
#else
    if ((bd->h_libaacs = dl_dlopen("libaacs.so.0"))) {
#endif
        DEBUG(DBG_BLURAY, "Downloaded libaacs (%p)\n", bd->h_libaacs);

        fptr_p_void fptr = dl_dlsym(bd->h_libaacs, "aacs_open");
        bd->libaacs_decrypt_unit = dl_dlsym(bd->h_libaacs, "aacs_decrypt_unit");

        if (fptr && bd->libaacs_decrypt_unit) {
            if ((bd->aacs = fptr(bd->device_path, keyfile_path))) {
                DEBUG(DBG_BLURAY, "Opened libaacs (%p)\n", bd->aacs);
                return 1;
            }
            DEBUG(DBG_BLURAY, "aacs_open() failed!\n");
        } else {
            DEBUG(DBG_BLURAY, "libaacs dlsym failed!\n");
        }
        dl_dlclose(bd->h_libaacs);
    } else {
        DEBUG(DBG_BLURAY, "libaacs not found!\n");
    }
#else
    bd->h_libaacs = NULL;
    DEBUG(DBG_BLURAY, "Using libaacs via normal linking\n");

    bd->libaacs_decrypt_unit = &aacs_decrypt_unit;

    if (bd->libaacs_decrypt_unit) {
        if ((bd->aacs = aacs_open(bd->device_path, keyfile_path))) {
            DEBUG(DBG_BLURAY, "Opened libaacs (%p)\n", bd->aacs);
            return 1;
        }
        DEBUG(DBG_BLURAY, "aacs_open() failed!\n");
    }
#endif

    bd->h_libaacs = NULL;
    bd->libaacs_decrypt_unit = NULL;

    return 0;
}

static void _libbdplus_open(BLURAY *bd, const char *keyfile_path)
{
    // Take a quick stab to see if we want/need bdplus
    // we should fix this, and add various string functions.
    uint8_t vid[16] = {
        0xC5,0x43,0xEF,0x2A,0x15,0x0E,0x50,0xC4,0xE2,0xCA,
        0x71,0x65,0xB1,0x7C,0xA7,0xCB}; // FIXME
    FILE_H *fd;
    char *tmp = NULL;
    tmp = str_printf("%s/BDSVM/00000.svm", bd->device_path);
    if ((fd = file_open(tmp, "rb"))) {
        file_close(fd);

        DEBUG(DBG_BDPLUS, "attempting to load libbdplus\n");
#ifdef USING_DLOPEN
#ifdef __APPLE__
        if ((bd->h_libbdplus = dl_dlopen("libbdplus.dylib"))) {
#else
        if ((bd->h_libbdplus = dl_dlopen("libbdplus.so.0"))) {
#endif
            DEBUG(DBG_BLURAY, "Downloaded libbdplus (%p)\n",
                  bd->h_libbdplus);

            fptr_p_void bdplus_init = dl_dlsym(bd->h_libbdplus, "bdplus_init");
            //bdplus_t *bdplus_init(path,configfile_path,*vid );
            if (bdplus_init)
                bd->bdplus = bdplus_init(bd->device_path, keyfile_path, vid);

            // Since we will call these functions a lot, we assign them
            // now.
            bd->bdplus_seek  = dl_dlsym(bd->h_libbdplus, "bdplus_seek");
            bd->bdplus_fixup = dl_dlsym(bd->h_libbdplus, "bdplus_fixup");

        }
#else
        bd->h_libbdplus = NULL;
        DEBUG(DBG_BLURAY,"Using libbdplus via normal linking\n");

        bd->bdplus = bdplus_init(bd->device_path, keyfile_path, vid);

        // Since we will call these functions a lot, we assign them
        // now.
        bd->bdplus_seek  = &bdplus_seek;
        bd->bdplus_fixup = &bdplus_fixup;
#endif
    } // file_open
    X_FREE(tmp);
}

int _index_open(BLURAY *bd)
{
    char *file;

    file = str_printf("%s/BDMV/index.bdmv", bd->device_path);
    bd->index = indx_parse(file);
    X_FREE(file);
    if (!bd->index) {
        return 0;
    }

    return 1;
}

BLURAY *bd_open(const char* device_path, const char* keyfile_path)
{
    BLURAY *bd = calloc(1, sizeof(BLURAY));

    if (device_path) {

        bd->device_path = (char*)malloc(strlen(device_path) + 1);
        strcpy(bd->device_path, device_path);

        bd->aacs = NULL;
        bd->h_libaacs = NULL;
        bd->fp = NULL;

        _libaacs_open(bd, keyfile_path);

        _libbdplus_open(bd, keyfile_path);

        _index_open(bd);

        bd->regs = bd_registers_init();

        DEBUG(DBG_BLURAY, "BLURAY initialized! (%p)\n", bd);
    } else {
        X_FREE(bd);

        DEBUG(DBG_BLURAY | DBG_CRIT, "No device path provided!\n");
    }

    return bd;
}

void bd_close(BLURAY *bd)
{
    if (bd->h_libaacs && bd->aacs) {
#ifdef USING_DLOPEN
        fptr_p_void fptr = dl_dlsym(bd->h_libaacs, "aacs_close");
        fptr(bd->aacs);  // FIXME: NULL
        dl_dlclose(bd->h_libaacs);
#else
        aacs_close(bd->aacs);  // FIXME: NULL
#endif
    }

    if (bd->h_libbdplus && bd->bdplus) {
#ifdef USING_DLOPEN
        fptr_p_void bdplus_free = dl_dlsym(bd->h_libbdplus, "bdplus_free");
        if (bdplus_free) bdplus_free(bd->bdplus);
#else
        bdplus_free(bd->bdplus);
#endif
        bd->bdplus = NULL;

#ifdef USING_DLOPEN
        dl_dlclose(bd->h_libbdplus);
#endif
        bd->h_libbdplus = NULL;

        bd->bdplus_seek  = NULL;
        bd->bdplus_fixup = NULL;
    }

    if (bd->fp) {
        file_close(bd->fp);
    }

    if (bd->title_list != NULL) {
        nav_free_title_list(bd->title_list);
    }
    if (bd->title != NULL) {
        nav_title_close(bd->title);
    }
    if (bd->index != NULL) {
        indx_free(bd->index);
    }

    bd_registers_free(bd->regs);

    X_FREE(bd->device_path);

    DEBUG(DBG_BLURAY, "BLURAY destroyed! (%p)\n", bd);

    X_FREE(bd);
}

int _read_block(BLURAY *bd)
{
    const int len = 6144;

    if (bd->fp) {
        DEBUG(DBG_BLURAY, "Reading unit [%d bytes] at %"PRIu64"... (%p)\n",
              len, bd->clip_block_pos, bd);

        if (len + bd->clip_block_pos <= bd->clip_size) {
            int read_len;

            if ((read_len = file_read(bd->fp, bd->int_buf, len))) {
                if (read_len != len)
                    DEBUG(DBG_BLURAY | DBG_CRIT, "Read %d bytes at %"PRIu64" ; requested %d ! (%p)\n", read_len, bd->clip_block_pos, len, bd);

                if (bd->libaacs_decrypt_unit) {
                    if (!bd->libaacs_decrypt_unit(bd->aacs, bd->int_buf, len, bd->clip_block_pos)) {
                        DEBUG(DBG_BLURAY, "Unable decrypt unit! (%p)\n", bd);

                        return 0;
                    } // decrypt
                } // aacs

                bd->clip_block_pos += len;

                // bdplus fixup, if required.
                if (bd->bdplus_fixup && bd->bdplus) {
                    int32_t numFixes;
                    numFixes = bd->bdplus_fixup(bd->bdplus, len, bd->int_buf);
#if 1
                    if (numFixes) {
                        DEBUG(DBG_BDPLUS,
                              "BDPLUS did %u fixups\n", numFixes);
                    }
#endif

                }

                DEBUG(DBG_BLURAY, "Read unit OK! (%p)\n", bd);

                return 1;
            }

            DEBUG(DBG_BLURAY | DBG_CRIT, "Read %d bytes at %"PRIu64" failed ! (%p)\n", len, bd->clip_block_pos, bd);

            return 0;
        }

        DEBUG(DBG_BLURAY | DBG_CRIT, "Read past EOF ! (%p)\n", bd);

        return 0;
    }

    DEBUG(DBG_BLURAY, "No valid title selected! (%p)\n", bd);

    return 0;
}

static int64_t _seek_internal(BLURAY *bd, NAV_CLIP *clip, uint32_t title_pkt, uint32_t clip_pkt)
{
    if (!clip)
        return -1;

    if (clip->ref != bd->clip->ref) {
        // The position is in a new clip
        bd->clip = clip;
        if (!_open_m2ts(bd)) {
            return -1;
        }
    }
    bd->s_pos = (uint64_t)title_pkt * 192;
    bd->clip_pos = (uint64_t)clip_pkt * 192;
    bd->clip_block_pos = (bd->clip_pos / 6144) * 6144;

    file_seek(bd->fp, bd->clip_block_pos, SEEK_SET);

    bd->int_buf_off = 6144;

    DEBUG(DBG_BLURAY, "Seek to %"PRIu64" (%p)\n",
          bd->s_pos, bd);
    if (bd->bdplus_seek && bd->bdplus)
        bd->bdplus_seek(bd->bdplus, bd->clip_block_pos);

    return bd->s_pos;
}

int64_t bd_seek_time(BLURAY *bd, uint64_t tick)
{
    uint32_t clip_pkt, out_pkt;
    NAV_CLIP *clip;

    if (bd->seamless_angle_change) {
        bd->clip = nav_set_angle(bd->title, bd->clip, bd->request_angle);
        bd->angle = bd->request_angle;
        bd->seamless_angle_change = 0;
    }
    if (tick < bd->title->duration) {
        tick /= 2;

        // Find the closest access unit to the requested position
        clip = nav_time_search(bd->title, tick, &clip_pkt, &out_pkt);

        return _seek_internal(bd, clip, out_pkt, clip_pkt);
    }

    return bd->s_pos;
}

int64_t bd_seek_chapter(BLURAY *bd, int chapter)
{
    uint32_t clip_pkt, out_pkt;
    NAV_CLIP *clip;

    if (chapter < bd->title->chap_list.count) {

        // Find the closest access unit to the requested position
        clip = nav_chapter_search(bd->title, chapter, &clip_pkt, &out_pkt);

        return _seek_internal(bd, clip, out_pkt, clip_pkt);
    }

    return bd->s_pos;
}

int64_t bd_chapter_pos(BLURAY *bd, int chapter)
{
    uint32_t clip_pkt, out_pkt;
    NAV_CLIP *clip;

    // Find the closest access unit to the requested position
    clip = nav_chapter_search(bd->title, chapter, &clip_pkt, &out_pkt);
    return (int64_t)out_pkt * 192;
}

int64_t bd_seek_mark(BLURAY *bd, int mark)
{
    uint32_t clip_pkt, out_pkt;
    NAV_CLIP *clip;

    if (mark < bd->title->mark_list.count) {

        // Find the closest access unit to the requested position
        clip = nav_mark_search(bd->title, mark, &clip_pkt, &out_pkt);

        return _seek_internal(bd, clip, out_pkt, clip_pkt);
    }

    return bd->s_pos;
}

int64_t bd_seek(BLURAY *bd, uint64_t pos)
{
    uint32_t pkt, clip_pkt, out_pkt, out_time;
    NAV_CLIP *clip;

    if (bd->seamless_angle_change) {
        bd->clip = nav_set_angle(bd->title, bd->clip, bd->request_angle);
        bd->angle = bd->request_angle;
        bd->seamless_angle_change = 0;
    }
    if (pos < bd->s_size) {
        pkt = pos / 192;

        // Find the closest access unit to the requested position
        clip = nav_packet_search(bd->title, pkt, &clip_pkt, &out_pkt, &out_time);

        return _seek_internal(bd, clip, out_pkt, clip_pkt);
    }

    return bd->s_pos;
}

static int64_t _clip_seek_time(BLURAY *bd, uint64_t tick)
{
    uint32_t clip_pkt, out_pkt;

    if (tick < bd->clip->out_time) {

        // Find the closest access unit to the requested position
        nav_clip_time_search(bd->clip, tick, &clip_pkt, &out_pkt);

        return _seek_internal(bd, bd->clip, out_pkt, clip_pkt);
    }

    return bd->s_pos;
}

int bd_read(BLURAY *bd, unsigned char *buf, int len)
{
    int out_len;

    if (bd->fp) {
        out_len = 0;
        DEBUG(DBG_BLURAY, "Reading [%d bytes] at %"PRIu64"... (%p)\n", len, bd->s_pos, bd);

        while (len > 0) {
            uint32_t clip_pkt;

            unsigned int size = len;
            // Do we need to read more data?
            clip_pkt = bd->clip_pos / 192;
            if (bd->seamless_angle_change) {
                if (clip_pkt >= bd->angle_change_pkt) {
                    if (clip_pkt >= bd->clip->end_pkt) {
                        bd->clip = nav_next_clip(bd->title, bd->clip);
                        if (!_open_m2ts(bd)) {
                            return -1;
                        }
                        bd->s_pos = bd->clip->pos;
                    } else {
                        bd->clip = nav_set_angle(bd->title, bd->clip, bd->request_angle);
                        _clip_seek_time(bd, bd->angle_change_time);
                    }
                    bd->angle = bd->request_angle;
                    bd->seamless_angle_change = 0;
                } else {
                    uint64_t angle_pos;

                    angle_pos = bd->angle_change_pkt * 192;
                    if (angle_pos - bd->clip_pos < size)
                    {
                        size = angle_pos - bd->clip_pos;
                    }
                }
            }
            if (bd->int_buf_off == 6144 || clip_pkt >= bd->clip->end_pkt) {

                // Do we need to get the next clip?
                if (bd->clip == NULL) {
                    // We previously reached the last clip.  Nothing
                    // else to read.
                    return -1;
                }
                if (clip_pkt >= bd->clip->end_pkt) {
                    bd->clip = nav_next_clip(bd->title, bd->clip);
                    if (bd->clip == NULL) {
                        DEBUG(DBG_BLURAY, "End of title (%p)\n", bd);
                        return out_len;
                    }
                    if (!_open_m2ts(bd)) {
                        return -1;
                    }
                }
                if (_read_block(bd)) {

                    bd->int_buf_off = bd->clip_pos % 6144;

                } else {
                    return out_len;
                }
            }
            if (size > (unsigned int)6144 - bd->int_buf_off) {
                size = 6144 - bd->int_buf_off;
            }
            memcpy(buf, bd->int_buf + bd->int_buf_off, size);
            buf += size;
            len -= size;
            out_len += size;
            bd->clip_pos += size;
            bd->int_buf_off += size;
            bd->s_pos += size;
        }

        DEBUG(DBG_BLURAY, "%d bytes read OK! (%p)\n", out_len, bd);

        return out_len;
    }

    DEBUG(DBG_BLURAY, "No valid title selected! (%p)\n", bd);

    return -1;
}

static int _open_playlist(BLURAY *bd, const char *f_name)
{
    if (bd->title) {
        nav_title_close(bd->title);
    }

    bd->title = nav_title_open(bd->device_path, f_name);
    if (bd->title == NULL) {
        DEBUG(DBG_BLURAY | DBG_CRIT, "Unable to open title %s! (%p)\n",
              f_name, bd);
        return 0;
    }

    bd->angle = 0;
    bd->seamless_angle_change = 0;
    bd->s_pos = 0;
    bd->s_size = (uint64_t)bd->title->packets * 192;

    // Get the initial clip of the playlist
    bd->clip = nav_next_clip(bd->title, NULL);
    if (_open_m2ts(bd)) {
        DEBUG(DBG_BLURAY, "Title %s selected! (%p)\n", f_name, bd);
        return 1;
    }
    return 0;
}

int bd_select_playlist(BLURAY *bd, uint32_t playlist)
{
    char *f_name = str_printf("%05d.mpls", playlist);
    int result;

    result = _open_playlist(bd, f_name);

    X_FREE(f_name);
    return result;
}

// Select a title for playback
// The title index is an index into the list
// established by bd_get_titles()
int bd_select_title(BLURAY *bd, uint32_t title_idx)
{
    const char *f_name;

    // Open the playlist
    if (bd->title_list == NULL) {
        DEBUG(DBG_BLURAY, "Title list not yet read! (%p)\n", bd);
        return 0;
    }
    if (bd->title_list->count <= title_idx) {
        DEBUG(DBG_BLURAY, "Invalid title index %d! (%p)\n", title_idx, bd);
        return 0;
    }

    f_name = bd->title_list->title_info[title_idx].name;

    return _open_playlist(bd, f_name);
}

int bd_select_angle(BLURAY *bd, int angle)
{
    if (bd->title == NULL) {
        DEBUG(DBG_BLURAY, "Title not yet selected! (%p)\n", bd);
        return 0;
    }
    bd->clip = nav_set_angle(bd->title, bd->clip, angle);
    return 1;
}

void bd_seamless_angle_change(BLURAY *bd, int angle)
{
    uint32_t clip_pkt;

    clip_pkt = (bd->clip_pos + 191) / 192;
    bd->angle_change_pkt = nav_angle_change_search(bd->clip, clip_pkt,
                                                   &bd->angle_change_time);
    bd->request_angle = angle;
    bd->seamless_angle_change = 1;
}

uint64_t bd_get_title_size(BLURAY *bd)
{
    return bd ? bd->s_size : UINT64_C(0);
}

uint64_t bd_tell(BLURAY *bd)
{
    return bd ? bd->s_pos : INT64_C(0);
}

// This must be called after bd_open() and before bd_select_title().
// Populates the title list in BLURAY.
// Filtering of the returned list is controled throught flags
//  TITLES_ALL              - all titles
//  TITLES_FILTER_DUP_TITLE - remove duplicate titles
//  TITLES_FILTER_DUP_CLIP  - remove titles that have duplicated clips
//  TITLES_RELEVANT         - remove dup titles and clips
//
//  Returns the number of titles found
uint32_t bd_get_titles(BLURAY *bd, uint8_t flags)
{
    if (!bd) {
        DEBUG(DBG_BLURAY | DBG_CRIT, "bd_get_titles(NULL) failed (%p)\n", bd);
        return 0;
    }

    if (bd->title_list != NULL) {
        nav_free_title_list(bd->title_list);
    }
    bd->title_list = nav_get_title_list(bd->device_path, flags);

    if (!bd->title_list) {
        DEBUG(DBG_BLURAY | DBG_CRIT, "nav_get_title_list(%s) failed (%p)\n", bd->device_path, bd);
        return 0;
    }

    return bd->title_list->count;
}

BD_TITLE_INFO* bd_get_title_info(BLURAY *bd, uint32_t title_idx)
{
    NAV_TITLE *title;
    BD_TITLE_INFO *title_info;
    unsigned int ii;

    if (bd->title_list == NULL) {
        DEBUG(DBG_BLURAY, "Title list not yet read! (%p)\n", bd);
        return NULL;
    }
    if (bd->title_list->count <= title_idx) {
        DEBUG(DBG_BLURAY, "Invalid title index %d! (%p)\n", title_idx, bd);
        return NULL;
    }
    title = nav_title_open(bd->device_path, bd->title_list->title_info[title_idx].name);
    if (title == NULL) {
        DEBUG(DBG_BLURAY | DBG_CRIT, "Unable to open title %s! (%p)\n",
              bd->title_list->title_info[title_idx].name, bd);
        return NULL;
    }
    title_info = calloc(1, sizeof(BD_TITLE_INFO));
    title_info->idx = title_idx;
    title_info->duration = (uint64_t)title->duration * 2;
    title_info->angle_count = title->angle_count;
    title_info->chapter_count = title->chap_list.count;
    title_info->chapters = calloc(title_info->chapter_count, sizeof(BD_TITLE_CHAPTER));
    for (ii = 0; ii < title_info->chapter_count; ii++) {
        title_info->chapters[ii].idx = ii;
        title_info->chapters[ii].start = (uint64_t)title->chap_list.mark[ii].title_time * 2;
        title_info->chapters[ii].duration = (uint64_t)title->chap_list.mark[ii].duration * 2;
        title_info->chapters[ii].offset = (uint64_t)title->chap_list.mark[ii].title_pkt * 192;
    }
    nav_title_close(title);
    return title_info;
}

void bd_free_title_info(BD_TITLE_INFO *title_info)
{
    X_FREE(title_info->chapters);
    X_FREE(title_info);
}

/*
 * player settings
 */

int bd_set_player_setting(BLURAY *bd, uint32_t idx, uint32_t value)
{
    static const struct { uint32_t idx; uint32_t  psr; } map[] = {
        { BD_PLAYER_SETTING_PARENTAL,       PSR_PARENTAL },
        { BD_PLAYER_SETTING_AUDIO_CAP,      PSR_AUDIO_CAP },
        { BD_PLAYER_SETTING_AUDIO_LANG,     PSR_AUDIO_LANG },
        { BD_PLAYER_SETTING_PG_LANG,        PSR_PG_AND_SUB_LANG },
        { BD_PLAYER_SETTING_MENU_LANG,      PSR_MENU_LANG },
        { BD_PLAYER_SETTING_COUNTRY_CODE,   PSR_COUNTRY },
        { BD_PLAYER_SETTING_REGION_CODE,    PSR_REGION },
        { BD_PLAYER_SETTING_VIDEO_CAP,      PSR_VIDEO_CAP },
        { BD_PLAYER_SETTING_TEXT_CAP,       PSR_TEXT_CAP },
        { BD_PLAYER_SETTING_PLAYER_PROFILE, PSR_PROFILE_VERSION },
    };

    unsigned i;

    if (idx == BD_PLAYER_SETTING_PLAYER_PROFILE) {
        value = ((value & 0xf) << 16) | 0x0200;  /* version fixed to BD-RO Part 3, version 2.0 */
    }

    for (i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
        if (idx == map[i].idx) {
            return bd_psr_setting_write(bd->regs, idx, value);
        }
    }

    return 0;
}

static uint32_t _string_to_uint(const char *s, int n)
{
    uint32_t val = 0;

    if (n > 4)
        n = 4;

    while (n--)
        val = (val << 8) | s[n];

    return val;
}

int bd_set_player_setting_str(BLURAY *bd, uint32_t idx, const char *s)
{
    switch (idx) {
        case BD_PLAYER_SETTING_AUDIO_LANG:
        case BD_PLAYER_SETTING_PG_LANG:
        case BD_PLAYER_SETTING_MENU_LANG:
            return bd_set_player_setting(bd, idx, s ? _string_to_uint(s, 3) : 0xffffff);

        case BD_PLAYER_SETTING_COUNTRY_CODE:
            return bd_set_player_setting(bd, idx, s ? _string_to_uint(s, 3) : 0xffff  );

        default:
            return 0;
    }
}

