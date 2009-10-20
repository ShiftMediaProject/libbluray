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

    title = malloc(sizeof(NAV_TITLE));
    if (title == NULL) {
        return NULL;
    }
    strncpy(title->root, root, 1024);
    title->root[1023] = '\0';
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
    title->cl = NULL;
    title->clip_index = -1;
    return title;
}

void nav_title_close(NAV_TITLE *title)
{
    mpls_free(title->pl);
    clpi_free(title->cl);
    X_FREE(title);
}

/*
 * Input Parameters:
 * title     - title struct obtained from nav_title_open
 *
 * Return value:
 * Pointer to NAV_CLIP struct
 * NULL - End of clip list
 *
 * free with X_FREE
 */
NAV_CLIP* nav_next_clip(NAV_TITLE *title)
{
    NAV_CLIP *clip;
    MPLS_PI *pi;
	char *path;

    clip = malloc(sizeof(NAV_CLIP));
    if (clip == NULL) {
        return NULL;
    }

    clpi_free(title->cl);
    title->cl = NULL;

    title->clip_index++;
    if (title->clip_index >= title->pl->list_count) {
        return NULL;
    }

    pi = &title->pl->play_item[title->clip_index];
    path = str_printf("%s" DIR_SEP "BDMV" DIR_SEP "CLIPINF" DIR_SEP "%s.clpi",
                      title->root, pi->clip_id);
    title->cl = clpi_parse(path, 0);
	X_FREE(path);
    if (title->cl == NULL) {
        return NULL;
    }

    strncpy(clip->name, pi->clip_id, 5);
    strncpy(&clip->name[5], ".m2ts", 6);
    switch (pi->connection_condition) {
        case 5:
        case 6:
            clip->start_spn = 0;
            clip->connection = CONNECT_SEAMLESS;
            break;
        default:
            clip->start_spn = clpi_lookup_spn(&title->cl->cpi, pi->in_time, 1);
            clip->connection = CONNECT_NON_SEAMLESS;
            break;
    }
    clip->end_spn = clpi_lookup_spn(&title->cl->cpi, pi->out_time, 0);

    return clip;
}

