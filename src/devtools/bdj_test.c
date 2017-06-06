/*
 * This file is part of libbluray
 * Copyright (C) 2010  William Hahne
 * Copyright (C) 2011-2017  Petri Hintukainen <phintuka@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders of libbluray
 * gives permission to link the code of its release of libbluray with the
 * OpenSSL project's "OpenSSL" library (or with modified versions of it
 * that use the same license as the "OpenSSL" library), and distribute
 * the linked executables.  You must obey the GNU General Public License
 * in all respects for all of the code used other than "OpenSSL".  If you
 * modify this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to do
 * so, delete this exception statement from your version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "bluray.h"

#if defined(_WIN32)
#include <windows.h>
#define sleep(x) Sleep(x)
#endif

static void _usage(void) {
    printf("Usage: [path to disc] [starting object]\n");
}

int main(int argc, char** argv)
{
    int major, minor, micro;
    const BLURAY_DISC_INFO *di;
    BLURAY *bd;

    bd_get_version(&major, &minor, &micro);
    printf("Using libbluray version %d.%d.%d\n", major, minor, micro);

    if (argc < 3) {
        _usage();
        return 0;
    }

    printf("Strating BD-J object %s from %s\n", argv[2], argv[1]);

    bd = bd_init();
    if (!bd) {
        return -1;
    }

    /* disable persistent storage */
    bd_set_player_setting(bd, BLURAY_PLAYER_SETTING_PERSISTENT_STORAGE, 0);

    /* initialize event queue */
    bd_get_event(bd, NULL);

    /* check BD-J config */

    di = bd_get_disc_info(bd);
    assert(di != NULL);

    if (!di->libjvm_detected) {
        fprintf(stderr, "JVM not found\n");
        goto fail;
    }
    if (!di->bdj_handled) {
        fprintf(stderr, "libbluray.jar not found\n");
        goto fail;
    }

    /* open disc */

    if (!bd_open_disc(bd, argv[1], NULL)) {
      fprintf(stderr, "bd_open_disc() failed\n");
      return -1;
    }

    bd_get_titles(bd, TITLES_ALL, 0);

    if (!bd_start_bdj(bd, argv[2])) {
        printf("Failed to start BD-J application.\n");
        goto fail;
    }

    while (1) {
        BD_EVENT ev;
        while (bd_get_event(bd, &ev)) {
        }
        sleep(20);
    }

    bd_stop_bdj(bd);
    bd_close(bd);

    return 0;

 fail:
    bd_close(bd);
    return -1;
}
