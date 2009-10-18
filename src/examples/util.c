#include "config.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void
str_realloc(STRING *str, int size)
{
    if (str == NULL) return;
    if (size > str->alloc)
    {
        // Alloc some extra
        size = 2 * size;
        str->buf = realloc(str->buf, size);
        str->alloc = size;
    }
}

inline void
str_append_sub(STRING *str, char *append, int start, int app_len)
{

    if (str == NULL) return;
    str_realloc(str, str->len + app_len + 1);
    memcpy(str->buf+str->len, append+start, app_len);
    str->len += app_len;
    str->buf[str->len] = 0;
}

STRING*
str_substr(char *str, int start, int len)
{
    STRING *ss;

    ss = calloc(1, sizeof(STRING));
    str_append_sub(ss, str, start, len);
    return ss;
}

inline void
str_append(STRING *str, char *append)
{
    str_append_sub(str, append, 0, strlen(append));
}

void
str_printf(STRING *str, const char *fmt, ...)
{
    /* Guess we need no more than 100 bytes. */
    int len;
    va_list ap;
    int size = 100;

    str_realloc(str, size);
    while (1) 
    {
        /* Try to print in the allocated space. */
        va_start(ap, fmt);
        len = vsnprintf(str->buf, size, fmt, ap);
        va_end(ap);
        /* If that worked, return the string. */
        if (len > -1 && len < size)
        {
            str->len = len;
            return;
        }
        /* Else try again with more space. */
        if (len > -1)    /* glibc 2.1 */
            size = len+1; /* precisely what is needed */
        else           /* glibc 2.0 */
            size *= 2;  /* twice the old size */
        str_realloc(str, size);
    }
}

void
str_free(STRING *str)
{
    if (str == NULL || str->buf == NULL) return;
    free(str->buf);
    str->len = 0;
    str->alloc = 0;
    str->buf = NULL;
}

void
hex_dump(uint8_t *buf, int count)
{
    int ii;

    for (ii = 0; ii < count; ii++)
    {
        if ((ii & 0xF) == 0xF)
            fprintf(stderr, "%02x\n", buf[ii]);
        else if ((ii & 0x7) == 0x7)
            fprintf(stderr, "%02x  ", buf[ii]);
        else
            fprintf(stderr, "%02x ", buf[ii]);
    }
    if ((ii & 0xF) != 0x0)
        fprintf(stderr, "\n");
}

void
indent_printf(int level, char *fmt, ...)
{
    va_list ap;
    int ii;
    size_t wrote;

    for (ii = 0; ii < level; ii++)
    {
        wrote = fwrite("    ", 1, 4, stdout);
    }
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    wrote = fwrite("\n", 1, 1, stdout);
}

