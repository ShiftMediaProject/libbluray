#include <stdint.h>
#include <string.h>
#include "../util/macro.h"
#include "../util/logging.h"
#include "../util/strutl.h"
#include "../file/dir.h"
#include "../file/file.h"
#include "mpls_parse.h"
#include "navigation.h"

static int _filter_dup(MPLS_PL *pl_list[], int count, MPLS_PL *pl)
{
    int ii, jj;

    for (ii = 0; ii < count; ii++) {
        if (pl->list_count != pl_list[ii]->list_count ||
            pl->duration != pl_list[ii]->duration) {
            continue;
        }
        for (jj = 0; jj < pl->list_count; jj++) {
            MPLS_PI *pi1, *pi2;

            pi1 = &pl->play_item[jj];
            pi2 = &pl_list[ii]->play_item[jj];

            if (memcmp(pi1->clip_id, pi2->clip_id, 5) != 0 ||
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

char* nav_find_main_title(char *root)
{
    DIR_H *dir;
    DIRENT ent;
    char *path = NULL;
    MPLS_PL **pl_list = NULL;
    MPLS_PL **tmp = NULL;
    MPLS_PL *pl = NULL;
    int count, ii, pl_list_size = 0;
    int res;
    char longest[11];
    int longest_ii = 0;

    DEBUG(DBG_NAV, "Root: %s:\n", root);
    path = str_printf("%s" DIR_SEP "BDMV" DIR_SEP "PLAYLIST", root);
    if (path == NULL) {
        fprintf(stderr, "Failed to find playlist path: %s\n", path);
        return NULL;
    }

    dir = dir_open(path);
    if (dir == NULL) {
        fprintf(stderr, "Failed to open dir: %s\n", path);
        X_FREE(path);
        return NULL;;
    }
    X_FREE(path);

    for (ii = 0, res = dir_read(dir, &ent); !res; res = dir_read(dir, &ent)) {

        if (ent.d_name[0] == '.') {
            continue;
        }
        path = str_printf("%s" DIR_SEP "BDMV" DIR_SEP "PLAYLIST" DIR_SEP "%s",
                          root, ent.d_name);

        if (ii >= pl_list_size) {
            pl_list_size += 100;
            tmp = realloc(pl_list, pl_list_size * sizeof(MPLS_PL*));
            if (tmp == NULL) {
                break;
            }
            pl_list = tmp;
        }
        pl = mpls_parse(path, 0);
        X_FREE(path);
        if (pl != NULL) {
            if (_filter_dup(pl_list, ii, pl)) {
                pl_list[ii] = pl;
                if (pl_list[ii]->duration > pl_list[longest_ii]->duration) {
                    strncpy(longest, ent.d_name, 11);
                    longest[10] = '\0';
                    longest_ii = ii;
                }
                ii++;
            } else {
                mpls_free(pl);
            }
        }
    }
    dir_close(dir);

    count = ii;
    for (ii = 0; ii < count; ii++) {
        mpls_free(pl_list[ii]);
    }
    if (count > 0) {
        return strdup(longest);
    } else {
        return NULL;
    }
}

NAV_TITLE* nav_title_open(char *root, char *playlist)
{
    NAV_TITLE *title = NULL;
    char *path;
    int ii;

    title = malloc(sizeof(NAV_TITLE));
    if (title == NULL) {
        return NULL;
    }
    strncpy(title->root, root, 1024);
    title->root[1023] = '\0';
    strncpy(title->name, playlist, 11);
    title->name[10] = '\0';
    path = str_printf("%s" DIR_SEP "BDMV" DIR_SEP "PLAYLIST" DIR_SEP "%s",
                      root, playlist);
    title->pl = mpls_parse(path, 0);
    if (title->pl == NULL) {
        DEBUG(DBG_NAV, "Fail: Playlist parse %s\n", path);
        X_FREE(title);
        X_FREE(path);
        return NULL;
    }
    X_FREE(path);
    // Find length in packets and end_pkt for each clip
    title->clip = malloc(title->pl->list_count * sizeof(NAV_CLIP));
    title->packets = 0;
    for (ii = 0; ii < title->pl->list_count; ii++) {
        MPLS_PI *pi;
        NAV_CLIP *clip;

        pi = &title->pl->play_item[ii];

        clip = &title->clip[ii];
        clip->play_item_ref = ii;
        strncpy(clip->name, pi->clip_id, 5);
        strncpy(&clip->name[5], ".m2ts", 6);

        path = str_printf("%s"DIR_SEP"BDMV"DIR_SEP"CLIPINF"DIR_SEP"%s.clpi",
                      title->root, pi->clip_id);
        clip->cl = clpi_parse(path, 0);
        X_FREE(path);
        if (clip->cl == NULL) {
            clip->start_pkt = 0;
            clip->end_pkt = 0;
            continue;
        }
        switch (pi->connection_condition) {
            case 5:
            case 6:
                clip->start_pkt = 0;
                clip->connection = CONNECT_SEAMLESS;
                break;
            default:
                clip->start_pkt = clpi_lookup_spn(&clip->cl->cpi, pi->in_time, 1);
                clip->connection = CONNECT_NON_SEAMLESS;
            break;
        }
        clip->seek_pkt = clip->start_pkt;
        clip->end_pkt = clpi_lookup_spn(&clip->cl->cpi, pi->out_time, 0);
        if (clip->end_pkt == (uint32_t)-1) {
            // The EP map couldn't tell us where the last packet is
            // because it runs through the end of the file.  Find the
            // file's length.
            FILE_H *fp;

            path = str_printf("%s"DIR_SEP"BDMV"DIR_SEP"STREAM"DIR_SEP"%s.m2ts",
                              title->root, pi->clip_id);
            fp = file_open(path, "rb");
            if (fp != NULL) {
                file_seek(fp, 0, SEEK_END);
                clip->end_pkt = file_tell(fp) / 192;
                file_close(fp);
            } else {
                clip->end_pkt = clip->start_pkt;
            }
        }
        title->packets += clip->end_pkt - clip->start_pkt;
    }
    return title;
}

void nav_title_close(NAV_TITLE *title)
{
    int ii;

    mpls_free(title->pl);
    for (ii = 0; ii < title->pl->list_count; ii++) {
        clpi_free(title->clip[ii].cl);
    }
    X_FREE(title->clip);
    X_FREE(title);
}

// Search for random access point closest to the requested packet
// Packets are 192 byte TS packets
NAV_CLIP* nav_packet_search(NAV_TITLE *title, uint32_t pkt)
{
    uint32_t pos, len;
    NAV_CLIP *clip;
    int ii;

    pos = 0;
    for (ii = 0; ii < title->pl->list_count; ii++) {
        len = title->clip[ii].end_pkt - title->clip[ii].start_pkt;
        if (pkt < pos + len)
            break;
        pos += len;
    }
    if (ii == title->pl->list_count) {
        clip = &title->clip[ii-1];
        clip->seek_pkt = clip->end_pkt;
    } else {
        clip = &title->clip[ii];
        clip->seek_pkt = clpi_access_point(&clip->cl->cpi, pkt - pos + clip->start_pkt);
    }
    return clip;
}

// Search for random access point closest to the requested time
// Time is in 45khz ticks
NAV_CLIP* nav_time_search(NAV_TITLE *title, uint32_t tick)
{
    uint32_t pos, len;
    MPLS_PI *pi;
    NAV_CLIP *clip;
    int ii;

    pos = 0;
    for (ii = 0; ii < title->pl->list_count; ii++) {
        pi = &title->pl->play_item[ii];
        len = pi->out_time - pi->in_time;
        if (tick < pos + len)
            break;
        pos += len;
    }
    if (ii == title->pl->list_count) {
        clip = &title->clip[ii-1];
        clip->seek_pkt = clip->end_pkt;
    } else {
        clip = &title->clip[ii];
        clip->seek_pkt = clpi_lookup_spn(&clip->cl->cpi, tick - pos + pi->in_time, 1);
    }
    return clip;
}

/*
 * Input Parameters:
 * title     - title struct obtained from nav_title_open
 *
 * Return value:
 * Pointer to NAV_CLIP struct
 * NULL - End of clip list
 */
NAV_CLIP* nav_next_clip(NAV_TITLE *title, NAV_CLIP *clip)
{
    if (clip == NULL) {
        return &title->clip[0];
    }
    if (clip->play_item_ref >= title->pl->list_count - 1) {
        return NULL;
    }
    return &title->clip[clip->play_item_ref + 1];
}

