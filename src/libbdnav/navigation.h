
#include <stdint.h>
#include "libbdnav/mpls_parse.h"
#include "libbdnav/clpi_parse.h"

#define CONNECT_NON_SEAMLESS 0
#define CONNECT_SEAMLESS 1

typedef struct {
    char     name[11];
    uint32_t start_spn;
    uint32_t end_spn;
    uint8_t  connection;
} NAV_CLIP;

typedef struct {
    char     root[1024];
    MPLS_PL  *pl;
    CLPI_CL  *cl;
    int      clip_index;
} NAV_TITLE;

char* nav_find_main_title(char *root);
NAV_TITLE* nav_title_open(char *root, char *playlist);
void nav_title_close(NAV_TITLE *title);
NAV_CLIP* nav_next_clip(NAV_TITLE *title);
