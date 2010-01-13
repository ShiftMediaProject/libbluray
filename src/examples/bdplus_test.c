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
    int result;

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


 fail:
    if (vm) {
        //        bdplus_VM_free(vm);
        vm = NULL;
    }

    //    bdplus_free();

    exit(0);
}

