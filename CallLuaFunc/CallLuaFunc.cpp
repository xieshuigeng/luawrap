// CallLuaFunc.cpp : Defines the entry point for the console application.
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

void callLuaFunc(lua_State* L, const char* func, const char* arg_sig, ...)
{
	va_list v1;
	int narg, nres;

	va_start(v1, arg_sig);

	lua_getglobal(L, func);				//get the global and push in stack
	narg = 0;
	while(*arg_sig)
	{
		bool bInputEnd = false;
		switch(*arg_sig)
		{
		case 'd':
			{
				lua_pushnumber(L, va_arg(v1, double));
			}
			break;

		case 'i':
			{
				lua_pushnumber(L, va_arg(v1, int));
			}
			break;

		case 's':
			{
				lua_pushstring(L, va_arg(v1, char*));
			}
			break;

		case '>':
			{
				bInputEnd = true;
			}
			break;

		default:
			{
				printf("invalid option (%c)", *(arg_sig));
			}
			break;
		}
		arg_sig++;
		if (bInputEnd)
			break;

		narg++;
		luaL_checkstack(L, 1, "too many arguments!\n");
	}

	nres = (int)strlen(arg_sig);
	if (lua_pcall(L, narg, nres, 0) != 0)
	{
		printf("call function %s error: %s\n", func, lua_tostring(L, -1));
	}

	nres = -nres;
	while(*arg_sig)
	{
		switch(*arg_sig)
		{
		case 'd':
			{
				if (!lua_isnumber(L, nres))
				{
					printf("wrong result type!\n");
				}
				else
					*va_arg(v1, double*) = lua_tonumber(L, nres);
			}
			break;

		case 'i':
			{
				if (!lua_isnumber(L, nres))
				{
					printf("wrong result type!\n");
				}
				else
					*va_arg(v1, int*) = (int)lua_tonumber(L, nres);
			}
			break;

		case 's':
			{
				if (!lua_isstring(L, nres))
				{
					printf("wrong result type!\n");
				}
				else
					*va_arg(v1, char**) = (char*)lua_tostring(L, nres);
			}
			break;

		default:
			printf("invalid option (%c)\n", arg_sig);
			break;
		}
		nres ++;
		arg_sig++;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	lua_State* state = luaInit("f.lua");
	double value = 0;
	char* szValue = NULL;
	callLuaFunc(state, "f", "dd>ds", 1.0, 3.1415926, &value, &szValue);
	printf("return value: %g, %s\n", value, szValue);
	lua_close(state);
	_getch();
	return 0;
}

