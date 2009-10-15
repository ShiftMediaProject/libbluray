
#include <malloc.h>

#include "file.h"

void file_close_linux(FILE_H* file);
int64_t file_seek_linux(FILE_H* file, int64_t offset, int64_t origin);
int64_t file_tell_linux(FILE_H* file);
int file_read_linux(FILE_H* file, uint8_t *buf, int64_t count);
int file_write_linux(FILE_H* file, uint8_t *buf, int64_t count);


FILE_H *file_open_linux(const char* filename, uint8_t mode)
{
    FILE_H *file = malloc(sizeof(FILE_H));

    file->close = file_close_linux;
    file->seek = file_seek_linux;
    file->read = file_read_linux;
    file->write = file_write_linux;
    file->tell = file_tell_linux;

    return file;
}
