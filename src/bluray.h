
#ifndef BLURAY_H_
#define BLURAY_H_

#include <stdint.h>
#include <unistd.h>

#include "file/file.h"

typedef struct bluray BLURAY;
struct bluray {
    char device_path[100];
    uint64_t title;
    FILE_H *fp;
    off_t s_size, s_pos;
};

BLURAY *bd_open(BLURAY *bd);
void bd_select_title(BLURAY *bd, uint64_t title);
void bd_close(BLURAY *bd);
off_t bd_seek(BLURAY *bd, off_t pos);
int bd_read(BLURAY *bd, unsigned char *buf, int len);

#endif /* BLURAY_H_ */
