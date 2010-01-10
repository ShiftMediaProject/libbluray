
#ifndef BLURAY_H_
#define BLURAY_H_

#include <stdint.h>
#include <unistd.h>

#include "file/file.h"

typedef int (*fptr_int)();
typedef void* (*fptr_p_void)();

typedef struct bluray BLURAY;
struct bluray {
    char device_path[100];
    FILE_H *fp;
    uint64_t s_size;
    uint64_t s_pos;
    void *aacs, *bdplus;
    void *h_libaacs, *h_libbdplus, *h_libbdnav;
    fptr_int libaacs_decrypt_unit;
    uint8_t int_buf[6144];
    uint16_t int_buf_off;
};

BLURAY *bd_open(const char* device_path, const char* keyfile_path); // Init libbluray objs
void bd_close(BLURAY *bd);                                          // Free libbluray objs

uint64_t bd_seek(BLURAY *bd, uint64_t pos);                // Seek to pos in currently selected title file
int bd_read(BLURAY *bd, unsigned char *buf, int len);   // Read from currently selected title file, decrypt if possible

int bd_select_title(BLURAY *bd, uint64_t title);    // Select an m2ts file (title is the file number, e.g. title = 123 will select 00123.m2ts)
uint64_t bd_get_title_size(BLURAY *bd);             // Returns file size in bytes of currently selected title, 0 in no title selected

uint64_t bd_tell(BLURAY *bd);       // Return current pos

#endif /* BLURAY_H_ */
