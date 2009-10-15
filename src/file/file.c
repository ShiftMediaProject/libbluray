
#include "file.h"

extern const struct file_type file_linux;

FILE_H *file_open(const char *filename, const char *mode)
{
    return file_linux.open(filename, mode);
}

void file_close(FILE_H *file)
{
    file->close(file);
}

int64_t file_seek(FILE_H *file, int64_t offset, int32_t origin)
{
    return file->seek(file, offset, origin);
}

int64_t file_tell(FILE_H *file)
{
    return file->tell(file);
}

int file_read(FILE_H *file, uint8_t *buf, int64_t count)
{
    return file->read(file, buf, count);
}

int file_write(FILE_H *file, uint8_t *buf, int64_t count)
{
    return file->write(file, buf, count);
}
