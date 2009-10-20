#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include "../util/macro.h"
#include "../file/file.h"
#include "../util/bits.h"
#include "mpls_parse.h"

#define MPLS_SIG1  ('M' << 24 | 'P' << 16 | 'L' << 8 | 'S')
#define MPLS_SIG2A ('0' << 24 | '2' << 16 | '0' << 8 | '0')
#define MPLS_SIG2B ('0' << 24 | '1' << 16 | '0' << 8 | '0')

static int mpls_verbose = 0;

static void
_human_readable_sig(char *sig, uint32_t s1, uint32_t s2)
{
    sig[0] = (s1 >> 24) & 0xFF;
    sig[1] = (s1 >> 16) & 0xFF;
    sig[2] = (s1 >>  8) & 0xFF;
    sig[3] = (s1      ) & 0xFF;
    sig[4] = (s2 >> 24) & 0xFF;
    sig[5] = (s2 >> 16) & 0xFF;
    sig[6] = (s2 >>  8) & 0xFF;
    sig[7] = (s2      ) & 0xFF;
    sig[8] = 0;
}

static int
_parse_header(BITSTREAM *bits, MPLS_PL *pl)
{
    pl->type_indicator  = bs_read(bits, 32);
    pl->type_indicator2 = bs_read(bits, 32);
    if (pl->type_indicator != MPLS_SIG1 || 
        (pl->type_indicator2 != MPLS_SIG2A && 
         pl->type_indicator2 != MPLS_SIG2B)) {

        char sig[9];
        char expect[9];

        _human_readable_sig(sig, pl->type_indicator, pl->type_indicator2);
        _human_readable_sig(expect, MPLS_SIG1, MPLS_SIG2A);
        fprintf(stderr, "failed signature match, expected (%s) got (%s)\n", 
                expect, sig);
        return 0;
    }
    pl->list_pos = bs_read(bits, 32);
    pl->mark_pos = bs_read(bits, 32);
    pl->ext_pos  = bs_read(bits, 32);

    return 1;
}

static int
_parse_stream(BITSTREAM *bits, MPLS_STREAM *s)
{
    int len;
    int pos;

    if (!bs_is_align(bits, 0x07)) {
        fprintf(stderr, "_parse_stream: Stream alignment error\n");
    }
    len = bs_read(bits, 8);
    pos = bs_pos(bits) >> 3;

    s->stream_type = bs_read(bits, 8);
    switch (s->stream_type) {
        case 1:
            s->pid = bs_read(bits, 16);
            break;

        case 2:
        case 4:
            s->subpath_id = bs_read(bits, 8);
            s->subclip_id = bs_read(bits, 8);
            s->pid        = bs_read(bits, 16);
            break;

        case 3:
            s->subpath_id = bs_read(bits, 8);
            s->pid        = bs_read(bits, 16);
            break;

        default:
            fprintf(stderr, "unrecognized stream type %02x\n", s->stream_type);
            break;
    };

    bs_seek_byte(bits, pos + len);

    len = bs_read(bits, 8);
    pos = bs_pos(bits) >> 3;

    s->lang[0] = '\0';
    s->coding_type = bs_read(bits, 8);
    switch (s->coding_type) {
        case 0x01:
        case 0x02:
        case 0xea:
        case 0x1b:
            s->format = bs_read(bits, 4);
            s->rate   = bs_read(bits, 4);
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
            s->format = bs_read(bits, 4);
            s->rate   = bs_read(bits, 4);
            bs_read_bytes(bits, s->lang, 3);
            break;

        case 0x90:
        case 0x91:
            bs_read_bytes(bits, s->lang, 3);
            break;

        case 0x92:
            s->char_code = bs_read(bits, 8);
            bs_read_bytes(bits, s->lang, 3);
            break;

        default:
            fprintf(stderr, "unrecognized coding type %02x\n", s->coding_type);
            break;
    };
    s->lang[3] = '\0';

    bs_seek_byte(bits, pos + len);
    return 1;
}

static int
_parse_playitem(BITSTREAM *bits, MPLS_PI *pi)
{
    int len, ii;
    uint8_t is_multi_angle;
    uint8_t codecId[5];
    int pos;
    MPLS_STREAM *ss;

    if (!bs_is_align(bits, 0x07)) {
        fprintf(stderr, "_parse_playitem: Stream alignment error\n");
    }

    // PlayItem Length
    len = bs_read(bits, 16);
    pos = bs_pos(bits) >> 3;

    // Primary Clip identifer
    bs_read_bytes(bits, (uint8_t*)pi->clip_id, 5);
    pi->clip_id[5] = '\0';

    // skip the redundant "M2TS" CodecIdentifier
    bs_read_bytes(bits, codecId, 4);
    if (memcmp(codecId, "M2TS", 4) != 0) {
        codecId[4] = 0;
        fprintf(stderr, "Incorrect CodecIdentifier (%s)\n", codecId);
    }

    // Skip reserved 11 bits
    bs_skip(bits, 11);

    // Top 3 bits of current byte are also reserved
    is_multi_angle = bs_read(bits, 1);

    pi->connection_condition = bs_read(bits, 4);
    if (pi->connection_condition != 0x01 && 
        pi->connection_condition != 0x05 &&
        pi->connection_condition != 0x06) {

        fprintf(stderr, "Unexpected connection condition %02x\n", 
                pi->connection_condition);
    }

    pi->stc_id   = bs_read(bits, 8);
    pi->in_time  = bs_read(bits, 32);
    pi->out_time = bs_read(bits, 32);

    // Skip UO_mask_table, random_access_flag, reserved, still_mode
    // and still_time
    bs_skip(bits, 12 * 8);

    if (is_multi_angle) {

        int num_angles;

        num_angles = bs_read(bits, 8);
        // skip reserved, is_different_audio, is_seamless_angle_change
        bs_skip(bits, 8);
        for (ii = 1; ii < num_angles; ii++) {
            // Drop clip_id, clip_codec_id, stc_id
            bs_skip(bits, 10 * 8);
        }
    }
    // Skip STN len
    bs_skip(bits, 16);
    // Skip 2 reserved bytes
    bs_skip(bits, 16);

    pi->stn.num_video           = bs_read(bits, 8);
    pi->stn.num_audio           = bs_read(bits, 8);
    pi->stn.num_pg              = bs_read(bits, 8);
    pi->stn.num_ig              = bs_read(bits, 8);
    pi->stn.num_secondary_audio = bs_read(bits, 8);
    pi->stn.num_secondary_video = bs_read(bits, 8);
    pi->stn.num_pip_pg          = bs_read(bits, 8);

    // 5 reserve bytes
    bs_skip(bits, 5 * 8);

    ss = NULL;
    if (pi->stn.num_video) {
        ss = malloc(pi->stn.num_video * sizeof(MPLS_STREAM));
    }
    for (ii = 0; ii < pi->stn.num_video; ii++) {
        if (!_parse_stream(bits, &ss[ii])) {
            X_FREE(ss);
            fprintf(stderr, "error parsing video entry\n");
            return 0;
        }
    }
    pi->stn.video = ss;

    ss = NULL;
    if (pi->stn.num_audio)
        ss = malloc(pi->stn.num_audio * sizeof(MPLS_STREAM));
    for (ii = 0; ii < pi->stn.num_audio; ii++) {

        if (!_parse_stream(bits, &ss[ii])) {
            X_FREE(ss);
            fprintf(stderr, "error parsing audio entry\n");
            return 0;
        }
    }
    pi->stn.audio = ss;

    ss = NULL;
    if (pi->stn.num_pg) {
        ss = malloc(pi->stn.num_pg * sizeof(MPLS_STREAM));
    }
    for (ii = 0; ii < pi->stn.num_pg; ii++) {
        if (!_parse_stream(bits, &ss[ii])) {
            X_FREE(ss);
            fprintf(stderr, "error parsing pg entry\n");
            return 0;
        }
    }
    pi->stn.pg = ss;

    // Seek past any unused items
    bs_seek_byte(bits, pos + len);
    return 1;
}

static int
_parse_playlistmark(BITSTREAM *bits, MPLS_PL *pl)
{
    int ii;
    MPLS_PLM *plm;

    bs_seek_byte(bits, pl->mark_pos);
    // Skip the length field, I don't use it
    bs_skip(bits, 4);
    // Then get the number of marks
    pl->mark_count = bs_read(bits, 16);

    plm = malloc(pl->mark_count * sizeof(MPLS_PLM));
    for (ii = 0; ii < pl->mark_count; ii++) {
        plm[ii].mark_id       = bs_read(bits, 8);
        plm[ii].mark_type     = bs_read(bits, 8);
        plm[ii].play_item_ref = bs_read(bits, 16);
        plm[ii].time          = bs_read(bits, 32);
        plm[ii].entry_es_pid  = bs_read(bits, 16);
        plm[ii].duration      = bs_read(bits, 32);
    }
    pl->play_mark = plm;
    return 1;
}

static int
_parse_playlist(BITSTREAM *bits, MPLS_PL *pl)
{
    int ii;
    MPLS_PI *pi;

    bs_seek_byte(bits, pl->list_pos);
    // Skip playlist length
    bs_skip(bits, 32);
    // Skip reserved bytes
    bs_skip(bits, 16);

    pl->list_count = bs_read(bits, 16);
    pl->sub_count = bs_read(bits, 16);

    pi = calloc(pl->list_count,  sizeof(MPLS_PI));
    for (ii = 0; ii < pl->list_count; ii++) {
        if (!_parse_playitem(bits, &pi[ii])) {
            X_FREE(pi);
            fprintf(stderr, "error parsing play list item\n");
            return 0;
        }
    }
    pl->play_item = pi;
#if 0
    // TODO
    for (ii = 0; ii < pl->sub_count; ii++)
    {
        if (!parse_subpath(stream, pl))
        {
            fprintf(stderr, "error parsing subpath\n");
            return 0;
        }
    }
#else
    if (pl->sub_count && mpls_verbose) {
        fprintf(stderr, "Subpath not supported. Skipping\n");
    }
#endif

    return 1;
}

static void
_extrapolate(MPLS_PL *pl)
{
    uint64_t duration = 0;
    int ii;

    for (ii = 0; ii < pl->list_count; ii++) {
        MPLS_PI *pi;

        pi = &pl->play_item[ii];
        pi->abs_start = duration;
        duration += pi->out_time - pi->in_time;
        pi->abs_end = duration;
    }
    pl->duration = duration;
    for (ii = 0; ii < pl->mark_count; ii++) {
        MPLS_PI *pi;
        MPLS_PLM *plm;

        plm = &pl->play_mark[ii];
        if (plm->play_item_ref < pl->list_count) {
            pi = &pl->play_item[plm->play_item_ref];
            plm->abs_start = pi->abs_start + plm->time - pi->in_time;
        } else {
            plm->abs_start = 0;
        }
    }
}

void
mpls_free(MPLS_PL *pl)
{
    int ii;

    if (pl == NULL) {
        return;
    }
    if (pl->play_mark != NULL) {
        X_FREE(pl->play_mark);
    }

    for (ii = 0; ii < pl->list_count; ii++) {
        MPLS_PI *pi;
        pi = &pl->play_item[ii];
        if (pi->stn.video != NULL) {
            X_FREE(pi->stn.video);
        }
        if (pi->stn.audio != NULL) {
            X_FREE(pi->stn.audio);
        }
        if (pi->stn.pg != NULL) {
            X_FREE(pi->stn.pg);
        }
    }

    if (pl->play_item != NULL) {
        X_FREE(pl->play_item);
    }
    X_FREE(pl);
}

MPLS_PL*
mpls_parse(char *path, int verbose)
{
    BITSTREAM  bits;
    FILE_H    *fp;
    MPLS_PL   *pl = NULL;

    mpls_verbose = verbose;

    pl = calloc(1, sizeof(MPLS_PL));
    if (pl == NULL) {
        return NULL;
    }

    fp = file_open(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open %s\n", path);
        X_FREE(pl);
        return NULL;
    }

    bs_init(&bits, fp);
    if (!_parse_header(&bits, pl)) {
        file_close(fp);
        mpls_free(pl);
        return NULL;
    }
    if (!_parse_playlist(&bits, pl)) {
        file_close(fp);
        mpls_free(pl);
        return NULL;
    }
    if (!_parse_playlistmark(&bits, pl)) {
        file_close(fp);
        mpls_free(pl);
        return NULL;
    }
    _extrapolate(pl);
    file_close(fp);
    return pl;
}

