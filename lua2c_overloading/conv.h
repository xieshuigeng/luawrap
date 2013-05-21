#pragma once

struct __true_type_traits{};
struct __false_type_traits{};

template<class T>
struct __lua_type_traits
{
	typedef __false_type_traits is_userdata;
};

template<class T> struct UD_T
{
	T* pObj;
	bool bOrg;
};

//////////////////////////conversion helper//////////////////////////
template<class T>
void lua2c_conv(lua_State* state, int nargs, void* buf, T)
{
}

template<class T>
void lua2c_conv(lua_State* state, int nargs, void* buf, T*)
{
	if (buf)
	{
		T* obj = unwrap_luadata<T>(state, 1, __lua_type_traits<T>::is_userdata());
		memcpy(buf, &obj, sizeof(T*));
	}
}

template<>
void lua2c_conv<int>(lua_State* state, int nargs, void* buf, int)
{
	int i = (int)luaL_checknumber(state, nargs);
	memcpy(buf, &i, sizeof(int));
}

template<>
void lua2c_conv<char const*>(lua_State* state, int nargs, void* buf, char const *)
{
	char* s = (char*)luaL_checkstring(state, nargs);
	memcpy(buf, &s, sizeof(char*));
}

template<>
void lua2c_conv<char const>(lua_State* state, int nargs, void* buf, char const*)
{
	char* s = (char*)luaL_checkstring(state, nargs);
	memcpy(buf, &s, sizeof(char*));
}

template<class T> class RegisterHelper;
template<class T>
int wrap_luadata(lua_State* state, T* obj, __true_type_traits)
{
	UD_T<T> *ud = static_cast<UD_T<T>*>(lua_newuserdata(state, sizeof(UD_T<T>)));
	ud->pObj = obj;
	ud->bOrg = false;
	luaL_getmetatable(state, RegisterHelper<T>::className);
	lua_setmetatable(state, -2);
	return 1;
}

template<class T>
T* unwrap_luadata(lua_State* state, int narg, __true_type_traits)
{
	void *p = lua_touserdata(state, narg);
	lua_getfield(state, LUA_REGISTRYINDEX, RegisterHelper<T>::className);  /* get correct metatable */
	if (p == NULL || !lua_getmetatable(state, narg) || !lua_rawequal(state, -1, -2))
	{
		lua_pop(state, 1);

		lua_getfield(state, narg, "userdata");
		p = luaL_checkudata(state, -1, RegisterHelper<T>::className);
		lua_pop(state, 1);
	}		
	else
	{
		lua_pop(state, 2);  /* remove both metatables */
	}
	UD_T<T> *ud = static_cast<UD_T<T>*>(p);
	return ud->pObj;
}

template<class T>
int wrap_luadata(lua_State* state, T obj, __false_type_traits)
{
	lua_pushlightuserdata(state, (void*)obj);
	return 1;
}

template<class T>
T unwrap_luadata(lua_State* state, int narg, __false_type_traits)
{
	if (lua_islightuserdata(state, narg))
	{
		return lua_touserdata(state, narg);
	}
	return 0;
}

template<class T>
int c2lua_conv(lua_State* state, T value)
{
	int rt = wrap_luadata(state, &value, __lua_type_traits<T>::is_userdata());
	return rt;
}

template<class T>
int c2lua_conv(lua_State* state, T* value)
{
	int rt = wrap_luadata(state, value, __lua_type_traits<T>::is_userdata());
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