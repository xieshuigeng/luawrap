// luatable.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}
#include <stdarg.h>
#include <string.h>
#include <conio.h>

lua_State* luaInit(char* filename)
{
	lua_State* L = lua_open();
	luaopen_base(L);
	//luaopen_io(L);		//lua5.1 not use it!
	luaopen_string(L);
	luaopen_math(L);

	if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0))
	{
		printf("can not open file: %s", lua_tostring(L, -1));
	}

	return L;
}

void luaUninit(lua_State* L)
{
	lua_close(L);
}

int lua_callback(lua_State *state)
{
	printf("lua_callback ok!\n");
	return 1;
}

int _tmain(int argc, _TCHAR* argv[])
{
	lua_State* state = luaInit("table.lua");

	//get table
	printf("get table test:\n");
	lua_getglobal(state, "gettable");
	if (lua_pcall(state, 0, 1, 0) != 0)
	{
		printf("call function gettable error: %s\n", lua_tostring(state, -1));
	}

	int type = lua_type(state, -1);
	if (LUA_TTABLE != type)
	{
		printf("function return error!\n");
	}

	int top = lua_gettop(state);

	//lua_pushstring(state, "version");
	//lua_gettable(state, -2);
	lua_getfield(state, -1, "version");
	double version = luaL_checknumber(state, -1);
	lua_pop(state, 1);
	printf("version = %g\n", version);

	//lua_pushstring(state, "title");
	//lua_gettable(state, -2);
	lua_getfield(state, -1, "title");
	char* title = (char*)luaL_checkstring(state, -1);
	lua_pop(state, 1);
	printf("title = %s\n", title);
	
	//lua_pushstring(state, "func");
	//lua_gettable(state, -2);
	lua_getfield(state, -1, "func");
	printf("function pointer: 0x%08x\n", lua_topointer(state, -1));
	type = lua_type(state, -1);
	if (LUA_TFUNCTION == type)
	{
		lua_pushnumber(state, 1.0);
		lua_pushnumber(state, 2.0);
		if (lua_pcall(state, 2, 2, 0) != 0)
		{
			printf("call function hello error: %s\n", lua_tostring(state, -1));
		}
		printf("func return: %g, %s\n", lua_tonumber(state, -2), lua_tostring(state, -1));
	}
	lua_pop(state, 2);
	top = lua_gettop(state);

	//set table
	printf("\nset table test:\n");
	lua_getglobal(state, "settable");

	lua_newtable(state);
	
	lua_pushnumber(state, 1.0);
	lua_setfield(state, -2, "version");

	lua_pushstring(state, "set table test!");
	lua_setfield(state, -2, "title");

	lua_pushcclosure(state, lua_callback, 0);
	lua_setfield(state, -2, "callback");

	if (lua_pcall(state, 1, 1, 0) != 0)
	{
		printf("call function settable error: %s\n", lua_tostring(state, -1));
	}
	char* result = (char*)luaL_checkstring(state, -1);
	printf("call function settable : %s\n", result);

	lua_close(state);
	_getch();
	return 0;
}