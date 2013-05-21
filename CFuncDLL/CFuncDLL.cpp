// CFuncDLL.cpp : Defines the entry point for the DLL application.
//
#define LUA_LIB
#include "stdafx.h"
extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}
#include <math.h>
#include <conio.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

int l_sin(lua_State* state)
{
	double d = luaL_checknumber(state, 1);
	lua_pushnumber(state, sin(d));
	return 1;
}

const struct luaL_reg mylib[] = 
{
	{"mysin", l_sin},
	{NULL, NULL}
};

extern "C" int __declspec(dllexport) luaopen_mylib (lua_State* state)
{
	luaL_openlib(state, "mylib", mylib, 0);
	return 1;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

