/*
 * This file is part of libbluray
 * Copyright (C) 2009-2010  hpi1
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

#include "libbluray/bdnav/meta_data.h"
#include "libbluray/bluray.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

static const char *_yes_no(int i)
{
    return i > 0 ? "yes" : i < 0 ? "unknown" : "no";
}

static const char *_str_chk_null(const char *s)
{
    return s ? s : "<undefined>";
}

static const char *_num2str(int i)
{
    if (i > 0 && i < 0xff) {
        static char str[32];
        sprintf(str, "%d", i);
        return str;
    }

    return "<undefined>";
}

static const char *_hex2str(const uint8_t *data, size_t len)
{
    static char *str = NULL;
    size_t i;

    str = realloc(str, 2*len + 1);
    *str = 0;

    for (i = 0; i < len; i++) {
        sprintf(str+2*i, "%02X", data[i]);
    }

    return str;
}

static const char *_aacs_error2str(int error_code)
{
    switch (error_code) {
        case BD_AACS_CORRUPTED_DISC: return "corrupted BluRay disc";
        case BD_AACS_NO_CONFIG:      return "AACS configuration file missing";
        case BD_AACS_NO_PK:          return "no matching processing key";
        case BD_AACS_NO_CERT:        return "no valid AACS certificate";
        case BD_AACS_CERT_REVOKED:   return "AACS certificate revoked";
        case BD_AACS_MMC_FAILED:     return "MMC authentication failed";
    }
    return "unknown error";
}

static const char *_res2str(int x, int y)
{
    if (x > 0 && y > 0 && x < 0xffff && y < 0xffff) {
        static char str[64];
        sprintf(str, "%dx%d", x, y);
        return str;
    }

    return "";
}

static void _print_meta(const META_DL *meta)
{
    if (!meta) {
        printf("\nNo disc library metadata\n");
        return;
    }

    unsigned ii;

    printf("\nDisc library metadata:\n");
    printf("Metadata file       : %s\n", _str_chk_null(meta->filename));
    printf("Language            : %s\n", _str_chk_null(meta->language_code));
    printf("Disc name           : %s\n", _str_chk_null(meta->di_name));
    printf("Alternative         : %s\n", _str_chk_null(meta->di_alternative));
    printf("Disc #              : %s/%s\n", _num2str(meta->di_set_number), _num2str(meta->di_num_sets));

    printf("TOC count           : %d\n", meta->toc_count);
    for (ii = 0; ii < meta->toc_count; ii++) {
        printf("\tTitle %d: %s\n",
               meta->toc_entries[ii].title_number,
               _str_chk_null(meta->toc_entries[ii].title_name));
    }

    printf("Thumbnail count     : %d\n", meta->thumb_count);
    for (ii = 0; ii < meta->thumb_count; ii++) {
        printf("\t%s \t%s\n",
               _str_chk_null(meta->thumbnails[ii].path),
               _res2str(meta->thumbnails[ii].xres, meta->thumbnails[ii].yres));
    }
}

int main(int argc, char *argv[])
{
    const char *disc_root = (argc > 1) ? argv[1] : NULL;
    const char *keyfile   = (argc > 2) ? argv[2] : NULL;

    if (!disc_root) {
        fprintf(stderr,
                "%s <BD base dir> [keyfile]\n"
                "   Show BD disc info\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }


    BLURAY *bd = bd_open(disc_root, keyfile);
    if (!bd) {
        fprintf(stderr, "bd_open('%s', '%s') failed.\n", disc_root, keyfile);
        exit(EXIT_FAILURE);
    }

    const BLURAY_DISC_INFO *info = bd_get_disc_info(bd);
    if (!info) {
        fprintf(stderr, "bd_get_disc_info() failed.\n");
        exit(EXIT_FAILURE);
    }

    printf("BluRay detected     : %s\n", _yes_no(info->bluray_detected));
    if (info->bluray_detected) {
        printf("First Play supported: %s\n", _yes_no(info->first_play_supported));
        printf("Top menu supported  : %s\n", _yes_no(info->top_menu_supported));
        printf("HDMV titles         : %d\n", info->num_hdmv_titles);
        printf("BD-J titles         : %d\n", info->num_bdj_titles);
        printf("UNSUPPORTED titles  : %d\n", info->num_unsupported_titles);
    }

    printf("\nAACS detected       : %s\n", _yes_no(info->aacs_detected));
    if (info->aacs_detected) {
        printf("libaacs detected    : %s\n", _yes_no(info->libaacs_detected));
        if (info->libaacs_detected) {
          printf("Disc ID             : %s\n", _hex2str(info->disc_id, sizeof(info->disc_id)));
          printf("AACS MKB version    : %d\n", info->aacs_mkbv);
          printf("AACS handled        : %s\n", _yes_no(info->aacs_handled));
          if (!info->aacs_handled) {
            printf("                      (%s)\n", _aacs_error2str(info->aacs_error_code));
          }
        }
    }

    printf("\nBD+ detected        : %s\n", _yes_no(info->bdplus_detected));
    if (info->bdplus_detected) {
        printf("libbdplus detected  : %s\n", _yes_no(info->libbdplus_detected));
        printf("BD+ handled         : %s\n", _yes_no(info->bdplus_handled));
    }

    _print_meta(bd_get_meta(bd));

    bd_close(bd);

    return 0;
}
