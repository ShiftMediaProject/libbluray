/*
 * This file is part of libbluray
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
 */

#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

#include "util/strutl.h"

#include "libbluray/bdnav/mpls_parse.h"
#include "libbluray/bdnav/navigation.h"

#include "util.h"

static int verbose;

typedef struct {
    int value;
    const char *str;
} VALUE_MAP;

VALUE_MAP codec_map[] = {
    {0x01, "MPEG-1 Video"},
    {0x02, "MPEG-2 Video"},
    {0x03, "MPEG-1 Audio"},
    {0x04, "MPEG-2 Audio"},
    {0x80, "LPCM"},
    {0x81, "AC-3"},
    {0x82, "DTS"},
    {0x83, "TrueHD"},
    {0x84, "AC-3 Plus"},
    {0x85, "DTS-HD"},
    {0x86, "DTS-HD Master"},
    {0xa1, "AC-3 Plus for secondary audio"},
    {0xa2, "DTS-HD for secondary audio"},
    {0xea, "VC-1"},
    {0x1b, "H.264"},
    {0x90, "Presentation Graphics"},
    {0x91, "Interactive Graphics"},
    {0x92, "Text Subtitle"},
    {0, NULL}
};

VALUE_MAP video_format_map[] = {
    {0, "Reserved"},
    {1, "480i"},
    {2, "576i"},
    {3, "480p"},
    {4, "1080i"},
    {5, "720p"},
    {6, "1080p"},
    {7, "576p"},
    {0, NULL}
};

VALUE_MAP video_rate_map[] = {
    {0, "Reserved1"},
    {1, "23.976"},
    {2, "24"},
    {3, "25"},
    {4, "29.97"},
    {5, "Reserved2"},
    {6, "50"},
    {7, "59.94"},
    {0, NULL}
};

VALUE_MAP audio_format_map[] = {
    {0, "Reserved1"},
    {1, "Mono"},
    {2, "Reserved2"},
    {3, "Stereo"},
    {4, "Reserved3"},
    {5, "Reserved4"},
    {6, "Multi Channel"},
    {12, "Combo"},
    {0, NULL}
};

VALUE_MAP audio_rate_map[] = {
    {0, "Reserved1"},
    {1, "48 Khz"},
    {2, "Reserved2"},
    {3, "Reserved3"},
    {4, "96 Khz"},
    {5, "192 Khz"},
    {12, "48/192 Khz"},
    {14, "48/96 Khz"},
    {0, NULL}
};

VALUE_MAP subpath_type_map[] = {
  {3, "Interactive Graphics presentation menu"},
  {4, "Text Subtitle"},
  {0,NULL}
};

const char*
_lookup_str(VALUE_MAP *map, int val)
{
    int ii;

    for (ii = 0; map[ii].str; ii++) {
        if (val == map[ii].value) {
            return map[ii].str;
        }
    }
    return "?";
}

static void
_show_stream(MPLS_STREAM *ss, int level)
{
    indent_printf(level, "Codec (%04x): %s", ss->coding_type,
                    _lookup_str(codec_map, ss->coding_type));
    switch (ss->stream_type) {
        case 1:
            indent_printf(level, "PID: %04x", ss->pid);
            break;

        case 2:
        case 4:
            indent_printf(level, "SubPath Id: %02x", ss->subpath_id);
            indent_printf(level, "SubClip Id: %02x", ss->subclip_id);
            indent_printf(level, "PID: %04x", ss->pid);
            break;

        case 3:
            indent_printf(level, "SubPath Id: %02x", ss->subpath_id);
            indent_printf(level, "PID: %04x", ss->pid);
            break;

        default:
            fprintf(stderr, "unrecognized stream type %02x\n", ss->stream_type);
            break;
    };

    switch (ss->coding_type) {
        case 0x01:
        case 0x02:
        case 0xea:
        case 0x1b:
            indent_printf(level, "Format %02x: %s", ss->format,
                        _lookup_str(video_format_map, ss->format));
            indent_printf(level, "Rate %02x: %s", ss->rate,
                        _lookup_str(video_rate_map, ss->rate));
            break;

        case 0x03:
        case 0x04:
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0xa1:
        case 0xa2:
            indent_printf(level, "Format %02x: %s", ss->format,
                        _lookup_str(audio_format_map, ss->format));
            indent_printf(level, "Rate %02x:", ss->rate,
                        _lookup_str(audio_rate_map, ss->rate));
            indent_printf(level, "Language: %s", ss->lang);
            break;

        case 0x90:
        case 0x91:
            indent_printf(level, "Language: %s", ss->lang);
            break;

        case 0x92:
            indent_printf(level, "Char Code: %02x", ss->char_code);
            indent_printf(level, "Language: %s", ss->lang);
            break;

        default:
            fprintf(stderr, "unrecognized coding type %02x\n", ss->coding_type);
            break;
    };
}

static void
_show_details(MPLS_PL *pl, int level)
{
    int ii, jj, kk;

    for (ii = 0; ii < pl->list_count; ii++) {
        MPLS_PI *pi;

        pi = &pl->play_item[ii];
        indent_printf(level, "Clip Id %s", pi->clip[0].clip_id);
        indent_printf(level+1, "Stc Id: %02x", pi->clip[0].stc_id);
        indent_printf(level+1, "Connection Condition: %02x", 
                        pi->connection_condition);
        indent_printf(level+1, "In-Time: %d", pi->in_time);
        indent_printf(level+1, "Out-Time: %d", pi->out_time);
        if (pi->still_mode == 1) {
            indent_printf(level+1, "Still time: %ds\n", pi->still_time);
        }
        if (pi->still_mode == 2) {
            indent_printf(level+1, "Still time: infinite\n");
        }
        if (pi->angle_count > 1) {
            for (jj = 1; jj < pi->angle_count; jj++) {
                indent_printf(level+1, "Angle %d:", jj);
                indent_printf(level+2, "Clip Id %s", pi->clip[jj].clip_id);
                indent_printf(level+2, "Stc Id: %02x", pi->clip[jj].stc_id);
            }
        }
        for (jj = 0; jj < pi->stn.num_video; jj++) {
            indent_printf(level+1, "Video Stream %d:", jj);
            _show_stream(&pi->stn.video[jj], level + 2);
        }
        for (jj = 0; jj < pi->stn.num_audio; jj++) {
            indent_printf(level+1, "Audio Stream %d:", jj);
            _show_stream(&pi->stn.audio[jj], level + 2);
        }
        for (jj = 0; jj < pi->stn.num_ig; jj++) {
            indent_printf(level+1, "Interactive Graphics Stream %d:", jj);
            _show_stream(&pi->stn.ig[jj], level + 2);
        }
        for (jj = 0; jj < (pi->stn.num_pg + pi->stn.num_pip_pg); jj++) {
            if (jj < pi->stn.num_pg) {
               indent_printf(level+1, "Presentation Graphics Stream %d:", jj);
            } else {
                indent_printf(level+1, "PIP Presentation Graphics Stream %d:", jj);
            }
            _show_stream(&pi->stn.pg[jj], level + 2);
        }
        for (jj = 0; jj < pi->stn.num_secondary_video; jj++) {
            indent_printf(level+1, "Secondary Video Stream %d:", jj);
            _show_stream(&pi->stn.secondary_video[jj], level + 2);
            for (kk = 0; kk < pi->stn.secondary_video[jj].sv_num_secondary_audio_ref; kk++) {
                indent_printf(level+2, "Secondary Audio Ref %d: %d", kk,pi->stn.secondary_video[jj].sv_secondary_audio_ref[kk]);
            }
            for (kk = 0; kk < pi->stn.secondary_video[jj].sv_num_pip_pg_ref; kk++) {
                indent_printf(level+2, "PIP Presentation Graphic Ref %d: %d", kk,pi->stn.secondary_video[jj].sv_pip_pg_ref[kk]);
            }
        }
        for (jj = 0; jj < pi->stn.num_secondary_audio; jj++) {
            indent_printf(level+1, "Secondary Audio Stream %d:", jj);
            _show_stream(&pi->stn.secondary_audio[jj], level + 2);
            for (kk = 0; kk < pi->stn.secondary_audio[jj].sa_num_primary_audio_ref; kk++) {
                indent_printf(level+2, "Primary Audio Ref %d: %d", kk,pi->stn.secondary_audio[jj].sa_primary_audio_ref[kk]);
            }
        }
        printf("\n");
    }
}

static void
_show_marks(MPLS_PL *pl, int level)
{
    int ii;

    indent_printf(level, "PlayMark Count %d", pl->mark_count);
    for (ii = 0; ii < pl->mark_count; ii++) {
        MPLS_PI *pi;
        MPLS_PLM *plm;
        int min;
        double sec;

        plm = &pl->play_mark[ii];
        indent_printf(level, "PlayMark %d", ii);
        indent_printf(level+1, "Type: %02x", plm->mark_type);
        if (plm->play_item_ref < pl->list_count) {
            pi = &pl->play_item[plm->play_item_ref];
            indent_printf(level+1, "PlayItem: %s", pi->clip[0].clip_id);
        } else {
            indent_printf(level+1, "PlayItem: Invalid reference");
        }
        indent_printf(level+1, "Time (ticks): %lu", plm->time);
        min = plm->duration / (45000*60);
        sec = (double)(plm->duration - min * 45000 * 60) / 45000;
        indent_printf(level+1, "Duration (mm:ss.ms, ticks): %d:%.2f, %lu", 
                      min, sec, plm->duration);
        printf("\n");
    }
}

static void
_show_clip_list(MPLS_PL *pl, int level)
{
    int ii, jj;

    for (ii = 0; ii < pl->list_count; ii++) {
        MPLS_PI *pi;

        pi = &pl->play_item[ii];
        if (verbose) {
            uint32_t duration;

            duration = pi->out_time - pi->in_time;
            indent_printf(level, "%s.m2ts -- Duration: %3d:%02d", 
                        pi->clip[0].clip_id,
                        duration / (45000 * 60), (duration / 45000) % 60);
        } else {
            indent_printf(level, "%s.m2ts", pi->clip[0].clip_id);
        }
        if (pi->angle_count > 1) {
            for (jj = 1; jj < pi->angle_count; jj++) {
                indent_printf(level+1, "Angle %d: %s.m2ts", jj+1, pi->clip[jj].clip_id);
            }
        }
    }
    printf("\n");
}

static void
_show_sub_paths(MPLS_PL *pl, int level)
{
    int ss, ii;

    for (ss = 0; ss < pl->sub_count; ss++) {
        MPLS_SUB *sub;

        sub = &pl->sub_path[ss];

        indent_printf(level,   "Sub Path %d:", ss);
        indent_printf(level+1, "Type: %d (%s)", sub->type, _lookup_str(subpath_type_map, sub->type));
        indent_printf(level+1, "Repeat: %d", sub->is_repeat);
        indent_printf(level+1, "Sub playitem count: %d", sub->sub_playitem_count);

        for (ii = 0; ii < sub->sub_playitem_count; ii++) {
            MPLS_SUB_PI *pi;

            pi = &sub->sub_play_item[ii];

            if (verbose) {
                indent_printf(level+1, "Sub playitem %d", ii);
                indent_printf(level+2, "Clip Id %s", pi->clip[0].clip_id);
                indent_printf(level+2, "Multi clip: %d", pi->is_multi_clip);
                indent_printf(level+2, "Clip count: %d", pi->clip_count);
                indent_printf(level+2, "Connection Condition: %02x", pi->connection_condition);
                indent_printf(level+2, "In-Time: %d", pi->in_time);
                indent_printf(level+2, "Out-Time: %d", pi->out_time);
                indent_printf(level+2, "Sync playitem Id: %d", pi->sync_play_item_id);
                indent_printf(level+2, "Sync PTS: %d", pi->sync_pts);
            } else {
                indent_printf(level+1, "%s.m2ts", pi->clip[0].clip_id);
            }
        }
    }
}

static uint32_t
_pl_duration(MPLS_PL *pl)
{
    int ii;
    uint32_t duration = 0;
    MPLS_PI *pi;

    for (ii = 0; ii < pl->list_count; ii++) {
        pi = &pl->play_item[ii];
        duration += pi->out_time - pi->in_time;
    }
    return duration;
}

static int
_filter_dup(MPLS_PL *pl_list[], int count, MPLS_PL *pl)
{
    int ii, jj;

    for (ii = 0; ii < count; ii++) {
        if (pl->list_count != pl_list[ii]->list_count ||
            _pl_duration(pl) != _pl_duration(pl_list[ii])) {
            continue;
        }
        for (jj = 0; jj < pl->list_count; jj++) {
            MPLS_PI *pi1, *pi2;

            pi1 = &pl->play_item[jj];
            pi2 = &pl_list[ii]->play_item[jj];

            if (memcmp(pi1->clip[0].clip_id, pi2->clip[0].clip_id, 5) != 0 ||
                pi1->in_time != pi2->in_time ||
                pi1->out_time != pi2->out_time) {
                break;
            }
        }
        if (jj != pl->list_count) {
            continue;
        }
        return 0;
    }
    return 1;
}

static int
_find_repeats(MPLS_PL *pl, const char *m2ts)
{
    int ii, count = 0;

    for (ii = 0; ii < pl->list_count; ii++) {
        MPLS_PI *pi;

        pi = &pl->play_item[ii];
        // Ignore titles with repeated segments
        if (strcmp(pi->clip[0].clip_id, m2ts) == 0) {
            count++;
        }
    }
    return count;
}

static int
_filter_short(MPLS_PL *pl, unsigned int seconds)
{
    // Ignore short playlists
    if (_pl_duration(pl) / 45000 <= seconds) {
        return 0;
    }
    return 1;
}

static int
_filter_repeats(MPLS_PL *pl, int repeats)
{
    int ii;

    for (ii = 0; ii < pl->list_count; ii++) {
        MPLS_PI *pi;

        pi = &pl->play_item[ii];
        // Ignore titles with repeated segments
        if (_find_repeats(pl, pi->clip[0].clip_id) > repeats) {
            return 0;
        }
    }
    return 1;
}

static int clip_list = 0, playlist_info = 0, chapter_marks = 0, sub_paths = 0;
static int repeats = 0, seconds = 0, dups = 0;

static MPLS_PL*
_process_file(char *name, MPLS_PL *pl_list[], int pl_count)
{
    MPLS_PL *pl;

    pl = mpls_parse(name, verbose);
    if (pl == NULL) {
        fprintf(stderr, "Parse failed: %s\n", name);
        return NULL;
    }
    if (seconds) {
        if (!_filter_short(pl, seconds)) {
            mpls_free(pl);
            return NULL;
        }
    }
    if (repeats) {
        if (!_filter_repeats(pl, repeats)) {
            mpls_free(pl);
            return NULL;
        }
    }
    if (dups) {
        if (!_filter_dup(pl_list, pl_count, pl)) {
            mpls_free(pl);
            return NULL;
        }
    }
    if (verbose) {
        indent_printf(0, 
                    "%s -- Num Clips: %3d , Duration: minutes %4lu:%02lu", 
                    basename(name),
                    pl->list_count,
                    _pl_duration(pl) / (45000 * 60),
                    (_pl_duration(pl) / 45000) % 60);
    } else {
        indent_printf(0, "%s -- Duration: minutes %4lu:%02lu", 
                    basename(name),
                    _pl_duration(pl) / (45000 * 60),
                    (_pl_duration(pl) / 45000) % 60);
    }
    if (playlist_info) {
        _show_details(pl, 1);
    }
    if (chapter_marks) {
        _show_marks(pl, 1);
    }
    if (clip_list) {
        _show_clip_list(pl, 1);
    }
    if (sub_paths) {
        _show_sub_paths(pl, 1);
    }
    return pl;
}

static void
_usage(char *cmd)
{
    fprintf(stderr, 
"Usage: %s -vli <mpls file> [<mpls file> ...]\n"
"With no options, produces a list of the playlist(s) with durations\n"
"Options:\n"
"    v             - Verbose output.\n"
"    l             - Produces a list of the m2ts clips\n"
"    i             - Dumps detailed information about each clip\n"
"    c             - Show chapter marks\n"
"    p             - Show sub paths\n"
"    r <N>         - Filter out titles that have >N repeating clips\n"
"    d             - Filter out duplicate titles\n"
"    s <seconds>   - Filter out short titles\n"
"    f             - Filter combination -r2 -d -s900\n"
, cmd);

    exit(EXIT_FAILURE);
}

#define OPTS "vlicpfr:ds:"

static int
_qsort_str_cmp(const void *a, const void *b)
{
    const char *stra = *(char * const *)a;
    const char *strb = *(char * const *)b;

    return strcmp(stra, strb);
}

int
main(int argc, char *argv[])
{
    MPLS_PL *pl;
    int opt;
    int ii, pl_ii;
    MPLS_PL *pl_list[1000];
    struct stat st;
    char *path = NULL;
    DIR *dir = NULL;

    do {
        opt = getopt(argc, argv, OPTS);
        switch (opt) {
            case -1: 
                break;

            case 'v':
                verbose = 1;
                break;

            case 'l':
                clip_list = 1;
                break;

            case 'i':
                playlist_info = 1;
                break;

            case 'c':
                chapter_marks = 1;
                break;

            case 'p':
                sub_paths = 1;
                break;

            case 'd':
                dups = 1;
                break;

            case 'r':
                repeats = atoi(optarg);
                break;

            case 'f':
                repeats = 2;
                dups = 1;
                seconds = 900;
                break;

            case 's':
                seconds = atoi(optarg);
                break;

            default:
                _usage(argv[0]);
                break;
        }
    } while (opt != -1);

    if (optind >= argc) {
        _usage(argv[0]);
    }

    for (pl_ii = 0, ii = optind; pl_ii < 1000 && ii < argc; ii++) {

        if (stat(argv[ii], &st)) {
            continue;
        }
        dir = NULL;
        if (S_ISDIR(st.st_mode)) {

            char *main_title = NULL;

            printf("Directory: %s:\n", argv[ii]);
            path = str_printf("%s/BDMV/PLAYLIST", argv[ii]);
            if (path == NULL) {
                fprintf(stderr, "Failed to find playlist path: %s\n", argv[ii]);
                continue;
            }
            dir = opendir(path);
            if (dir == NULL) {
                fprintf(stderr, "Failed to open dir: %s\n", path);
                free(path);
                continue;
            }
            main_title = nav_find_main_title(argv[ii]);
            if (main_title != NULL) {
                printf("Main Title: %s\n", main_title);
                free(main_title);
            } else {
                fprintf(stderr, "Main title search failed\n");
            }
        }
        if (dir != NULL) {
            char **dirlist = calloc(10001, sizeof(char*));
            struct dirent *ent;
            int jj = 0;
            for (ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
                if (ent->d_name != NULL) {
                    dirlist[jj++] = strdup(ent->d_name);
                }
            }
            qsort(dirlist, jj, sizeof(char*), _qsort_str_cmp);
            for (jj = 0; dirlist[jj] != NULL; jj++) {
                char *name = NULL;
                name = str_printf("%s/%s", path, dirlist[jj]);
                free(dirlist[jj]);
                if (stat(name, &st)) {
                    free(name);
                    continue;
                }
                if (!S_ISREG(st.st_mode)) {
                    free(name);
                    continue;
                }
                pl = _process_file(name, pl_list, pl_ii);
                free(name);
                if (pl != NULL) {
                    pl_list[pl_ii++] = pl;
                }
            } while (ent != NULL);
            free(dirlist);
            free(path);
        } else {
            pl = _process_file(argv[ii], pl_list, pl_ii);
            if (pl != NULL) {
                pl_list[pl_ii++] = pl;
            }
        }
    }
    // Cleanup
    for (ii = 0; ii < pl_ii; ii++) {
        mpls_free(pl_list[ii]);
    }
    return 0;
}

