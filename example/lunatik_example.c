#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/module.h>

#include "../lunatik/lunatik.h"
#include "../lunatik/lua/lauxlib.h"

MODULE(MODULE_CLASS_MISC, lunatik_example, "lunatik");

static int lunatik_example_init(void);

static int lunatik_example_fini(void);

static kmutex_t kmutex;

static int
lunatik_example_modcmd(modcmd_t cmd, void *arg)
{

	switch (cmd) {
	case MODULE_CMD_INIT:
		return lunatik_example_init();

	case MODULE_CMD_FINI:
		return lunatik_example_fini();

	case MODULE_CMD_STAT:
		return ENOTTY;

	default:
		return ENOTTY;
	}

	return 0;
}

extern lunatik_State *lunatik_registry[];

static int
lunatik_example_init(void)
{
	char * result;
	size_t size;
	lunatik_State *Lk;

	mutex_init(&kmutex, MUTEX_DEFAULT, IPL_NONE);

	Lk = lunatikL_newstate(KM_SLEEP, &kmutex, MUTEX_DEFAULT);
	if (Lk == NULL)
		return ENOMEM;
	
	lunatik_registry[ 0 ] = Lk;

	return 0;
}

static int
lunatik_example_fini(void)
{

	lunatik_close(lunatik_registry[ 0 ]);
	mutex_destroy(&kmutex);
	lunatik_registry[ 0 ] = NULL;
	return 0;
}


