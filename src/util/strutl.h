
#ifndef STRUTL_H_
#define STRUTL_H_

#include "attributes.h"
 
char * str_printf(const char *fmt, ...) BD_ATTR_FORMAT_PRINTF(1,2) BD_ATTR_MALLOC;

#endif // STRUTL_H_
