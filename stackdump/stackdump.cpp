// stackdump.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}
#include <stdio.h>
#include <conio.h>

void StackDump(lua_State* L)
{
	int i;
	int top = lua_gettop(L);

	for (i = 1; i <= top; i ++)
	{
		int t = lua_type(L, i);
		
		switch(t)
		{
		case LUA_TSTRING:
			{
				printf("\'%s\'", lua_tostring(L, i));
			}
			break;

		case LUA_TBOOLEAN:
			{
				printf(lua_toboolean(L, i) ? "true" : "false");
			}
			break;

		case LUA_TNUMBER:
			{
				printf("%g", lua_tonumber(L, i));
			}
			break;

		default:
			{
				printf("%s", lua_typename(L, i));
			}
			break;
		}

		printf("\n");
	}

	printf("\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	lua_State *L = lua_open();
	lua_pushboolean(L, 1);
	lua_pushnumber(L, 10);
	lua_pushnil(L);
	lua_pushstring(L, "Hello World!");
	StackDump(L);

	lua_pushvalue(L, -4);
	StackDump(L);

	lua_replace(L, 3);
	StackDump(L);

	lua_settop(L, 6);
	StackDump(L);

	lua_remove(L, -3);
	StackDump(L);

	lua_settop(L, -5);
	StackDump(L);


	lua_close(L);
	_getch();
	return 0;
}

