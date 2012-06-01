#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/module.h>

#include "../lunatik.h"
#include "../lua/lauxlib.h"

MODULE(MODULE_CLASS_MISC, lunatik_test, "lunatik");

static lunatik_State *Lk;

static int errors = 0;
static int total  = 0;

static int case_count     = 0;
static int function_count = 0;

static void test_init(void);

static void test_exit(void);

static void test_newcase(const char *name);

static void test_closecase();

static void test_enterfunction(const char *name);

static void test_exitfunction(int ret, int expected);

static void test_error(const char *msg);

static void test_result(const char *result, const char *expected);

static void test_runstring(const char *, const char *, int, const char *);

static void lunatik_test(void);

static int luacfunction(lua_State *L);

static int lanswer(lua_State *L);

static int ladd(lua_State *L);

static int
lunatik_test_modcmd(modcmd_t cmd, void *arg)
{

	switch (cmd) {
	case MODULE_CMD_INIT:
		lunatik_test();
		break;

	case MODULE_CMD_FINI:
		break;

	case MODULE_CMD_STAT:
		return ENOTTY;

	default:
		return ENOTTY;
	}

	return 0;
}

typedef struct add_data{
	int a;
	int b;
	int result;
} add_data_t;

static void
lunatik_test(void)
{
	kmutex_t kmutex;
	int ret = -1;
	char *result = NULL;
	lua_State *L = NULL;
	size_t size = -1;
	add_data_t add_data;

	mutex_init(&kmutex, MUTEX_DEFAULT, IPL_NONE);

	test_init();

	/* test case */
	test_newcase("create new Lunatik state");
	test_enterfunction("lunatikL_newstate");
	Lk = lunatikL_newstate(KM_SLEEP, &kmutex, MUTEX_DEFAULT);
	test_exitfunction(0, 0);
	if (Lk == NULL){
		test_error("no memory");
		return;
	}
	test_closecase();

	/* test case */
	test_runstring("run a simple script", "return 42", 0, "42");

	/* test case */
	test_runstring("syntax error", "return + 42", 3, NULL);

	/* test case */
	{
	char *string = 
		"function fib(n) 			\
			if (n < 2) then			\
				return 1 		\
			end 				\
			return fib(n-2) + fib(n-1) 	\
		end"; 
	test_runstring("define the recursive fibonacci function", string, 0, NULL);
	}

	/* test case */
	test_runstring("call fib(3)", "return 2", 0, "2");

	/* test case */
	test_runstring("execution error", "return 1 + x", 2, NULL);

	/* test case */
	test_newcase("access Lua state directly");

	test_enterfunction("lunatik_getluastate");
	L = lunatik_getluastate(Lk);
	test_exitfunction(0, 0);
	if (L == NULL)	
		test_error("Lua state == NULL");
	else {
		test_enterfunction("lunatik_lock");
		lunatik_lock(Lk);
		test_exitfunction(0, 0);

		test_enterfunction("luaL_loadstring || lua_pcall");
		ret = luaL_loadstring(L, "x = fib(4) return x") || lua_pcall(L, 0, 1, 0);
		test_exitfunction(ret, 0);

		test_enterfunction("lua_tostring");
		result = (char *) lua_tostring(L, -1);
		test_exitfunction(0, 0);

		test_result(result, "5");
	
		test_enterfunction("lunatik_unlock");
		lunatik_unlock(Lk);
		test_exitfunction(0, 0);
	}
	test_closecase();

	/* test case */
	test_newcase("run a simple C function");
	test_enterfunction("lunatik_runcfunction");
	ret = lunatik_runcfunction(Lk, lanswer, NULL, &result, &size);
	test_exitfunction(ret, 0);

	if (result != NULL){
		test_result(result, "42");

		test_enterfunction("lunatik_free");
		lunatik_free(Lk, result, size);
		test_exitfunction(0, 0);
	}
	test_closecase();

	/* test case */
	test_newcase("run a C function with parms");

	add_data.a = 1007;
	add_data.b = 330;

	test_enterfunction("lunatik_runcfunction");
	ret = lunatik_runcfunction(Lk, ladd, &add_data, NULL, NULL);
	test_exitfunction(ret, 0);

	if (add_data.result != 1337)
		test_error("result error");
	test_closecase();

	/* test case */
	test_newcase("close Lunatik state");
	test_enterfunction("lunatik_close");
	lunatik_close(Lk);
	test_exitfunction(0, 0);
	test_closecase();

	test_exit();	
	mutex_destroy(&kmutex);
	return;
}

static void
test_runstring(const char *casemsg, const char *string, int expected_ret, 
	       const char *expected_result)
{
	char *result = NULL;
	size_t size  = -1;
	int ret = -1;

	test_newcase(casemsg);

	test_enterfunction("lunatik_runstring");
	ret = lunatik_runstring(Lk, string, &result, &size);
	test_exitfunction(ret, expected_ret);

	if (result != NULL){
		if (ret == 0 && expected_result != NULL)
				test_result(result, expected_result);

		test_enterfunction("lunatik_free");
		lunatik_free(Lk, result, size);
		test_exitfunction(0, 0);
	}
	test_closecase();
}

static void
test_init(void)
{
	printf("\n\nlktest: -- Lunatik test init --\n");

	errors = 0;
	total  = 0;

	case_count     = 0;
	function_count = 0;
}

static void
test_exit(void)
{
	printf("lktest: -- [ total errors = %d, total cases = %d, total functions = %d ]\n",
	      total, case_count, function_count);
	printf("lktest: -- Lunatik test exit --\n\n");
}

static void
test_newcase(const char *name)
{
	printf("lktest: -- new case (C%d) - \"%s\"\n", ++case_count, name);
}

static void
test_closecase()
{
	printf("lktest: -- close case (C%d)\t - [ errors = %d, total = %d ]\n\n", case_count, errors, total);
	errors = 0;
}

static void
test_enterfunction(const char *name)
{
	printf("lktest: --\t enter function (F%d) - \"%s\"\n", ++function_count, name);
}

static void
test_exitfunction(int ret, int expected)
{
	printf("lktest: --\t exit  function (F%d) - [ return = %d, expected = %d]\n",
		function_count, ret, expected);
	if (ret != expected){
		errors++;
		total++;
	}
}

static void
test_error(const char *msg)
{
	printf("lktest: --\t error - \"%s\"\n", msg);
	errors++;
	total++;
}

static void
test_result(const char *result, const char *expected)
{
	printf("lktest: --\t test result - [ result = \"%s\", expected = \"%s\"]\n",
		result, expected);
	if (strcmp(result, expected) != 0){
		errors++;
		total++;
	}
}

static int
lanswer(lua_State *L)
{
	lua_pushnumber(L, 42);
	
	return 1;
}

static int
ladd(lua_State *L)
{
	add_data_t *add_data = lua_touserdata(L, -1);

	add_data->result = add_data->a + add_data->b;
	
	return 0;
}

