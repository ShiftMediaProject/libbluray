#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <unistd.h>

#include "libbluray/bluray.h"
#include "../util/logging.h"

#define OPTS "hs:"

static void _usage(char *cmd)
{
    fprintf(stderr, 
"%s <BD base dir> [-s <seconds>]\n"
"   List the titles and title info of a BD\n"
"   -s #    - Filter out titles shorter then # seconds\n"
"   -h      - This message\n",
        cmd
    );
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    BLURAY *bd;
    int count, ii, opt, seconds;

    DEBUG(DBG_BLURAY,"\nListing titles:\n");

    do
    {
        opt = getopt(argc, argv, OPTS);
        switch (opt)
        {
            case -1:
                break;
            case 's':
                seconds = strtol(optarg, NULL, 0);
                break;
            case 'h':
            default:
                _usage(argv[0]);
                break;
        }
    } while (opt != -1);

    if (optind >= argc)
    {
        _usage(argv[0]);
    }
    bd = bd_open(argv[optind], NULL);

    count = bd_get_titles(bd, TITLES_RELEVANT);
    for (ii = 0; ii < count; ii++)
    {
        BD_TITLE_INFO* ti;
        ti = bd_get_title_info(bd, ii);
        if (ti->duration / 90000 < seconds)
        {
            continue;
        }
        printf(
       "index: %d duration: %02"PRIu64":%02"PRIu64":%02"PRIu64" chapters: %d\n",
              ii,
              (ti->duration / 90000) / (3600),
              ((ti->duration / 90000) % 3600) / 60,
              ((ti->duration / 90000) % 60),
              ti->chapter_count
        );
    }
    return 0;
}
