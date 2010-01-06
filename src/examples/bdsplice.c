#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_MALLOC_H
#include <malloc.h>
#endif

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include "../util/macro.h"
#include "../util/strutl.h"
#include "../libbdnav/clpi_parse.h"
#include "../libbdnav/mpls_parse.h"
#include "../libbdnav/navigation.h"

#define PKT_SIZE 192
#define MIN(a,b) (((a) < (b)) ? a : b)

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
    int title_no = -1;
    char *path = NULL, *fname = NULL, *tspath = NULL;
    char *bdpath = NULL, *dest = NULL;
    FILE *out, *tsfile;
    int opt;
    int verbose = 0;
    int64_t size, total = 0;
    NAV_TITLE *title;
    NAV_CLIP *clip;

    do {
        opt = getopt(argc, argv, OPTS);
        switch (opt) {
            case -1:
                break;

            case 't':
                title_no = atoi(optarg);
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
    if (optind >= argc) {
        _usage(argv[0]);
    }
    bdpath = argv[optind++];
    if (optind < argc) {
        dest = argv[optind];
    }

    fname = str_printf("%05d.mpls", title_no);
    X_FREE(path);
    title = nav_title_open(bdpath, fname);
    if (title == NULL) {
        fprintf(stderr, "Failed to open title: %s\n", fname);
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

    tspath = str_printf("%s/BDMV/STREAM", bdpath);
    if (tspath == NULL) {
        X_FREE(path);
        fprintf(stderr, "Failed to find stream path\n");
        return 1;
    }
    for (clip = nav_next_clip(title, NULL); clip; clip = nav_next_clip(title, clip)) {

        fname = str_printf("%s/%s", tspath, clip->name);
        tsfile = fopen(fname, "rb");
        if (tsfile == NULL) {
            fprintf(stderr, "Failed to open m2ts file: %s\n", fname);
            return 1;
        }
        if (verbose) {
            fprintf(stderr, "Splicing: %s\n", basename(fname));
        }

        if (verbose) {
            fprintf(stderr, "Start SPN %u - End SPN %u\n",
                    clip->start_pkt, clip->end_pkt);
        }
        size = _write_packets(out, tsfile, clip->start_pkt, clip->end_pkt);
        total += size;
        fclose(tsfile);
    }
    if (verbose) {
        fprintf(stderr, "Wrote %lld bytes\n", total);
    }
    X_FREE(fname);
    X_FREE(path);
    X_FREE(tspath);
    fclose(out);
    return 0;
}

