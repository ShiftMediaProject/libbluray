
#ifndef BLURAY_H_
#define BLURAY_H_

#include <stdint.h>
#include <unistd.h>

#include "file/file.h"
#include "libaacs/aacs.h"

typedef struct bluray BLURAY;
struct bluray {
    char device_path[100];
    FILE_H *fp;
    uint64_t title;
    uint64_t s_size;
    off_t s_pos;
    AACS_KEYS *aacs;
    void *libaacs_h, *libbdplus_h;
};

BLURAY *bd_open(const char* device_path, const char* keyfile_path);
void bd_close(BLURAY *bd);
void bd_select_title(BLURAY *bd, uint64_t title);
off_t bd_seek(BLURAY *bd, off_t pos);
int bd_read(BLURAY *bd, unsigned char *buf, int len);

#endif /* BLURAY_H_ */
