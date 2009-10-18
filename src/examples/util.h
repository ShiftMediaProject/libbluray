//#include <stdio.h>
#include <stdint.h>
//#include <stdlib.h>
//#include <stdarg.h>
//#include <string.h>

#if defined( __MINGW32__ )
#   undef  lseek
#   define lseek  _lseeki64
#   undef  fseeko
#   define fseeko fseeko64
#   undef  ftello
#   define ftello ftello64
#   define flockfile(...)
#   define funlockfile(...)
#   define getc_unlocked getc
#   undef  off_t
#   define off_t off64_t
#   undef  stat
#   define stat  _stati64
#   define fstat _fstati64
#   define wstat _wstati64
#endif

typedef struct
{
    char * buf;
    int    alloc;
    int    len;
} STRING;

void bdt_hex_dump(uint8_t *buf, int count);

void str_append_sub(STRING *str, char *append, int start, int app_len);
STRING* str_substr(char *str, int start, int len);
void str_append(STRING *str, char *append);
void str_printf(STRING *str, const char *fmt, ...);
void str_free(STRING *str);
void hex_dump(uint8_t *buf, int count);
void indent_printf(int level, char *fmt, ...);

