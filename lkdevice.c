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
 * Copyright (c) 1998-2006 Brett Lymn (blymn@NetBSD.org)
 * All rights reserved.
 *
 * This code has been donated to The NetBSD Foundation by the Author.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software withough specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

#include <sys/cdefs.h>
__COPYRIGHT("@(#) Copyright (c) 2010\
 Lourival Vieira Neto <lneto@inf.puc-rio.br>, All rights reserved.");
__KERNEL_RCSID(0, "$NetBSD: lkdevice.c,v 0.1 2010/08/16 XX:XX:XX ln Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/device.h>
#include <sys/conf.h>
#include <sys/mutex.h>

#include "lunatik.h"

#include "lkdevice.h"

#ifndef _MODULE
static void lunatikattach(int num);
#endif

static int lunatikD_open(dev_t device, int flags, int fmt, struct lwp *process);

static int lunatikD_close(dev_t device, int flags, int fmt, struct lwp *process);

static int lunatikD_ioctl(dev_t device, u_long command, void *data,
		  int flags, struct lwp *process);

/* just define the character device handlers because that is all we need */
const struct cdevsw lunatik_cdevsw = {
        lunatikD_open,
	lunatikD_close,
	noread,
	nowrite,
        lunatikD_ioctl,
	nostop,
	notty,
	nopoll,
	nommap,
	nokqfilter,
};

/*
 * Attach for autoconfig to find.  The parameter is the number given in
 * the configuration file, and defaults to 1.  New code should not expect
 * static configuration from the kernel config file, and thus ignore that
 * parameter.
 */
#ifndef _MODULE
static void
lunatikattach(int num)
{
	  /* nothing to do for skeleton, this is where resources that
	     need to be allocated/initialised before open is called
	     can be set up */
}
#endif

/*
 * Handle an open request on the device.
 */
static int
lunatikD_open(dev_t device, int flags, int fmt, struct lwp *process)
{
	return 0; /* this always succeeds */
}

/*
 * Handle the close request for the device.
 */
static int
lunatikD_close(dev_t device, int flags, int fmt, struct lwp *process)
{
	return 0; /* again this always succeeds */
}

/*
 * Handle the ioctl for the device
 */
static int
lunatikD_ioctl(dev_t device, u_long command, void *data, int flags,
	      struct lwp *process)
{

	switch (command) {
		case LUNATIK_IOCTL:
			return lunatikL_runuserscript((lunatik_userparms_t *) data);

		default:
			return ENODEV;
	}

	return 0;
}

int
lunatikD_init(void)
{
	int bmajor = -1;
	int cmajor = -1;

	int error = devsw_attach("lunatik", NULL, &bmajor, &lunatik_cdevsw, &cmajor);
	printf("lunatik: cmajor = %d\n", cmajor);
	if (error != 0)
		return error;

	return 0;
}

int
lunatikD_fini(void)
{
	devsw_detach(NULL, &lunatik_cdevsw);

	return 0;
}

/* end: lunatik device */

