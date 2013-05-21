// luatable2.cpp : Defines the entry point for the console application.
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
#include "luatable3.h"

class lua_A
{
public:
	lua_A() {printf("lua_A()\n");}
	~lua_A() {printf("~lua_A()\n");}

public:
	int hello(const char* str)
	{
		printf("Hello %s!\n", str);
		return 0;
	}

	int cool(int value, const char* str)
	{
		printf("input: %d, %s\n", value, str);
		return 0;
	}

	void hellovoid()
	{
		printf("Hello Void!\n");
	}
};

lua_State* luaInit(char* filename)
{
	lua_State* L = lua_open();
	luaopen_base(L);
	luaopen_string(L);
	luaopen_math(L);

	if (filename && *filename)
	{
		if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0))
		{
			printf("can not open file: %s", lua_tostring(L, -1));
		}
	}

	return L;
}

void luaUninit(lua_State* L)
{
	lua_close(L);
}

#define INIT_LUACLASS(name)	\
	char RegisterHelper<name>::className[256]= #name;	\
	vector<RegisterHelper<name>::FTable_T*> RegisterHelper<name>::g_functable;	\
	RegisterHelper<name>* RegisterHelper<name>::g_Instance = NULL;	\
	void reg_##name(lua_State* state)											\


INIT_LUACLASS(lua_A)
{
	RegisterHelper<lua_A>::Instance()->def("hello", &lua_A::hello);
	RegisterHelper<lua_A>::Instance()->def("cool", &lua_A::cool);
	RegisterHelper<lua_A>::Instance()->def("hellovoid", &lua_A::hellovoid);
	RegisterHelper<lua_A>::Instance()->reg_T(state);
}

int _tmain(int argc, _TCHAR* argv[])
{
	lua_State *state = luaInit(NULL);

	reg_lua_A(state);

	if (luaL_loadfile(state, "table3.lua") || lua_pcall(state, 0, 0, 0))
	{
		printf("can not open file: %s", lua_tostring(state, -1));
	}

	luaUninit(state);
	_getch();
	return 0;
}

