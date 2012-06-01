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
__KERNEL_RCSID(0, "$NetBSD: lkauxlib.c,v 0.3 2010/08/16 XX:XX:XX ln Exp $");

#include <sys/kmem.h>
#include <sys/param.h>

#include "lunatik.h"

static void * lk_alloc(void *, void *, size_t, size_t);

static int lk_panic(lua_State *);

lunatik_State *
lunatikL_newstate(km_flag_t km_flags, kmutex_t *kmutex, kmutex_type_t kmutex_type)
{
	lunatik_State   *Lk;
	lua_State       *L;
	lunatik_lock_t   kmutex_lock;
	lunatik_unlock_t kmutex_unlock;

	if (kmutex_type == MUTEX_SPIN){
		kmutex_lock   = (lunatik_lock_t)   mutex_spin_enter;
		kmutex_unlock = (lunatik_unlock_t) mutex_spin_exit;
	}
	else {
		kmutex_lock   = (lunatik_lock_t)   mutex_enter;
		kmutex_unlock = (lunatik_unlock_t) mutex_exit;
	}

	Lk= lunatik_newstate(lk_alloc, (void *) km_flags, (void *) kmutex,
			kmutex_lock, kmutex_unlock);

	L = lunatik_getluastate(Lk);

	lua_atpanic(L, lk_panic);

	return Lk;
}

extern lunatik_State *lunatik_registry[];

int
lunatikL_runuserscript(lunatik_userparms_t *userparms)
{
	int error;
	char *script;
	char *result;
	size_t size_result;
	lunatik_State *Lk;

	/* temp code until lunatik registry be finished */
	int reg_id = userparms->reg_id;

	if (reg_id >= 0 && reg_id < LUNATIK_REGISTRY_MAXID &&
	    lunatik_registry[ reg_id ] != NULL)
		Lk = lunatik_registry[ reg_id ];
	else
		return EFAULT;
	/* end temp code */

	script = lunatik_alloc(Lk, userparms->size_script);
	if (script == NULL)
		return ENOMEM;

	error = copyinstr(userparms->script, script, userparms->size_script, NULL);
	if (error != 0){
		lunatik_free(Lk, script, userparms->size_script);
		return error;
	}

	result = NULL;
	userparms->lua_error = (userparms->result == NULL) ? 
		lunatik_runstring(Lk, script, NULL, NULL) :
		lunatik_runstring(Lk, script, &result, &size_result);

	lunatik_free(Lk, script, userparms->size_script);

	if (result != NULL){
		int size = (userparms->size_result < size_result) ?
			userparms->size_result :
			size_result;

		error = copyoutstr(result, userparms->result, size,
			&userparms->size_result);

		lunatik_free(Lk, result, size_result);

		if (error != 0)
			return error;
	}
	else {
		userparms->result = NULL;
		userparms->size_result = 0;
	}

	return 0;
}

/* static functions */

static void *
lk_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	void *new_ptr;
	km_flag_t km_flags = (km_flag_t) ud;

	if (nsize == 0) 
    		new_ptr = NULL;
	else {
    		new_ptr = kmem_alloc(nsize, km_flags);
    		memcpy(new_ptr, ptr, osize < nsize ? osize : nsize);
  	}

  	if (ptr != NULL)
    		kmem_free(ptr, osize);

  	return new_ptr;
}


static int
lk_panic(lua_State *L)
{
  	printf("[lunatik] PANIC: unprotected error in call to Lua API (%s)\n",
			lua_tostring(L, -1));
 	return 0;
}

/* end: lunatik auxiliar library */

