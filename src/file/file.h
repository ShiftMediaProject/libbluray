
#ifndef FILE_H_
#define FILE_H_

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

typedef struct file FILE_H;
struct file
{
    void* internal;
    void (*close)(FILE_H* file);
    int64_t (*seek)(FILE_H* file, int64_t offset, int64_t origin);
    int64_t (*tell)(FILE_H* file);
    int (*read)(FILE_H* file, uint8_t *buf, int64_t count);
    int (*write)(FILE_H* file, uint8_t *buf, int64_t count);
};

FILE_H* file_open(const char* filename, uint8_t mode);

#endif /* FILE_H_ */
