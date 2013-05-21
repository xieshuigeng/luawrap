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
#include "overloading.h"


typedef struct __gfunctable
{
	char* name;
	SFunctionHelperBase* func;
}GFTable;
static vector<GFTable*> g_sFunctable;

template <class Fn>
void def(char* name, Fn f)
{
	SFunctionHelperBase *fn = getFunctionHelper(f);
	GFTable *l = new GFTable;

	l->name = new char[strlen(name)+1];
	strcpy_s(l->name, strlen(name)+1, name);
	l->func = fn;
	g_sFunctable.push_back(l);
}

int trunk(lua_State* state)
{
	int rt = 0;

	GFTable *l = static_cast<GFTable*>(lua_touserdata(state, lua_upvalueindex(1)));
	rt = (*l->func)(state);
	return rt;
}

void reg(lua_State* state)
{
	vector<GFTable*>::iterator iter = g_sFunctable.begin();
	for (; iter != g_sFunctable.end(); iter++)
	{
		GFTable *l = *iter;
		lua_pushlightuserdata(state, (void*)l);
		lua_pushcclosure(state, trunk, 1);
		lua_setglobal(state, l->name);
	}
}


#define DEF_LUACLASS(name)	\
	char RegisterHelper<name>::className[256]= #name;	\
	vector<RegisterHelper<name>::FTable_T*> RegisterHelper<name>::g_functable;	\
	vector<RegisterHelper<name>::SFTable_T*> RegisterHelper<name>::g_sfunctable; \
	RegisterHelper<name>* RegisterHelper<name>::g_Instance = NULL;	\
	template<>												\
	struct __lua_type_traits<name>									\
	{																\
		typedef __true_type_traits is_userdata;						\
	};																\
	void reg_##name(lua_State* state)											\

#define REG_LUACLASS(name, state)	\
	reg_##name(state)


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

DEF_LUACLASS(lua_A)
{
	int (lua_A::*f1)(const char*) = &lua_A::hello;
	RegisterHelper<lua_A>::Instance()->def("hello", f1);

	int (lua_A::*f2)(const char*, const char*) = &lua_A::hello;
	RegisterHelper<lua_A>::Instance()->def("hello", f2);

	int (lua_A::*f3)(const char*, int) = &lua_A::hello;
	RegisterHelper<lua_A>::Instance()->def("hello", f3);

	RegisterHelper<lua_A>::Instance()->def("cool", &lua_A::cool);
	RegisterHelper<lua_A>::Instance()->def("hellovoid", &lua_A::hellovoid);

	void (*f4)(void) = &lua_A::hellostatic;
	RegisterHelper<lua_A>::Instance()->defs("hellostatic", f4);

	lua_A* (*f5)(void) = &lua_A::createlua_A;
	RegisterHelper<lua_A>::Instance()->defs("createlua_A", f5);

	void (*f6)(lua_A*) = &lua_A::deletelua_A;
	RegisterHelper<lua_A>::Instance()->defs("deletelua_A", f6);

	RegisterHelper<lua_A>::Instance()->reg_T(state);
}

void global_hello()
{
	printf("global hello!\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	lua_State *state = luaInit(NULL);

	def("global_hello", global_hello);
	reg(state);
	REG_LUACLASS(lua_A, state);

	if (luaL_loadfile(state, "table3.lua") || lua_pcall(state, 0, 0, 0))
	{
		printf("can not open file: %s", lua_tostring(state, -1));
	}

	luaUninit(state);
	_getch();
	return 0;
}

