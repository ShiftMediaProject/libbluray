
#include <stdint.h>
#include "mpls_parse.h"
#include "clpi_parse.h"

#define CONNECT_NON_SEAMLESS 0
#define CONNECT_SEAMLESS 1

typedef struct {
    int      play_item_ref;
    uint32_t start_pkt;
    uint32_t start_time;
} NAV_CHAP;

typedef struct {
    int      count;
    NAV_CHAP *list;
} NAV_CHAP_LIST;

typedef struct {
    char     name[11];
    CLPI_CL  *cl;
    int      play_item_ref;
    uint32_t start_pkt;
    uint32_t end_pkt;
    uint32_t seek_pkt;
    uint8_t  connection;
} NAV_CLIP;

typedef struct {
    char     root[1024];
    char     name[11];
    MPLS_PL  *pl;
    uint32_t packets;
    NAV_CLIP *clip;
} NAV_TITLE;

char* nav_find_main_title(char *root);
NAV_TITLE* nav_title_open(char *root, char *playlist);
void nav_title_close(NAV_TITLE *title);
NAV_CLIP* nav_next_clip(NAV_TITLE *title, NAV_CLIP *clip);
