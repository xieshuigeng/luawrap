#pragma once

//////////////////////////register helper//////////////////////////
template<typename T>
class RegisterHelper
{
	//typedef struct __ud
	//{
	//	T* pObj;
	//	bool bOrg;
	//}UD_T;

	typedef struct __functable
	{
		int nfunc;
		char* name;
		FunctionHelperBase<T>* func[32];
	}FTable_T;

	typedef struct __sfunctable
	{
		int nfunc;
		char* name;
		SFunctionHelperBase* func[32];
	}SFTable_T;

public:
	static char className[256];

private:
	static RegisterHelper<T>* g_Instance;	
	static vector<FTable_T*> g_functable;
	static vector<SFTable_T*> g_sfunctable;

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
			for (int i = 0; i < l->nfunc; i++)
				delete l->func[i];
			delete l;
		}
		g_functable.clear();

		for (size_t i = 0; i < g_sfunctable.size(); i++)
		{
			SFTable_T *l = g_sfunctable[i];
			delete []l->name;
			for (int i = 0; i < l->nfunc; i++)
				delete l->func[i];
			delete l;
		}
		g_sfunctable.clear();
	}

//	type check
	static T* check_T(lua_State* state, int narg)
	{
		void *p = lua_touserdata(state, narg);
		lua_getfield(state, LUA_REGISTRYINDEX, className);  /* get correct metatable */
		if (p == NULL || !lua_getmetatable(state, narg) || !lua_rawequal(state, -1, -2))
		{
			lua_pop(state, 1);

			lua_getfield(state, narg, "userdata");
			p = luaL_checkudata(state, -1, className);
			lua_pop(state, 1);
		}		
		else
		{
			lua_pop(state, 2);  /* remove both metatables */
		}
		UD_T<T> *ud = static_cast<UD_T<T>*>(p);
		
		//UD_T *ud = static_cast<UD_T*>(luaL_checkudata(state, narg, className));
		if(!ud) luaL_typerror(state, narg, className);
		return ud->pObj;
	}

//	default metamethod for metatable
	static int gc_T(lua_State* state)
	{
		UD_T<T> *ud = static_cast<UD_T<T>*>(lua_touserdata(state, 1));
		T *obj = ud->pObj;
		if (ud->bOrg)
			delete obj;
		return 0;
	}

	static int new_T(lua_State* state)
	{
		lua_remove(state, 1);

		int top = lua_gettop(state);
		T *obj = new T();

		UD_T<T> *ud = static_cast<UD_T<T>*>(lua_newuserdata(state, sizeof(UD_T<T>)));
		ud->pObj = obj;
		ud->bOrg = true;
		luaL_getmetatable(state, className);
		lua_setmetatable(state, -2);
		return 1;
	}

	static int tostring_T(lua_State* state)
	{
		char buff[32];
		UD_T<T> *ud = static_cast<UD_T<T>*>(lua_touserdata(state, 1));
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
		if (l->nfunc > 1)		//polymorphic
		{
			char sig[32] = "";
			int nargs = lua_gettop(state);			
			for (int i = 0; i < nargs; i++)
			{
				sig[i]  = LuaTypeSignal(state, i+1);
			}
						
			for (int nfunc = 0; nfunc < l->nfunc; nfunc++)
			{
				const char* sig1 = l->func[nfunc]->Signal();
				if (l->func[nfunc]->Args() == nargs && strcmp(sig, sig1) == 0)
				{
					rt = (*l->func[nfunc])(state, obj);
					break;
				}
			}
		}
		else
		{
			rt = (*l->func[0])(state, obj);
		}

		return rt;
	}

	static int thunk_TS(lua_State* state)
	{
		int rt = 0;
		lua_remove(state, 1);

		SFTable_T *l = static_cast<SFTable_T*>(lua_touserdata(state, lua_upvalueindex(1)));
		if (l->nfunc > 1)		//polymorphic
		{
			char sig[32] = "";
			int nargs = lua_gettop(state);
			for (int i = 0; i < nargs; i++)
			{
				sig[i]  = LuaTypeSignal(state, i+1);
			}
						
			for (int nfunc = 0; nfunc < l->nfunc; nfunc++)
			{
				const char* sig1 = l->func[nfunc]->Signal();
				if (l->func[nfunc]->Args() == nargs && strcmp(sig, sig1) == 0)
				{
					rt = (*l->func[nfunc])(state);
					break;
				}
			}
		}
		else
		{
			rt = (*l->func[0])(state);
		}

		return rt;
	}

//	define the function
	template <class Fn>
	void def(char* name, Fn f)
	{
		FTable_T* l = 0;
		if (g_functable.size() > 0)
		{
			for (size_t i = 0; i < g_functable.size(); i++)
			{
				FTable_T* ll = g_functable[i];
				if (strcmp(name, ll->name) == 0)
				{
					l = ll;
					break;
				}
			}
		}

		if (l)
		{
			FunctionHelperBase<T> *fn1 = getFunctionHelper(f);

			int* f1 = (int*)l->func[l->nfunc-1]->Pointer();
			int* f2 = (int*)fn1->Pointer();
			if (*f1 == *f2 && strcmp(l->func[l->nfunc-1]->Signal(), fn1->Signal()) == 0)
			{
				delete fn1;	//exist
			}
			else			//new
			{
				if (l->nfunc < 32)
				{
					l->func[l->nfunc] = fn1;
					l->nfunc ++;
				}
			}
		}
		else
		{
			FunctionHelperBase<T> *fn = getFunctionHelper(f);
			l = new FTable_T;

			l->name = new char[strlen(name)+1];
			strcpy_s(l->name, strlen(name)+1, name);
			l->nfunc = 1;
			l->func[0] = fn;

			g_functable.push_back(l);
		}
	}

//	define the function
	template <class Fn>
	void defs(char* name, Fn f)
	{
		SFTable_T* l = 0;
		if (g_sfunctable.size() > 0)
		{
			for (size_t i = 0; i < g_sfunctable.size(); i++)
			{
				SFTable_T* ll = g_sfunctable[i];
				if (strcmp(name, ll->name) == 0)
				{
					l = ll;
					break;
				}
			}
		}

		if (l)
		{
			SFunctionHelperBase *fn1 = getFunctionHelper(f);

			int* f1 = (int*)l->func[l->nfunc-1]->Pointer();
			int* f2 = (int*)fn1->Pointer();
			if (*f1 == *f2 && strcmp(l->func[l->nfunc-1]->Signal(), fn1->Signal()) == 0)
			{
				delete fn1;	//exist
			}
			else			//new
			{
				if (l->nfunc < 32)
				{
					l->func[l->nfunc] = fn1;
					l->nfunc ++;
				}
			}
		}
		else
		{
			SFunctionHelperBase *fn = getFunctionHelper(f);
			l = new SFTable_T;

			l->name = new char[strlen(name)+1];
			strcpy_s(l->name, strlen(name)+1, name);
			l->nfunc = 1;
			l->func[0] = fn;

			g_sfunctable.push_back(l);
		}
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

		vector<SFTable_T*>::iterator siter = g_sfunctable.begin();
		for (; siter != g_sfunctable.end(); siter++)
		{
			SFTable_T *l = *siter;
			lua_pushstring(state, l->name);
			lua_pushlightuserdata(state, (void*)l);
			lua_pushcclosure(state, thunk_TS, 1);
			lua_settable(state, methods);
		}
		lua_pop(state, 2);
	}
};
