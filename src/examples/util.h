#include <stdint.h>

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

void hex_dump(uint8_t *buf, int count);
void indent_printf(int level, char *fmt, ...);

