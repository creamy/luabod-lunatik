#include <sys/null.h>

#ifndef _LUA_INCLUDE_LOCALE_
#define _LUA_INCLUDE_LOCALE_

struct lconv {
   char * decimal_point ;
} ;

#define localeconv()	NULL
#endif

