#pragma once

//////////////////////////conversion helper//////////////////////////
template<class T>
void lua2c_conv(lua_State* state, int nargs, T* buf)
{
	if (buf)
		*buf = 0;
}

template<>
void lua2c_conv<int>(lua_State* state, int nargs, int* buf)
{
	int i = (int)luaL_checknumber(state, nargs);
	*buf = i;
}

template<>
void lua2c_conv<char*>(lua_State* state, int nargs, char** buf)
{
	char* s = (char*)luaL_checkstring(state, nargs);
	*buf = s;
}

template<>
void lua2c_conv<char const*>(lua_State* state, int nargs, char const ** buf)
{
	char* s = (char*)luaL_checkstring(state, nargs);
	*buf = s;
}

template<class T>
int c2lua_conv(lua_State* state, T value)
{
	int rt = 0;
	return rt;
}

template<>
int c2lua_conv<int>(lua_State* state, int value)
{
	lua_pushnumber(state, value);
	return 1;
}

template<>
int c2lua_conv<char*>(lua_State* state, char* value)
{
	lua_pushstring(state, value);
	return 1;
}