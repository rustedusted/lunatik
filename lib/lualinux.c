/*
* Copyright (c) 2023 ring-0 Ltda.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <linux/module.h>
#include <linux/random.h>
#include <linux/stat.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <lunatik.h>

/* based on math_random() @ lua/lmathlib.c */
static int lualinux_random(lua_State *L)
{
	lua_Integer low, up, rand;

	switch (lua_gettop(L)) {  /* check number of arguments */
	case 0: {  /* no arguments */
#ifdef __LP64__
		lua_pushinteger(L, (lua_Integer)get_random_u64());
#else
		lua_pushinteger(L, (lua_Integer)get_random_u32());
#endif
		return 1;
	}
	case 1: {  /* only upper limit */
		low = 1;
		up = luaL_checkinteger(L, 1);
		break;
	}
	case 2: {  /* lower and upper limits */
		low = luaL_checkinteger(L, 1);
		up = luaL_checkinteger(L, 2);
		break;
	}
	default:
		return luaL_error(L, "wrong number of arguments");
	}
 
	/* random integer in the interval [low, up] */
	luaL_argcheck(L, low <= up, 1, "interval is empty");
	luaL_argcheck(L, low >= 0 || up <= LUA_MAXINTEGER + low, 1, "interval too large");

	rand = low + get_random_u32() % (up - low + 1);
	lua_pushinteger(L, rand);
	return 1;
}

static const lunatik_reg_t lualinux_stat[] = {
	/* user */
	{"IRWXU", S_IRWXU},
	{"IRUSR", S_IRUSR},
	{"IWUSR", S_IWUSR},
	{"IXUSR", S_IXUSR},
	/* group */
	{"IRWXG", S_IRWXG},
	{"IRGRP", S_IRGRP},
	{"IWGRP", S_IWGRP},
	{"IXGRP", S_IXGRP},
	/* other */
	{"IRWXO", S_IRWXO},
	{"IROTH", S_IROTH},
	{"IWOTH", S_IWOTH},
	{"IXOTH", S_IXOTH},
	/* user, group, other */
	{"IRWXUGO", (S_IRWXU|S_IRWXG|S_IRWXO)},
	{"IALLUGO", (S_ISUID|S_ISGID|S_ISVTX|S_IRWXUGO)},
	{"IRUGO", (S_IRUSR|S_IRGRP|S_IROTH)},
	{"IWUGO", (S_IWUSR|S_IWGRP|S_IWOTH)},
	{"IXUGO", (S_IXUSR|S_IXGRP|S_IXOTH)},
	{NULL, 0}
};

static const lunatik_namespace_t lualinux_flags[] = {
	{"stat", lualinux_stat},
	{NULL, NULL}
};

static const luaL_Reg lualinux_lib[] = {
	{"random", lualinux_random},
	{NULL, NULL}
};

static const lunatik_class_t lualinux_class = {
	.sleep = false,
};

LUNATIK_NEWLIB(linux, lualinux_lib, &lualinux_class, lualinux_flags);

static int __init lualinux_init(void)
{
	return 0;
}

static void __exit lualinux_exit(void)
{
}

module_init(lualinux_init);
module_exit(lualinux_exit);
MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Lourival Vieira Neto <lourival.neto@ring-0.io>");

