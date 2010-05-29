/*
 * This file is part of libbluray
 * Copyright (C) 2008-2010  Accident
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_MALLOC_H
#include <malloc.h>
#endif

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#endif

#include <stdio.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif


#include "bdplus.h"

extern int bluray_debuglevel; // FIXME


int main(int argc, char **argv)
{
    VM *vm = NULL;

    if (argc < 2) {
        fprintf(stderr, "%s /path/tobluray\r\n", argv[0]);
        fprintf(stderr, "Where we expect to find /path/tobluray/BDSVM/\r\n");
        exit(1);
    }

    if (chdir(argv[1])) {
        perror("chdir(): ");
        exit(1);
    }

#if 0
    // Technically not required, yet
    if (bdplus_init(NULL, NULL, NULL)) goto fail;

    // Debug flags?
    bluray_debuglevel = 3;

    // Create the VM
    vm = bdplus_VM_new(0);

    if (!vm) goto fail;

    printf("Loading core...\n");
    if (bdplus_loadsvm(vm, 0x0, "BDSVM/00000.svm")) {
        printf("Unable to load BDSVM/00000.svm\n");
        goto fail;
    }


    printf("Loading slots...\n");
    bdplus_load_slots(vm, "flash.bin");

    // Set VolumeID?
    // if (bdplus_loadVolumeID(vm, "volume_id.bin")) {

    printf("Running VM for convtab...\n");
    result = bdplus_run_convtab(vm);

    printf("run_convtab said %d\n", result);
#endif
    printf("Cleaning up...\n");


    if (vm) {
        //        bdplus_VM_free(vm);
        vm = NULL;
    }

    //    bdplus_free();

    exit(0);
}

