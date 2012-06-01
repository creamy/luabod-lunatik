#include <sys/param.h>

#ifndef _LUA_INCLUDE_STDIO
#define _LUA_INCLUDE_STDIO

#define fprintf(output, ...)	printf(__VA_ARGS__)

#include <sys/kprintf.h>
#define BUFSIZ	KPRINTF_BUFSIZE

#define putchar(c)	printf("%c", c)

#endif

