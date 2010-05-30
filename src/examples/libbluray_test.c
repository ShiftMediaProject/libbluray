#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "libbluray/bluray.h"
#include "util/logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define HEX_PRINT(X,Y) { int zz; for(zz = 0; zz < Y; zz++) fprintf(stderr, "%02X", X[zz]); fprintf(stderr, "\n"); }

int main(int argc, char *argv[])
{
    if (argc == 4) {
    BLURAY *bd = bd_open(argv[1], argv[2]);
    int count, ii;

    //bd_get_titles(bd, 0);

    DEBUG(DBG_BLURAY,"\nListing titles:\n");

    count = bd_get_titles(bd, TITLES_RELEVANT);
    for (ii = 0; ii < count; ii++)
    {
        BD_TITLE_INFO* ti;
        ti = bd_get_title_info(bd, ii);
        DEBUG(DBG_BLURAY,
       "index: %d duration: %02"PRIu64":%02"PRIu64":%02"PRIu64" chapters: %d\n",
              ii,
              (ti->duration / 90000) / (3600),
              ((ti->duration / 90000) % 3600) / 60,
              ((ti->duration / 90000) % 60),
              ti->chapter_count);
    }

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

