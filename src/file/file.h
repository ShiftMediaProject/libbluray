
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
    uint64_t (*seek)(FILE_H* file, uint64_t offset, uint64_t origin);
    uint64_t (*tell)(FILE_H* file);
    int (*read)(FILE_H* file, uint8_t *buf, uint64_t count);
    int (*write)(FILE_H* file, uint8_t *buf, uint64_t count);
};

FILE_H *file_open(const char* filename, uint8_t mode);
void file_close(FILE_H* file);
uint64_t file_seek(FILE_H* file, uint64_t offset, uint64_t origin);
uint64_t file_tell(FILE_H* file);
int file_read(FILE_H* file, uint8_t *buf, uint64_t count);
int file_write(FILE_H* file, uint8_t *buf, uint64_t count);

#endif /* FILE_H_ */
