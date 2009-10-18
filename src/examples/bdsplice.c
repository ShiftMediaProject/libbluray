#include "config.h"
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include "libbdnav/clpi_parse.h"
#include "libbdnav/mpls_parse.h"
#include "util.h"

#define PKT_SIZE 192
#define MIN(a,b) (((a) < (b)) ? a : b)

static void
_make_path(STRING *path, char *root, char *dir)
{
    struct stat st_buf;
    char *base;

    base = basename(root);
    if (strcmp(base, "BDMV") != 0) {
        str_printf(path, "%s/BDMV/%s", root, dir);
    } else {
        str_printf(path, "%s/%s", root, dir);
    }

    if (stat(path->buf, &st_buf) || !S_ISDIR(st_buf.st_mode)) {
        str_free(path);
    }
}

int64_t
_write_packets(FILE *out, FILE *tsfile, uint32_t start_pkt, uint32_t end_pkt)
{
    uint8_t buf[PKT_SIZE * 1024];
    uint32_t ii;
    size_t num, wrote;
    int64_t total = 0;

    fseek(tsfile, start_pkt * PKT_SIZE, SEEK_SET);
    for (ii = start_pkt; ii < end_pkt; ii += num) {

        num = end_pkt - ii;
        num = MIN(1024, num);
        num = fread(buf, PKT_SIZE, num, tsfile);
        if (!num) {
            if (ferror(tsfile)) {
                perror("Read error");
            }
            break;
        }
        wrote = fwrite(buf, PKT_SIZE, num, out);
        if (wrote != num) {
            fprintf(stderr, "read/write sizes do not match\n");
        }
        if (!wrote) {
            if (ferror(out)) {
                perror("Write error");
            }
            break;
        }
        total += PKT_SIZE * wrote;
    }
    return total;
}

static void
_usage(char *cmd)
{
    fprintf(stderr, 
"Usage: %s -t playlist [-c first[-last]] <bd path> [dest]\n"
"Summary:\n"
"    Given a playlist number and Blu-Ray directory tree,\n"
"    find the clips that compose the movie and splice\n"
"    them together in the destination file\n"
"Options:\n"
"    t N         - Index of title to splice\n"
"    c N or N-M  - Chapter or chapter range\n"
"    <bd path>   - Path to root of Blu-Ray directory tree\n"
"    [dest]      - Destination of spliced clips\n"
, cmd);

    exit(EXIT_FAILURE);
}

#define OPTS "c:vt:"

int
main(int argc, char *argv[])
{
    int title = -1;
    int chapter_start = 0;
    int chapter_end = 0;
    uint16_t start_ref;
    uint16_t end_ref;
    uint16_t chapter_start_ref;
    uint16_t chapter_end_ref;
    uint32_t chapter_start_time = 0;
    uint32_t chapter_end_time = 0;
    STRING path = {0,}, fname = {0,}, tspath = {0,};
    char *bdpath = NULL, *dest = NULL;
    FILE *out, *tsfile;
    MPLS_PL *pl;
    CLPI_CL *cl;
    int opt, ii;
    uint32_t start_pkt, end_pkt;
    int verbose = 0;
    int64_t size, total = 0;

    do {
        opt = getopt(argc, argv, OPTS);
        switch (opt) {
            case -1: 
                break;

            case 'c':
            {
                int match;
                match = sscanf(optarg, "%d-%d", &chapter_start, &chapter_end);
                if (match == 1) {
                    chapter_end = chapter_start;
                }
                break;
            }

            case 't':
                title = atoi(optarg);
                break;

            case 'v':
                verbose = 1;
                break;

            default:
                _usage(argv[0]);
                break;
        }
    } while (opt != -1);

    if (title < 0) {
        _usage(argv[0]);
    }
    if (optind >= argc) {
        _usage(argv[0]);
    }
    bdpath = argv[optind++];
    if (optind < argc) {
        dest = argv[optind];
    }
    _make_path(&path, bdpath, "PLAYLIST");
    if (path.buf == NULL) {
        fprintf(stderr, "Failed to find playlist path\n");
        return 1;
    }
    str_printf(&fname, "%s/%05d.mpls", path.buf, title);
    str_free(&path);
    pl = mpls_parse(fname.buf, verbose);
    if (pl == NULL) {
        fprintf(stderr, "Failed to parse playlist: %s\n", fname.buf);
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
    _make_path(&path, bdpath, "CLIPINF");
    if (path.buf == NULL) {
        fprintf(stderr, "Failed to find clip info path\n");
        return 1;
    }
    _make_path(&tspath, bdpath, "STREAM");
    if (tspath.buf == NULL) {
        str_free(&path);
        fprintf(stderr, "Failed to find stream path\n");
        return 1;
    }
    if (chapter_start > pl->mark_count) {
        chapter_start = pl->mark_count;
    }
    if (chapter_end > pl->mark_count) {
        chapter_end = pl->mark_count;
    }
    if (chapter_start) {
        MPLS_PLM *plm;

        plm = &pl->play_mark[chapter_start - 1];
        start_ref = chapter_start_ref = plm->play_item_ref;
        chapter_start_time = plm->time;
        if (chapter_end < pl->mark_count) {
            plm = &pl->play_mark[chapter_end];
            end_ref = chapter_end_ref = plm->play_item_ref;
            chapter_end_time = plm->time;
        } else {
            end_ref = chapter_end_ref = pl->list_count - 1;
            chapter_end_ref = -1;
        }
    } else {
        start_ref = 0;
        chapter_start_ref = -1;
        end_ref = chapter_end_ref = pl->list_count - 1;
        chapter_end_ref = -1;
    }
    for (ii = start_ref; ii <= end_ref; ii++) {
        MPLS_PI *pi;

        pi = &pl->play_item[ii];

        str_printf(&fname, "%s/", path.buf);
        str_append_sub(&fname, pi->clip_id, 0, 5);
        str_append(&fname, ".clpi");

        cl = clpi_parse(fname.buf, verbose);
        if (cl == NULL) {
            fprintf(stderr, "Failed to parse clip info: %s\n", fname.buf);
            return 1;
        }

        str_printf(&fname, "%s/", tspath.buf);
        str_append_sub(&fname, pi->clip_id, 0, 5);
        str_append(&fname, ".m2ts");
        tsfile = fopen(fname.buf, "rb");
        if (tsfile == NULL) {
            fprintf(stderr, "Failed to open m2ts file: %s\n", fname.buf);
            clpi_free(&cl);
            return 1;
        }
        if (verbose) {
            fprintf(stderr, "Splicing: %s\n", basename(fname.buf));
        }

        if (ii == chapter_start_ref && chapter_start != 1) {
            start_pkt = clpi_lookup_spn(&cl->cpi, chapter_start_time, 0);
        } else {
            // When the connection condition is seamless, start at 
            // first source packet of the clip
            if ((pi->connection_condition == 5 ||
                pi->connection_condition == 6)) {

                start_pkt = 0;
            } else {
                start_pkt = clpi_lookup_spn(&cl->cpi, pi->in_time, 1);
                // For some reason, the in_time of a clip always resolves 
                // to a packet that is 1 packet after a PCR which we need
                // in order to handle the discontinuities correctly
                if (start_pkt > 0) {
                    start_pkt--;
                }
            }
        }
        if (ii == chapter_end_ref) {
            end_pkt = clpi_lookup_spn(&cl->cpi, chapter_end_time, 0);
        } else {
            end_pkt = clpi_lookup_spn(&cl->cpi, pi->out_time, 0);
        }
        if (verbose) {
            fprintf(stderr, "Start SPN %u - End SPN %u\n", start_pkt, end_pkt);
        }
        size = _write_packets(out, tsfile, start_pkt, end_pkt);
        total += size;
        if (verbose) {
            fprintf(stderr, "Wrote %ld - Total %ld\n", size, total);
        }
        fclose(tsfile);
        clpi_free(&cl);
    }
    mpls_free(&pl);
    str_free(&fname);
    str_free(&path);
    str_free(&tspath);
    fclose(out);
    return 0;
}

