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

/*-
 * Copyright (c) 2008 The NetBSD Foundation, Inc.
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
__RCSID("$NetBSD: lkmodule.c,v 0.2 2010/08/16 XX:XX:XX ln Exp $");

#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include "lunatik.h"
#include "lkdevice.h"


lunatik_State *lunatik_registry[ LUNATIK_REGISTRY_MAXID ];

static int lunatikM_init(void);

static int lunatikM_fini(void);

static int lunatikR_init(void);

static int lunatikR_fini(void);

static int
lunatik_modcmd(struct module *module, int cmd, void *arg)
{
	int e = 0;

	switch (cmd) {
		case MOD_LOAD:
			e = lunatikM_init();
			break;
		case MOD_UNLOAD:
			e = lunatikM_fini();
			break;
		default:
			e = EOPNOTSUPP;
			break;
	}

	return e;
}

static int
lunatikM_init(void)
{
	int error;
	
	error = lunatikR_init();
	if (error != 0)
		return error;

	error = lunatikD_init();
	if (error != 0)
		return error;

	return 0;
}

static int
lunatikM_fini(void)
{
	int error;

	error = lunatikD_fini();
	if (error != 0)
		return error;

	error = lunatikR_fini();
	if (error != 0)
		return error;

	return 0;
}

static int
lunatikR_init(void)
{
	int i;

	for (i = 0; i < LUNATIK_REGISTRY_MAXID; i++)
		lunatik_registry[ i ] = NULL;

	return 0;
}

static int
lunatikR_fini(void)
{
	int i;

	for (i = 0; i < LUNATIK_REGISTRY_MAXID; i++){
		if(lunatik_registry[ i ] != NULL)
			return EBUSY;
	}

	return 0;
}

/*
static
void
handle_props(prop_dictionary_t props)
{
	prop_string_t str;

	if (props != NULL) {
		str = prop_dictionary_get(props, "msg");
	} else {
		str = NULL;
	}
	if (str == NULL)
		printf("The 'msg' property was not given.\n");
	else if (prop_object_type(str) != PROP_TYPE_STRING)
		printf("The 'msg' property is not a string.\n");
	else {
		const char *msg = prop_string_cstring_nocopy(str);
		if (msg == NULL)
			printf("Failed to process the 'msg' property.\n");
		else
			printf("The 'msg' property is: %s\n", msg);
	}
}
*/

static moduledata_t lunatik_mod_conf = {
        "lunatik_luabod",
        lunatik_modcmd,
        NULL
};

DECLARE_MODULE(lunatik_luabod,lunatik_mod_conf,SI_SUB_DRIVERS,SI_ORDER_MIDDLE);


