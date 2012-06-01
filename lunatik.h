/*-
 * Copyright (c) 2010 Lourival Vieira Neto <lneto@inf.puc-rio.br>. 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__COPYRIGHT("@(#) Copyright (c) 2010\
 Lourival Vieira Neto <lneto@inf.puc-rio.br>, All rights reserved.");
__RCSID("$NetBSD: lunatik.h,v 0.4 2010/08/16 XX:XX:XX ln Exp $");

#ifndef _SYS_LUNATIK_H_
#define _SYS_LUNATIK_H_

#include "lua/lua.h"
typedef struct lunatik_userparms
{
	const char *script;
	size_t      size_script;
	char       *result;
	size_t      size_result;
	int         lua_error;
	int         reg_id;
} lunatik_userparms_t;

#define LUNATIK_IOCTL _IOWR('i', 0x1, lunatik_userparms_t)

#ifdef _KERNEL


#define LUNATIK_ALLOC(alloc, ud, size)		alloc(ud, NULL, 0, size)

#define LUNATIK_FREE(alloc, ud, ptr, size)	alloc(ud, ptr, size, 0)

#define LUNATIK_REGISTRY_MAXID 10

typedef void (*lunatik_lock_t)(void *);

typedef void (*lunatik_unlock_t)(void *);

typedef struct lunatik_State lunatik_State;

/*
 * Lunatik new state
 *
 * create a new Lunatik state
 *
 * parameters:
 * 	allocf	- allocator function
 * 	ud	- user data to be given to allocator function 
 * 	mutex	- mutex handler to be given to lock/unlock functions
 * 	lockf	- lock function
 * 	unlockf	- unlock function
 *
 * returns:
 * 	new state	- 
 * 	NULL		- if there is no memory available
 */

lunatik_State * lunatik_newstate(lua_Alloc, void *, void *, lunatik_lock_t,
		                 lunatik_unlock_t);

/*
 * Lunatik close
 *
 * close a Lunatik state
 *
 * parameters:
 * 	Lk	- Lunatik state
 */

void lunatik_close(lunatik_State *);

/*
 * Lunatik lock
 *
 * lock a Lunatik state using the given lock function
 *
 * parameters:
 * 	Lk	- Lunatik state
 */

inline void lunatik_lock(lunatik_State *);

/*
 * Lunatik unlock
 *
 * unlock a Lunatik state using the given unlock function
 *
 * parameters:
 * 	Lk	- Lunatik state
 */

inline void lunatik_unlock(lunatik_State *);

/*
 * Lunatik alloc
 *
 * alloc memory using the given allocator function
 *
 * parameters:
 * 	Lk	- Lunatik state
 * 	size	- size in bytes to be allocated
 *
 * returns:
 * 	new pointer	- 
 * 	NULL		- if there is no memory available
 */

inline void *lunatik_alloc(lunatik_State *, size_t);

/*
 * Lunatik free
 *
 * free memory using the given allocator function
 *
 * parameters:
 * 	Lk	- Lunatik state
 * 	ptr	- pointer to be freed
 * 	size	- size in bytes to be freed
 */

inline void lunatik_free(lunatik_State *, void *, size_t);

/*
 * Lunatik get Lua state
 *
 * returns the Lua state encapsulated in the given Lunatik state
 *
 * parameters:
 * 	Lk	- Lunatik state
 *
 * returns:
 * 	Lua state	- 
 */

inline lua_State *lunatik_getluastate(lunatik_State *);

/*
 * Lunatik run string
 *
 * run a string containing Lua code
 *
 * parameters:
 * 	Lk	- Lunatik state
 * 	string	- string containing Lua code
 * 	resultp	- pointer to be uptated with the result
 * 	lenp	- pointer to be uptated with the result length
 *
 * returns:
 * 	0		- no errors
 * 	LUA_ERRSYNTAX	- syntax error during pre-compilation
 * 	LUA_ERRMEM	- memory allocation error
 */

int lunatik_runstring(lunatik_State *, const char *, char **, size_t *);

/*
 * Lunatik run C function
 *
 * run a lua_CFunction
 *
 * parameters:
 * 	Lk		- Lunatik state
 * 	cfunction	- C function to be ran
 * 	lighuserdata	- light user data to be given to the cfuntion
 * 	resultp		- pointer to be uptated with the result
 * 	lenp		- pointer to be uptated with the result length
 *
 * returns:
 * 	0		- no errors
 * 	LUA_ERRMEM	- memory allocation error
 */

int lunatik_runcfunction(lunatik_State *, lua_CFunction, void *, char **, size_t *);

/* Lunatik auxiliary library */

/*
#include <sys/kmem.h>
*/
#include <sys/lock.h>
#include <sys/mutex.h>

/*
 * Lunatik auxliary new state
 *
 * create a new Lunatik state using kmem allocator and kmutex
 *
 * parameters:
 * 	km_flags	- flags to be given to the kmem_alloc/free functions
 * 	kmutex		- initialized kmutex 
 * 	kmutex_type	- the type of the given kmutex
 *
 * returns:
 * 	new state	- 
 * 	NULL		- if there is no memory available
 */

lunatik_State *lunatikL_newstate(km_flag_t, kmutex_t *, kmutex_type_t);

/*
 * Lunatik auxliary run user script
 *
 * create a new Lunatik state using kmem allocator and kmutex
 *
 * parameters:
 * 	userparms	- user parameters
 *
 * returns:
 * 	0		- no errors
 * 	errno		- error
 */

int lunatikL_runuserscript(lunatik_userparms_t *);

#endif
#endif /* !_SYS_LUNATIK_H_ */

