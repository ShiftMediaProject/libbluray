
#include "file.h"

Fhat ILE_H* file_open(const char* filename, uint8_t mode)
{
    //file_open_linux(filename, mode);
}

FILE_H* file_close(FILE_H* file)
FILE_H* file_seek(FILE_H* file, int64_t offset, int64_t origin);
FILE_H* file_tell(FILE_H* file);
FILE_H* file_read(FILE_H* file, uint8_t *buf, int64_t count);
FILE_H* file_write(FILE_H* file, uint8_t *buf, int64_t count);
