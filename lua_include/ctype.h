#include <sys/systm.h>

#ifndef _LUA_INCLUDE_CTYPE_
#define _LUA_INCLUDE_CTYPE_

LIBKERN_INLINE int
isalnum(int ch)
{
	return (isalpha(ch) || isdigit(ch));
}

LIBKERN_INLINE int
iscntrl(int ch)
{
	return ((ch >= 0x00 && ch <= 0x1F) || ch == 0x7F);
}

LIBKERN_INLINE int
isprint(int ch)
{
	return (ch >= 0x20 && ch <= 0x7E);
}

#endif

