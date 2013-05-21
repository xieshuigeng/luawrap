#pragma once

//////////////////////////register helper//////////////////////////
template<typename T>
class RegisterHelper
{
	typedef struct __ud
	{
		T* pObj;
	}UD_T;

	typedef struct __functable
	{
		char* name;
		FunctionHelperBase<T>* func;
	}FTable_T;

private:
	static RegisterHelper<T>* g_Instance;
	static char className[256];
	static vector<FTable_T*> g_functable;

	RegisterHelper(const char* name)
	{
	}

public:
	static RegisterHelper<T>* Instance()
	{
		if (!g_Instance)
		{
			g_Instance = new RegisterHelper("");
		}

		return g_Instance;
	}

	virtual ~RegisterHelper()
	{
		for (size_t i = 0; i < g_functable.size(); i++)
		{
			FTable_T *l = g_functable[i];
			delete []l->name;
			delete l->func;
			delete l;
		}
		g_functable.clear();
	}

//	type check
	static T* check_T(lua_State* state, int narg)
	{
		UD_T *ud = static_cast<UD_T*>(luaL_checkudata(state, narg, className));
		if(!ud) luaL_typerror(state, narg, className);
		return ud->pObj;
	}

//	default metamethod for metatable
	static int gc_T(lua_State* state)
	{
		UD_T *ud = static_cast<UD_T*>(lua_touserdata(state, 1));
		T *obj = ud->pObj;
		delete obj;
		return 0;
	}

	static int new_T(lua_State* state)
	{
		lua_remove(state, 1);
		T *obj = new T();		//new, how???!!!
		UD_T *ud = static_cast<UD_T*>(lua_newuserdata(state, sizeof(UD_T)));
		ud->pObj = obj;
		luaL_getmetatable(state, className);
		lua_setmetatable(state, -2);
		return 1;
	}

	static int tostring_T(lua_State* state)
	{
		char buff[32];
		UD_T *ud = static_cast<UD_T*>(lua_touserdata(state, 1));
		T *obj = ud->pObj;
		sprintf_s(buff, 32, "%p", obj);
		lua_pushfstring(state, "%s (%s)", className, buff);
		return 1;
	}

//	thunk
	static int thunk_T(lua_State* state)
	{
		int rt = 0;
		T *obj = check_T(state, 1);
		lua_remove(state, 1);

		FTable_T *l = static_cast<FTable_T*>(lua_touserdata(state, lua_upvalueindex(1)));
		rt = (*l->func)(state, obj);

		return rt;
	}

//	define the function
	template <class Fn>
	void def(char* name, Fn f)
	{
		FunctionHelperBase<T> *fn = getFunctionHelper(f);
		FTable_T* l = new FTable_T;

		l->name = new char[strlen(name)+1];
		strcpy_s(l->name, strlen(name)+1, name);
		l->func = fn;

		g_functable.push_back(l);
	}

//	register
	void reg_T(lua_State* state)
	{
		lua_newtable(state);
		int methods = lua_gettop(state);

		luaL_newmetatable(state, className);
		int metatable = lua_gettop(state);

		lua_pushstring(state, className);
		lua_pushvalue(state, methods);
		lua_settable(state, LUA_GLOBALSINDEX);

		lua_pushliteral(state, "__metatable");		//set the parent
		lua_pushvalue(state, methods);
		lua_settable(state, metatable);

		lua_pushliteral(state, "__index");
		lua_pushvalue(state, methods);
		lua_settable(state, metatable);

		lua_pushliteral(state, "__tostring");
		lua_pushcfunction(state, tostring_T);
		lua_settable(state, metatable);

		lua_pushliteral(state, "__gc");
		lua_pushcfunction(state, gc_T);
		lua_settable(state, metatable);

		lua_newtable(state);
		int mt = lua_gettop(state);
		lua_pushliteral(state, "__call");
		lua_pushcfunction(state, new_T);
		lua_pushliteral(state, "new");
		lua_pushvalue(state, -2);
		lua_settable(state, methods);
		lua_settable(state, mt);
		lua_setmetatable(state, methods);

		vector<FTable_T*>::iterator iter = g_functable.begin();
		for (; iter != g_functable.end(); iter++)
		{
			FTable_T *l = *iter;
			lua_pushstring(state, l->name);
			lua_pushlightuserdata(state, (void*)l);
			lua_pushcclosure(state, thunk_T, 1);
			lua_settable(state, methods);
		}

		lua_pop(state, 2);
	}
};
