#pragma once

//////////////////////////function helper//////////////////////////
template<class T>
class FunctionHelperBase
{
public:
	FunctionHelperBase()
	{
	}

	virtual ~FunctionHelperBase()
	{
	}

	virtual int operator()(lua_State*, T*)
	{
		return 0;
	}
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
	}

	virtual ~FunctionHelper0()
	{
	}

	virtual int operator()(lua_State* state, T* obj)
	{
		int i = c2lua_conv(state, (obj->*callee)());
		return i;
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
	}

	virtual ~FunctionHelper0()
	{
	}

	virtual int operator()(lua_State* state, T* obj)
	{
		(obj->*callee)();
		return 0;
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
	}

	virtual ~FunctionHelper1()
	{
	}

	virtual int operator()(lua_State* state, T* obj)
	{
		A1 buf = 0;
		lua2c_conv(state, 0, &buf);
		int i = c2lua_conv(state, (obj->*callee)((A1)buf));
		return i;
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
	}

	virtual ~FunctionHelper2()
	{
	}

	virtual int operator()(lua_State* state, T* obj)
	{
		int nargs = 2;
		A1 buf = 0;
		lua2c_conv(state, --nargs, &buf);

		A2 buf2 = 0;
		lua2c_conv(state, --nargs, &buf2);

		int i = c2lua_conv(state, (obj->*callee)((A1)buf, (A2)(buf2)));
		return i;
	}
};

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
