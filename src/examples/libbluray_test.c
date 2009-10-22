
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "../bluray.h"

#define HEX_PRINT(X,Y) { int zz; for(zz = 0; zz < Y; zz++) fprintf(stderr, "%02X", X[zz]); fprintf(stderr, "\n"); }

int main(int argc, char *argv[])
{
    if (argc == 4) {
    BLURAY *bd = bd_open(argv[1], argv[2]);

    bd_select_title(bd, atoi(argv[3]));

    unsigned char *buf = malloc(6144);
    memset(buf,0,6144);

    bd_read(bd, buf, 2048);
    bd_read(bd, buf, 2048);
    bd_read(bd, buf, 2048);
    bd_read(bd, buf, 6144);

    bd_close(bd);
    } else {
        printf("\nUsage:\n   %s <media_path> <keyfile_path> <title_number>\n\n", argv[0]);
    }

    return 0;
}

