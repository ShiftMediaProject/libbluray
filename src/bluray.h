
#ifndef BLURAY_H_
#define BLURAY_H_

#include <stdint.h>
#include <unistd.h>

#include "file/file.h"
#include "libaacs/aacs.h"
#include "libbdplus/bdplus.h"

typedef int (*fptr_int)();

typedef struct bluray BLURAY;
struct bluray {
    char device_path[100];
    FILE_H *fp;
    uint64_t s_size;
    off_t s_pos;
    AACS_KEYS *aacs;
    void *h_libaacs;
    fptr_int libaacs_decrypt_unit;
};

BLURAY *bd_open(const char* device_path, const char* keyfile_path);
void bd_close(BLURAY *bd);
off_t bd_seek(BLURAY *bd, uint64_t pos);
int bd_read(BLURAY *bd, unsigned char *buf, int len);
int bd_select_title(BLURAY *bd, uint64_t title);

#endif /* BLURAY_H_ */
