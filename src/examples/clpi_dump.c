/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  John Stebbins
 * Copyright (C) 2012-2013  Petri Hintukainen <phintuka@users.sourceforge.net>
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
#include <unistd.h>
#include <inttypes.h>

#include "libbluray/bdnav/clpi_data.h"
#include "libbluray/bluray.h"

#include "util.h"

static int verbose;

typedef struct {
    int value;
    const char *str;
} VALUE_MAP;

static inline const char*
_lookup_str(const VALUE_MAP *map, int val)
{
    int ii;

    for (ii = 0; map[ii].str; ii++) {
        if (val == map[ii].value) {
            return map[ii].str;
        }
    }
    return "?";
}

const VALUE_MAP codec_map[] = {
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
    {0x20, "H.264 MVC dep."},
    {0x90, "Presentation Graphics"},
    {0x91, "Presentation Graphics"},
    {0x92, "Interactive Graphics"},
    {0, NULL}
};

const VALUE_MAP video_format_map[] = {
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

const VALUE_MAP video_rate_map[] = {
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

const VALUE_MAP video_aspect_map[] = {
    {0, "Reserved1"},
    {1, "Reserved2"},
    {2, "4:3"},
    {3, "16:9"},
    {0, NULL}
};

const VALUE_MAP audio_format_map[] = {
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

const VALUE_MAP audio_rate_map[] = {
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

const VALUE_MAP application_type_map[] = {
    {1, "Main TS for a main-path of Movie"},
    {2, "Main TS for a main-path of Time based slide show"},
    {3, "Main TS for a main-path of Browsable slide show"},
    {4, "Sub TS for a sub-path of Browsable slide show"},
    {5, "Sub TS for a sub-path of Interactive Graphics menu"},
    {6, "Sub TS for a sub-path of Text subtitle"},
    {7, "Sub TS for a sub-path of one or more elementary streams path"},
    {0, NULL},
};

static void
_show_stream(CLPI_PROG_STREAM *ss, int level)
{
    indent_printf(level, "Codec (%04x): %s", ss->coding_type,
                    _lookup_str(codec_map, ss->coding_type));
    indent_printf(level, "PID: %04x", ss->pid);
    switch (ss->coding_type) {
        case 0x01:
        case 0x02:
        case 0xea:
        case 0x1b:
        case 0x20:
            indent_printf(level, "Format %02x: %s", ss->format,
                        _lookup_str(video_format_map, ss->format));
            indent_printf(level, "Rate %02x: %s", ss->rate,
                        _lookup_str(video_rate_map, ss->rate));
            indent_printf(level, "Aspect %02x: %s", ss->aspect,
                        _lookup_str(video_aspect_map, ss->aspect));
            indent_printf(level, "oc_flag %02x", ss->oc_flag);
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
            indent_printf(level, "Rate %02x: %s", ss->rate,
                        _lookup_str(audio_rate_map, ss->rate));
            indent_printf(level, "Language: %s", ss->lang);
            break;

        case 0x90:
        case 0x91:
        case 0xa0:
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
_show_clip_info(CLPI_CL *cl, int level)
{
    CLPI_CLIP_INFO *ci = &cl->clip;
    int ii;

    indent_printf(level, "Clip Info");
    indent_printf(level+1, "Clip Stream Type: %02x", ci->clip_stream_type);
    indent_printf(level+1, "Clip Application Type (%02x): %s",
                  ci->application_type, _lookup_str(application_type_map, ci->application_type));
    indent_printf(level+1, "is_ATC_delta: %s", ci->is_atc_delta ? "True" : "False");
    indent_printf(level+1, "ATC delta count: %d", ci->atc_delta_count);
    indent_printf(level+1, "TS Recording Rate: %u", ci->ts_recording_rate);
    indent_printf(level+1, "Number Source Packets: %u", ci->num_source_packets);
    // Show ts type info
    indent_printf(level+1, "TS Type Info");
    indent_printf(level+2, "Validity Flags %02x", ci->ts_type_info.validity);
    indent_printf(level+2, "Format Id %s", ci->ts_type_info.format_id);
    // Show cc5 thing
    for (ii = 0; ii < ci->atc_delta_count; ii++) {
        indent_printf(level+1, "ATC delta[ %d ]", ii);
        indent_printf(level+2, "Delta %08x", ci->atc_delta[ii].delta);
        indent_printf(level+2, "File Id %s", ci->atc_delta[ii].file_id);
        indent_printf(level+2, "File Code %s", ci->atc_delta[ii].file_code);
    }
    // show fonts
    if (cl->font_info.font_count) {
        indent_printf(level+1, "Font files");
        for (ii = 0; ii < cl->font_info.font_count; ii++) {
            indent_printf(level+2, "Font file %d: %s.otf", ii+1, cl->font_info.font[ii].file_id);
        }
    }

    printf("\n");
}

static void
_show_seq_info(CLPI_SEQ_INFO *si, int level)
{
    CLPI_ATC_SEQ *atc;
    CLPI_STC_SEQ *stc;
    int ii, jj;

    indent_printf(level, "Sequence Info");
    indent_printf(level+1, "Number ATC Sequences: %d", si->num_atc_seq);
    for (ii = 0; ii < si->num_atc_seq; ii++) {
        atc = &si->atc_seq[ii];
        indent_printf(level+1, "ATC Sequence %d", ii);
        indent_printf(level+2, "SPN ATC Start: %u", atc->spn_atc_start);
        indent_printf(level+2, "Offset STC Id: %d", atc->offset_stc_id);
        indent_printf(level+2, "Number STC Sequences: %d", atc->num_stc_seq);
        for (jj = 0; jj < atc->num_stc_seq; jj++) {
            stc = &atc->stc_seq[jj];
            indent_printf(level+2, "ATC Sequence %d", jj);
            indent_printf(level+3, "SPN STC Start: %u", stc->spn_stc_start);
            indent_printf(level+3, "PCR PID: %04x", stc->pcr_pid);
            indent_printf(level+3, "Presentation Start: %u", 
                            stc->presentation_start_time);
            indent_printf(level+3, "Presentation End: %u", 
                            stc->presentation_end_time);
        }
    }
}

static void
_show_prog_info(CLPI_PROG_INFO *pi, int level)
{
    CLPI_PROG *prog;
    int ii, jj;

    indent_printf(level, "Program Info");
    indent_printf(level+1, "Number Programs: %d", pi->num_prog);
    for (ii = 0; ii < pi->num_prog; ii++) {
        prog = &pi->progs[ii];
        indent_printf(level+1, "Program %d", ii);
        indent_printf(level+2, "SPN Program Sequence Start: %d", 
                        prog->spn_program_sequence_start);
        indent_printf(level+2, "Program Map PID: %d", prog->program_map_pid);
        indent_printf(level+2, "Number Streams: %d", prog->num_streams);
        indent_printf(level+2, "Number Groups: %d", prog->num_groups);
        for (jj = 0; jj < prog->num_streams; jj++) {
            indent_printf(level+2, "Stream %d", jj);
            _show_stream(&prog->streams[jj], level+3);
        }
    }
}

static void
_show_extent_start(CLPI_EXTENT_START *es, int level)
{
    unsigned int ii;

    indent_printf(level, "Extension data: Extent Start Point");

    if (!es->num_point) {
        indent_printf(level+1, "(no data)");

    } else {
        indent_printf(level+1, "Number of Start Points: %d", es->num_point);

        if (verbose) {
            for (ii = 0; ii < es->num_point; ii++) {
                indent_printf(level+1, "Extent %5d: SPN 0x%08X", ii, es->point[ii]);
            }
        }
    }
}

static void
_show_cpi_info(CLPI_CPI *cpi, int level)
{
    CLPI_EP_MAP_ENTRY *entry;
    CLPI_EP_COARSE *coarse;
    CLPI_EP_FINE *fine;
    int ii, jj, kk;

    indent_printf(level, "CPI");
    indent_printf(level+1, "Number Stream PID: %d", cpi->num_stream_pid);
    for (ii = 0; ii < cpi->num_stream_pid; ii++) {
        entry = &cpi->entry[ii];
        indent_printf(level+1, "Stream: %d", ii);
        indent_printf(level+2, "PID: %04x", entry->pid);
        indent_printf(level+2, "EP Stream Type: %d", entry->ep_stream_type);
        indent_printf(level+2, "Number EP Coarse: %d", entry->num_ep_coarse);
        indent_printf(level+2, "Number EP Fine: %d", entry->num_ep_fine);
        indent_printf(level+2, "EP Map Start: %d", 
                        entry->ep_map_stream_start_addr);
        for (jj = 0; jj < entry->num_ep_coarse; jj++) {
            coarse = &entry->coarse[jj];
            indent_printf(level+2, "Coarse: %d", jj);
            indent_printf(level+3, "Ref EP Fine: %d", coarse->ref_ep_fine_id);
            indent_printf(level+3, "PTS EP: %d", coarse->pts_ep);
            indent_printf(level+3, "SPN EP: %d", coarse->spn_ep);
        }
        for (jj = 0; jj < entry->num_ep_fine; jj++) {
            fine = &entry->fine[jj];
            indent_printf(level+2, "Fine: %d", jj);
            indent_printf(level+3, "Angle Change Point: %s", 
                fine->is_angle_change_point ? "True":"False");
            indent_printf(level+3, "I End Offset: %d", 
                fine->i_end_position_offset);
            indent_printf(level+3, "PTS EP: %d", fine->pts_ep);
            indent_printf(level+3, "SPN EP: %d", fine->spn_ep);
        }
        if (verbose) {
            uint64_t pts;
            uint32_t spn;

            indent_printf(level+2, "PTS - SPN Map");
            for (jj = 0; jj < entry->num_ep_coarse; jj++) {
                int start, end;

                indent_printf(level+3, "Coarse: %d", jj);
                coarse = &entry->coarse[jj];
                start = coarse->ref_ep_fine_id;
                if (jj < entry->num_ep_coarse - 1) {
                    end = entry->coarse[jj+1].ref_ep_fine_id;
                } else {
                    end = entry->num_ep_fine;
                }
                for (kk = start; kk < end; kk++) {
                    fine = &entry->fine[kk];
                    pts = ((uint64_t) (coarse->pts_ep & ~0x01) << 19) +
                            ((uint64_t)fine->pts_ep << 9);
                    spn = (coarse->spn_ep & ~0x1FFFF) + fine->spn_ep;
                    indent_printf(level+4, "PTS %8"PRIu64"/%8"PRIu64" -- SPN %u",
                        pts, pts >> 1, spn);
                }
            }
        }
    }
}


static void
_usage(char *cmd)
{
    fprintf(stderr, 
"Usage: %s -vcspi <clpi file> [<clpi file> ...]\n"
"With no options, produces no output (not very useful)\n"
"Options:\n"
"    v - Verbose output.\n"
"    c - Shows the Clip Info structure\n"
"    s - Shows the Sequence Info structure\n"
"    p - Shows the Program Info structure\n"
"    i - Shows the CPI. PTS to SPN map\n"
"    e - Shows Extent Start Table\n"
, cmd);

    exit(EXIT_FAILURE);
}

#define OPTS "vcspie"

int
main(int argc, char *argv[])
{
    CLPI_CL *cl;
    int opt;
    int opt_clip_info = 0, opt_seq_info = 0, opt_prog_info = 0;
    int opt_cpi_info = 0, opt_extent_start = 0;
    int ii;

    do {
        opt = getopt(argc, argv, OPTS);
        switch (opt) {
            case -1: break;

            case 'v':
                verbose = 1;
                break;

            case 's':
                opt_seq_info = 1;
                break;

            case 'i':
                opt_cpi_info = 1;
                break;

            case 'c':
                opt_clip_info = 1;
                break;

            case 'p':
                opt_prog_info = 1;
                break;

            case 'e':
                opt_extent_start = 1;
                break;

            default:
                _usage(argv[0]);
                break;
        }
    } while (opt != -1);

    if (optind >= argc) {
        _usage(argv[0]);
    }

    for (ii = optind; ii < argc; ii++) {
        cl = bd_read_clpi(argv[ii]);
        if (cl == NULL) {
            fprintf(stderr, "Parsing %s failed\n", argv[ii]);
            continue;
        }
        if (opt_clip_info) {
            // Show clip info
            _show_clip_info(cl, 1);
        }
        if (opt_seq_info) {
            // Show sequence info
            _show_seq_info(&cl->sequence, 1);
        }
        if (opt_prog_info) {
            // Show program info
            _show_prog_info(&cl->program, 1);
        }
        if (opt_cpi_info) {
            // Show cpi
            _show_cpi_info(&cl->cpi, 1);
        }

        if (opt_prog_info) {
            if (cl->program_ss.num_prog) {
                printf("\n");
                indent_printf(1, "Extension: Program Info SS");
                _show_prog_info(&cl->program_ss, 1);
            }
        }
        if (opt_cpi_info) {
            if (cl->program_ss.num_prog) {
                printf("\n");
                indent_printf(1, "Extension: CPI SS");
                _show_cpi_info(&cl->cpi_ss, 1);
            }
        }
        if (opt_extent_start) {
            // Show extent start point
            if (cl->extent_start.num_point > 0) {
                _show_extent_start(&cl->extent_start, 1);
            }
        }

        bd_free_clpi(cl);
    }
    return 0;
}

