#pragma once
extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

#include <string.h>
#include <vector>
using namespace std;

#include "conv.h"
#include "call_func.h"
#include "class_wrap.h"


class lua_A
{
public:
	lua_A() 
	{
		printf("lua_A()\n");
	}

	~lua_A() 
	{
		printf("~lua_A()\n");
	}

public:
	int hello(const char* str)
	{
		printf("Hello %s!\n", str);
		return 0;
	}

	int hello(const char* str, const char* str1)
	{
		printf("hello %s, %s!\n", str, str1);
		return 0;
	}
	
	int hello(const char* str, int value)
	{
		printf("hello %s, %d!\n", str, value);
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

	static void hellostatic()
	{
		printf("Hello Static Method!\n");
	}

	static lua_A* createlua_A()
	{
		printf("Hello createlua_A Method!\n");
		return new lua_A();
	}

	static void deletelua_A(lua_A* obj)
	{
		printf("Hello deletelua_A Method!\n");
		delete obj;
	}
};
