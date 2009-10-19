
#ifndef FILE_H_
#define FILE_H_

#include <stdint.h>
#include <unistd.h>

//#ifdef __LINUX__
#define file_open file_open_linux
//#endif

#define file_close(X) X->close(X)
#define file_seeko(X,Y,Z) X->seeko(X,Y,Z)
#define file_tello(X) X->tello(X)
#define file_eof(X) X->eof(X)
#define file_read(X,Y,Z) X->read(X,Y,Z)
#define file_write(X,Y,Z) X->write(X,Y,Z)

typedef struct file FILE_H;
struct file
{
    void* internal;
    void (*close)(FILE_H *file);
    int64_t (*seeko)(FILE_H *file, int64_t offset, int32_t origin);
    int64_t (*tello)(FILE_H *file);
    int (*eof)(FILE_H *file);
    int (*read)(FILE_H *file, uint8_t *buf, int64_t size);
    int (*write)(FILE_H *file, uint8_t *buf, int64_t size);
};

extern FILE_H *file_open_linux(const char* filename, const char *mode);

#endif /* FILE_H_ */
