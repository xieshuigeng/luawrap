#pragma once

//////////////////////////type signal helper//////////////////////////
template<class T>
char TypeSignal(T*)
{
	return '0';
}

template<>
char TypeSignal<char*>(char**)
{
	return 's';
}

template<>
char TypeSignal<char const*>(char const **)
{
	return 's';
}

template<>
char TypeSignal<int>(int*)
{
	return 'd';
}

template<>
char TypeSignal<char>(char*)
{
	return 'd';
}

template<>
char TypeSignal<double>(double*)
{
	return 'd';
}

template<>
char TypeSignal<bool>(bool*)
{
	return 'b';
}

char LuaTypeSignal(lua_State* state, int nargs)
{
	char sig = '0';
	int t = lua_type(state, nargs);

	switch(t)
	{
	case LUA_TSTRING:
		{
			sig = 's';
		}
		break;

	case LUA_TBOOLEAN:
		{
			sig = 'b';
		}
		break;

	case LUA_TNUMBER:
		{
			sig = 'd';
		}
		break;

	default:
		break;
	}
	return sig;
}
//////////////////////////function helper//////////////////////////

template<class T>
class FunctionHelperBase
{
public:
	FunctionHelperBase()
	{
		m_nArgs = 0;
		memset(m_szSig, 0, sizeof(m_szSig));
	}

	virtual ~FunctionHelperBase()
	{
	}

	virtual int operator()(lua_State*, T*)
	{
		return 0;
	}

	virtual int Args()
	{
		return m_nArgs;
	}

	virtual const char* Signal()
	{
		return m_szSig;
	}

	virtual const void* Pointer()
	{
		return 0;
	}

protected:
	int		m_nArgs;
	char	m_szSig[32];
};

template<class T, class RT>
class FunctionHelper0 : public FunctionHelperBase<T>
{
	typedef RT(T::*Fn)();
	Fn callee;

public:
	FunctionHelper0(Fn f)
	{
		callee = f;
		m_nArgs = 0;
		m_szSig[0] = '\0';
	}

	virtual ~FunctionHelper0()
	{
	}

	virtual int operator()(lua_State* state, T* obj)
	{
		int i = c2lua_conv(state, (obj->*callee)());
		return i;
	}

	virtual const void* Pointer()
	{
		return &callee;
	}
};

template<class T>
class FunctionHelper0<T,void> : public FunctionHelperBase<T>
{
	typedef void(T::*Fn)();
	Fn callee;

public:
	FunctionHelper0(Fn f)
	{
		callee = f;
		m_nArgs = 0;
		m_szSig[0] = '\0';
	}

	virtual ~FunctionHelper0()
	{
	}

	virtual int operator()(lua_State* state, T* obj)
	{
		(obj->*callee)();
		return 0;
	}

	virtual const void* Pointer()
	{
		return &callee;
	}
};

template<class T, class RT, class A1>
class FunctionHelper1 : public FunctionHelperBase<T>
{
	typedef RT(T::*Fn)(A1);
	Fn callee;

public:
	FunctionHelper1(Fn f)
	{
		callee = f;
		m_nArgs = 1;

		A1 *sig = 0;
		m_szSig[0] = TypeSignal(sig);
		m_szSig[1] = '\0';
	}

	virtual ~FunctionHelper1()
	{
	}

	virtual int operator()(lua_State* state, T* obj)
	{
		A1 buf = 0;
		lua2c_conv(state, 0, &buf, buf);
		int i = c2lua_conv(state, (obj->*callee)((A1)buf));
		return i;
	}

	virtual const void* Pointer()
	{
		return &callee;
	}
};

template<class T, class RT, class A1, class A2>
class FunctionHelper2 : public FunctionHelperBase<T>
{
	typedef RT(T::*Fn)(A1, A2);
	Fn callee;

public:
	FunctionHelper2(Fn f)
	{
		callee = f;
		m_nArgs = 2;

		A1 *sig1 = 0;
		m_szSig[0] = TypeSignal(sig1);
		A2 *sig2 = 0;
		m_szSig[1] = TypeSignal(sig2);
		m_szSig[2] = '\0';
	}

	virtual ~FunctionHelper2()
	{
	}

	virtual int operator()(lua_State* state, T* obj)
	{
		A1 buf = 0;
		lua2c_conv(state, 1, &buf, buf);

		A2 buf2 = 0;
		lua2c_conv(state, 2, &buf2, buf2);

		int i = c2lua_conv(state, (obj->*callee)((A1)buf, (A2)(buf2)));
		return i;
	}

	virtual const void* Pointer()
	{
		return &callee;
	}
};

class SFunctionHelperBase
{
public:
	SFunctionHelperBase()
	{
		m_nArgs = 0;
		memset(m_szSig, 0, sizeof(m_szSig));
	}

	virtual ~SFunctionHelperBase()
	{
	}

	virtual int operator()(lua_State*)
	{
		return 0;
	}

	virtual int Args()
	{
		return m_nArgs;
	}

	virtual const char* Signal()
	{
		return m_szSig;
	}

	virtual const void* Pointer()
	{
		return 0;
	}

protected:
	int		m_nArgs;
	char	m_szSig[32];
};

template<class RT>
class SFunctionHelper0 : public SFunctionHelperBase
{
	typedef RT(*Fn)();
	Fn callee;
public:
	SFunctionHelper0(Fn f)
	{
		callee = f;
	}

	virtual ~SFunctionHelper0()
	{
	}

	virtual int operator()(lua_State* state)
	{
		c2lua_conv(state, (*callee)());
		return 1;
	}

	virtual int Args()
	{
		return m_nArgs;
	}

	virtual const char* Signal()
	{
		return m_szSig;
	}

	virtual const void* Pointer()
	{
		return &callee;
	}
};

template<>
class SFunctionHelper0<void> : public SFunctionHelperBase
{
	typedef void(*Fn)();
	Fn callee;
public:
	SFunctionHelper0(Fn f)
	{
		callee = f;
	}

	virtual ~SFunctionHelper0()
	{
	}

	virtual int operator()(lua_State*)
	{
		(*callee)();
		return 0;
	}

	virtual int Args()
	{
		return m_nArgs;
	}

	virtual const char* Signal()
	{
		return m_szSig;
	}

	virtual const void* Pointer()
	{
		return &callee;
	}
};

template<class RT, class A1>
class SFunctionHelper1 : public SFunctionHelperBase
{
	typedef RT(*Fn)(A1);
	Fn callee;
public:
	SFunctionHelper1(Fn f)
	{
		callee = f;
	}

	virtual ~SFunctionHelper1()
	{
	}

	virtual int operator()(lua_State* state)
	{
		A1 buf = 0;
		lua2c_conv(state, 1, buf);

		c2lua_conv(state, (*callee)(buf));
		return 1;
	}

	virtual int Args()
	{
		return m_nArgs;
	}

	virtual const char* Signal()
	{
		return m_szSig;
	}

	virtual const void* Pointer()
	{
		return &callee;
	}
};

template<class A1>
class SFunctionHelper1<void, A1> : public SFunctionHelperBase
{
	typedef void(*Fn)(A1);
	Fn callee;
public:
	SFunctionHelper1(Fn f)
	{
		callee = f;
	}

	virtual ~SFunctionHelper1()
	{
	}

	virtual int operator()(lua_State* state)
	{
		A1 buf = 0;
		lua2c_conv(state, 1, &buf, buf);

		(*callee)(buf);
		return 0;
	}

	virtual int Args()
	{
		return m_nArgs;
	}

	virtual const char* Signal()
	{
		return m_szSig;
	}

	virtual const void* Pointer()
	{
		return &callee;
	}
};

//////////////////////////function class helper//////////////////////////
template<class T, class RT>
FunctionHelper0<T, RT>* getFunctionHelper(RT(T::*f)())
{
	return new FunctionHelper0<T, RT>(f);
}

template<class T, class RT, class A1>
FunctionHelper1<T, RT, A1>* getFunctionHelper(RT(T::*f)(A1))
{
	return new FunctionHelper1<T, RT, A1>(f);
}

template<class T, class RT, class A1, class A2>
FunctionHelper2<T, RT, A1, A2>* getFunctionHelper(RT(T::*f)(A1, A2))
{
	return new FunctionHelper2<T, RT, A1, A2>(f);
}

template<class RT>
SFunctionHelper0<RT>* getFunctionHelper(RT(*f)())
{
	return new SFunctionHelper0<RT>(f);
}

template<class RT, class A1>
SFunctionHelper1<RT, A1>* getFunctionHelper(RT(*f)(A1))
{
	return new SFunctionHelper1<RT, A1>(f);
}