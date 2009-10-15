
#include "file.h"

FILE_H* file_open(const char* filename, uint8_t mode)
{
    //return file_open_linux(filename, mode);
}

void file_close(FILE_H* file)
{
    file->close(file);
}

uint64_t file_seek(FILE_H* file, uint64_t offset, uint64_t origin)
{
    file->seek(file, offset, origin);
}

uint64_t file_tell(FILE_H* file)
{
    file->tell(file);
}

int file_read(FILE_H* file, uint8_t *buf, uint64_t count)
{
    file->read(file, buf, count);
}

int file_write(FILE_H* file, uint8_t *buf, uint64_t count)
{
    file->write(file, buf, count);
}
