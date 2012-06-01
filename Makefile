#	$NetBSD: Makefile,v 1.2 2008/05/02 14:20:50 ad Exp $

KMOD=   lunatik 
SRCS=	lkmodule.c
SRCS+=	lkstate.c
SRCS+=	lkrun.c
#SRCS+=	lkresgister.c 
SRCS+=	lkauxlib.c 
SRCS+=	lkdevice.c 
#SRCS+=  sys_lunatik.c

CFLAGS+=      -Ilua_include

.PATH:	libc
SRCS+=	strncat.c 
SRCS+=  strspn.c 
SRCS+=  strcspn.c 

.PATH:   lua 
SRCS+=   lapi.c 
SRCS+=   lcode.c 
SRCS+=   ldebug.c 
SRCS+=   ldo.c 
SRCS+=   ldump.c 
SRCS+=   lfunc.c 
SRCS+=   lgc.c 
SRCS+=   llex.c 
SRCS+=   lmem.c 
SRCS+=   lobject.c 
SRCS+=   lopcodes.c 
SRCS+=   lparser.c 
SRCS+=   lstate.c 
SRCS+=   lstring.c 
SRCS+=   ltable.c 
SRCS+=   ltm.co 
SRCS+=   lundump.c 
SRCS+=   lvm.c 
SRCS+=   lzio.c 

SRCS+=   lauxlib.c 
SRCS+=   ltablib.c 
#SRCS+=   lstrlib.c 
#SRCS+=   lbaselib.c 
#SRCS+=   ldblib.c 

.include <bsd.kmod.mk>
