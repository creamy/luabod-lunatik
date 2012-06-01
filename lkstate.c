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
__KERNEL_RCSID(0, "$NetBSD: lkstate.c,v 0.3 2010/07/30 XX:XX:XX ln Exp $");

#include <sys/systm.h>
#include "lunatik.h"

struct lunatik_State {
	lua_State *L;
	void      *mutex; 
	lunatik_lock_t   lock;
	lunatik_unlock_t unlock;
};

static inline lua_Alloc locked_getallocf(lunatik_State *, void **);

lunatik_State *
lunatik_newstate(lua_Alloc alloc, void *ud, void *mutex,
		 lunatik_lock_t lock, lunatik_unlock_t unlock)
{
	lunatik_State *Lk = (lunatik_State *) LUNATIK_ALLOC(alloc, ud, sizeof(lunatik_State));
	if (Lk == NULL)
		return NULL;

	Lk->L = lua_newstate(alloc, ud);
	if (Lk->L == NULL){
		LUNATIK_FREE(alloc, ud, Lk, sizeof(lunatik_State));

		return NULL;
	}

	Lk->mutex  = mutex;
	Lk->lock   = lock;
	Lk->unlock = unlock;

	return Lk;
}

void
lunatik_close(lunatik_State *Lk)
{
	void *ud;
	lua_Alloc alloc = NULL;

	lunatik_lock(Lk);
	alloc = lua_getallocf(Lk->L, &ud);
	lua_close(Lk->L);
	lunatik_unlock(Lk);

	LUNATIK_FREE(alloc, ud, Lk, sizeof(lunatik_State));
}	

inline lua_State *
lunatik_getluastate(lunatik_State *Lk)
{
	return Lk->L;
}

inline void lunatik_lock(lunatik_State *Lk)
{
	Lk->lock(Lk->mutex);
}

inline void lunatik_unlock(lunatik_State *Lk)
{
	Lk->unlock(Lk->mutex);
}

inline void *
lunatik_alloc(lunatik_State *Lk, size_t size)
{ 
	void *ud;
	lua_Alloc alloc = locked_getallocf(Lk, &ud);
	
	return LUNATIK_ALLOC(alloc, ud, size);
}

inline void 
lunatik_free(lunatik_State *Lk, void *ptr, size_t size)
{ 
	void *ud;
	lua_Alloc alloc = locked_getallocf(Lk, &ud);

	LUNATIK_FREE(alloc, ud, ptr, size);
}

/* static code */

static inline lua_Alloc
locked_getallocf(lunatik_State *Lk, void **ud)
{
	lua_Alloc alloc = NULL;

	lunatik_lock(Lk);
	alloc = lua_getallocf(Lk->L, ud);
	lunatik_unlock(Lk);

	return alloc;
}

/* end: lunatik state */

