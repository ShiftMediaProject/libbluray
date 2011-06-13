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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "libbluray/bluray.h"
#include "util/macro.h"
#include "util/strutl.h"

#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <inttypes.h>

#define PKT_SIZE 192
#define BUF_SIZE (PKT_SIZE * 1024)
#define MIN(a,b) (((a) < (b)) ? a : b)

static void
_usage(char *cmd)
{
    fprintf(stderr,
"Usage: %s -t playlist [-c first[-last]] [-k keyfile] [-a angle]  <bd path> [dest]\n"
"Summary:\n"
"    Given a playlist number and Blu-Ray directory tree,\n"
"    find the clips that compose the movie and splice\n"
"    them together in the destination file\n"
"Options:\n"
"    t N         - Index of title to splice. First title is 1.\n"
"    a N         - Angle. First angle is 1.\n"
"    c N or N-M  - Chapter or chapter range. First chapter is 1.\n"
"    k keyfile   - AACS keyfile path.\n"
"    <bd path>   - Path to root of Blu-Ray directory tree.\n"
"    [dest]      - Destination of spliced clips. stdout if not specified.\n"
, cmd);

    exit(EXIT_FAILURE);
}

#define OPTS "c:vt:k:a:"

int
main(int argc, char *argv[])
{
    int title_no = -1;
    int angle = 0;
    char *bdpath = NULL, *dest = NULL;
    FILE *out;
    int opt;
    int verbose = 0;
    int64_t total = 0;
    int64_t pos, end_pos = -1;
    size_t size, wrote;
    int bytes;
    int title_count;
    BLURAY *bd;
    int chapter_start = 0;
    int chapter_end = -1;
    uint8_t buf[BUF_SIZE];
    char *keyfile = NULL;
    BLURAY_TITLE_INFO *ti;

    do {
        opt = getopt(argc, argv, OPTS);
        switch (opt) {
            case -1:
                if (optind < argc && bdpath == NULL) {
                    bdpath = argv[optind];
                    optind++;
                    opt = 1;
                }
                else if (optind < argc && dest == NULL) {
                    dest = argv[optind];
                    optind++;
                    opt = 1;
                }
                break;

            case 'c': {
                int match;
                match = sscanf(optarg, "%d-%d", &chapter_start, &chapter_end);
                if (match == 1) {
                    chapter_end = chapter_start + 1;
                }
                chapter_start--;
                chapter_end--;
            } break;

            case 'k':
                keyfile = optarg;
                break;

            case 'a':
                angle = atoi(optarg);
                angle--;
                break;

            case 't':
                title_no = atoi(optarg);
                title_no--;
                break;

            case 'v':
                verbose = 1;
                break;

            default:
                _usage(argv[0]);
                break;
        }
    } while (opt != -1);

    if (title_no < 0) {
        _usage(argv[0]);
    }
    if (optind < argc) {
        _usage(argv[0]);
    }

    bd = bd_open(bdpath, keyfile);
    if (bd == NULL) {
        fprintf(stderr, "Failed to open disc: %s\n", bdpath);
        return 1;
    }

    title_count = bd_get_titles(bd, TITLES_RELEVANT, 0);
    if (title_count <= 0) {
        fprintf(stderr, "No titles found: %s\n", bdpath);
        return 1;
    }

    if (!bd_select_title(bd, title_no)) {
        fprintf(stderr, "Failed to open title: %d\n", title_no);
        return 1;
    }

    if (dest) {
        out = fopen(dest, "wb");
        if (out == NULL) {
            fprintf(stderr, "Failed to open destination: %s\n", dest);
            return 1;
        }
    } else {
        out = stdout;
    }

    ti = bd_get_title_info(bd, title_no, angle);

    if (angle >= (int)ti->angle_count) {
        fprintf(stderr, "Invalid angle %d > angle count %d. Using angle 1.\n", 
                angle+1, ti->angle_count);
        angle = 0;
    }
    bd_select_angle(bd, angle);

    if (chapter_start >= (int)ti->chapter_count) {
        fprintf(stderr, "First chapter %d > chapter count %d\n", 
                chapter_start+1, ti->chapter_count);
        return 1;
    }
    if (chapter_end >= (int)ti->chapter_count) {
        chapter_end = -1;
    }
    if (chapter_end >= 0) {
        end_pos = bd_chapter_pos(bd, chapter_end);
    }
    bd_free_title_info(ti);

    bd_seek_chapter(bd, chapter_start);
    pos = bd_tell(bd);
    while (end_pos < 0 || pos < end_pos) {
        size = BUF_SIZE;
        if (size > (size_t)(end_pos - pos)) {
            size = end_pos - pos;
        }
        bytes = bd_read(bd, buf, size);
        if (bytes <= 0) {
            break;
        }
        pos = bd_tell(bd);
        wrote = fwrite(buf, 1, bytes, out);
        if (wrote != (size_t)bytes) {
            fprintf(stderr, "read/write sizes do not match: %d/%zu\n", bytes, wrote);
        }
        if (wrote == 0) {
            if (ferror(out)) {
                perror("Write error");
            }
            break;
        }
        total += PKT_SIZE * wrote;
    }
    if (verbose) {
        fprintf(stderr, "Wrote %"PRId64" bytes\n", total);
    }
    bd_close(bd);
    fclose(out);
    return 0;
}

