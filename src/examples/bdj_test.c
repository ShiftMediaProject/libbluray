/*
 * This file is part of libbluray
 * Copyright (C) 2010  William Hahne
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

#include "libbluray/bluray.h"

void usage(void) {
    printf("Usage: [path to disc] [starting object]\n");
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        usage();
        return 0;
    }

    printf("%s %s\n", argv[1], argv[2]);

    BLURAY* bd = bd_open(argv[1], NULL);

    bd_get_titles(bd, TITLES_ALL, 0);
    bd_select_title(bd, 1);

    if (!bd_start_bdj(bd, argv[2])) {
        printf("Failed to start BD-J application.\n");
    } else {
        while (1) { sleep(20); }
        bd_stop_bdj(bd);
    }

    bd_close(bd);

    return 0;
}
