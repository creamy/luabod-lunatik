#include <sys/param.h>
#include <sys/kmem.h>

#ifndef _LUA_INCLUDE_STDLIB
#define _LUA_INCLUDE_STDLIB

#define realloc(ptr, nsize)	kmem_alloc(nsize, KM_SLEEP)
#define free(ptr)		kmem_free(ptr, osize)

#define exit(EXIT_FAILURE)	return

#endif

