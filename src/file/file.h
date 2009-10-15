
#ifndef FILE_H_
#define FILE_H_

#include <stdint.h>
#include <unistd.h>

enum OS {
    LINUX,
    OSX,
    BSD,
};

typedef struct file FILE_H;
struct file
{
    void* internal;
    void (*close)(FILE_H *file);
    int64_t (*seek)(FILE_H *file, int64_t offset, int32_t origin);
    int64_t (*tell)(FILE_H *file);
    int (*read)(FILE_H *file, uint8_t *buf, int64_t size);
    int (*write)(FILE_H *file, uint8_t *buf, int64_t size);
};

struct file_type
{
    enum OS os;
    FILE_H *(*open)(const char *filename, const char *mode);
};

FILE_H *file_open(const char *filename, const char *mode);
void file_close(FILE_H *file);
int64_t file_seek(FILE_H *file, int64_t offset, int32_t origin);
int64_t file_tell(FILE_H *file);
int file_read(FILE_H *file, uint8_t *buf, int64_t size);
int file_write(FILE_H *file, uint8_t *buf, int64_t size);

#endif /* FILE_H_ */
