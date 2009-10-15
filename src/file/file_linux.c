
#include <malloc.h>
#include <stdio.h>

#include "file.h"

FILE_H *file_open_linux(const char* filename, const char *mode);
void file_close_linux(FILE_H *file);
int64_t file_seek_linux(FILE_H *file, int64_t offset, int32_t origin);
int64_t file_tell_linux(FILE_H *file);
int file_read_linux(FILE_H *file, uint8_t *buf, int64_t size);
int file_write_linux(FILE_H *file, uint8_t *buf, int64_t size);


void file_close_linux(FILE_H *file)
{
    fclose((FILE *)file->internal);
}

int64_t file_seek_linux(FILE_H *file, int64_t offset, int32_t origin)
{
    return fseek((FILE *)file->internal, offset, origin);
}

int64_t file_tell_linux(FILE_H *file)
{
    return ftell((FILE *)file->internal);
}

int file_read_linux(FILE_H *file, uint8_t *buf, int64_t size)
{
    return fread(buf, 1, size, (FILE *)file->internal);
}

int file_write_linux(FILE_H *file, uint8_t *buf, int64_t size)
{
    return fwrite(buf, 1, size, (FILE *)file->internal);
}

FILE_H *file_open_linux(const char* filename, const char *mode)
{
    FILE *fp = NULL;
    FILE_H *file = malloc(sizeof(FILE_H));

    file->close = file_close_linux;
    file->seek = file_seek_linux;
    file->read = file_read_linux;
    file->write = file_write_linux;
    file->tell = file_tell_linux;

    if ((fp = fopen(filename, mode))) {
        file->internal = fp;

        return file;
    }

    return NULL;
}

const struct file_type file_linux = {
    LINUX,
    file_open_linux
};
