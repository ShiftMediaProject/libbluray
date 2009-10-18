#if !defined(_MPLS_PARSE_H_)
#define _MPLS_PARSE_H_

#include <stdio.h>
#include <stdint.h>

typedef struct
{
    uint8_t         stream_type;
    uint8_t         coding_type;
    uint16_t        pid;
    uint8_t         subpath_id;
    uint8_t         subclip_id;
    uint8_t         format;
    uint8_t         rate;
    uint8_t         char_code;
    uint8_t         lang[3];
} MPLS_STREAM;

typedef struct
{
    uint8_t         num_video;
    uint8_t         num_audio;
    uint8_t         num_pg;
    uint8_t         num_ig;
    uint8_t         num_secondary_audio;
    uint8_t         num_secondary_video;
    uint8_t         num_pip_pg;
    MPLS_STREAM    *video;
    MPLS_STREAM    *audio;
    MPLS_STREAM    *pg;
} MPLS_PL_STN;

typedef struct
{
    char            clip_id[5];
    uint8_t         connection_condition;
    uint8_t         stc_id;
    uint32_t        in_time;
    uint32_t        out_time;
    MPLS_PL_STN     stn;

    // Extrapolated items
    uint32_t        abs_start;
    uint32_t        abs_end;
} MPLS_PI;

typedef struct
{
    uint8_t         mark_id;
    uint8_t         mark_type;
    uint16_t        play_item_ref;
    uint32_t        time;
    uint16_t        entry_es_pid;
    uint32_t        duration;

    // Extrapolated items
    uint32_t        abs_start;
} MPLS_PLM;

typedef struct
{
    uint32_t        type_indicator;
    uint32_t        type_indicator2;
    uint32_t        list_pos;
    uint32_t        mark_pos;
    uint32_t        ext_pos;
    uint16_t        list_count;
    uint16_t        sub_count;
    uint16_t        mark_count;
    MPLS_PI        *play_item;
    MPLS_PLM       *play_mark;

    // Extrapolated items
    uint64_t        duration;
} MPLS_PL;


MPLS_PL* mpls_parse(char *path, int verbose);
void mpls_free(MPLS_PL **pl);

#endif // _MPLS_PARSE_H_
