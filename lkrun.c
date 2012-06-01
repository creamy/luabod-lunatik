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
__KERNEL_RCSID(0, "$NetBSD: lkrun.c,v 0.3 2010/08/16 XX:XX:XX ln Exp $");

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lunatik.h"

typedef int (*runfunction_t)(lua_State *, void *, int);

typedef struct cfunction_t {
	lua_CFunction luacfunction;
	void         *lightuserdata;
} cfunction_t;

static int runcfunction(lua_State *, void *, int);

static int runstring(lua_State *, void *, int);

static int run(lunatik_State *, runfunction_t, void *, char **, size_t *);

int
lunatik_runcfunction(lunatik_State *Lk, lua_CFunction luacfunction,
		      void *lightuserdata, char **resultp, size_t *lenp)
{
	cfunction_t cfunction = { luacfunction, lightuserdata };
	return run(Lk, runcfunction, (void *) &cfunction, resultp, lenp);
}

int
lunatik_runstring(lunatik_State *Lk, const char *string, char **resultp,
		   size_t *lenp)
{
	return run(Lk, runstring, (void *) string, resultp, lenp);
}

/* static functions */

static int
runcfunction(lua_State *L, void *rundata, int nresult)
{
	cfunction_t *cfunction = (cfunction_t *) rundata;

	lua_pushcfunction(L, cfunction->luacfunction); 
	lua_pushlightuserdata(L, cfunction->lightuserdata);

	return lua_pcall(L, 1, nresult, 0);
}

static int
runstring(lua_State *L, void *rundata, int nresult)
{
	char *string = (char *) rundata;
	int   error = luaL_loadstring(L, string);
	if (error != 0)
		return error;

	return lua_pcall(L, 0, nresult, 0);
}

static int
run(lunatik_State *Lk, runfunction_t runfunction, void *rundata, char **resultp, size_t *sizep)
{
	int         error;
	size_t      size;
	const char *result;
	lua_State  *L = lunatik_getluastate(Lk);

	lunatik_lock(Lk);
	if (resultp == NULL){
		error = runfunction(L, rundata, 0);
		if (error != 0)
			lua_pop(L, 1);

		lunatik_unlock(Lk);
		return error;
	}

	/* assertive: resultp != NULL */

	error = runfunction(L, rundata, 1);
	
	result = lua_tolstring(L, -1, &size);
	size   = size + 1;

	if (result != NULL){
		void     *alloc_ud;
		lua_Alloc alloc = lua_getallocf(L, &alloc_ud);

		*resultp = LUNATIK_ALLOC(alloc, alloc_ud, sizeof(char) * size);
		if (*resultp != NULL)
			memcpy(*resultp, result, size);

		lua_pop(L, 1);
	}
	else 
		*resultp = NULL;

	lunatik_unlock(Lk);
	
	if (sizep != NULL)
		*sizep = size;

	return error;
}

/* end: lunatik run */

