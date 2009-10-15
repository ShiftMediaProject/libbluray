
#ifndef KEYFILE_H_
#define KEYFILE_H_

#include <stdint.h>
#include <unistd.h>

typedef struct keyfile KEYFILE;
struct keyfile {
    size_t size;
    uint8_t *buf;
};

KEYFILE *keyfile_open(const char *path);
void keyfile_close(KEYFILE *kf);

#endif /* KEYFILE_H_ */
