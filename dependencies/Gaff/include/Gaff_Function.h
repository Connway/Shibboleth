/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

/*! \file */

#pragma once

#include "Gaff_IncludeAssert.h"
#include "Gaff_Pair.h"
#include <cstring>

#define FUNCTION_BUFFER_SIZE 32

NS_GAFF

/*!
	\brief Base class for all function bindings.
	\tparam ReturnType The function return type.
	\tparam Args The function argument types.
*/
template <class ReturnType, class... Args>
class IFunction
{
public:
	typedef IFunction<ReturnType, Args...> Func;

	IFunction(void) {}
	virtual ~IFunction(void) {}

	ReturnType operator()(Args... args) const
	{
		return call(args...);
	}

	ReturnType operator()(Args... args)
	{
		return call(args...);
	}

	operator bool() const
	{
		return valid();
	}

	virtual ReturnType call(Args... args) const = 0;
	virtual ReturnType call(Args... args) = 0;
	virtual bool valid(void) const = 0;
};

/*!
	\brief Function pointer binding implementation.
	\tparam ReturnType The function return type.
	\tparam Args The function argument types.
*/
template <class ReturnType, class... Args>
class Function : public IFunction<ReturnType, Args...>
{
public:
	typedef ReturnType (*FunctionType)(Args...);
	typedef Function<ReturnType, Args...> Func;

	Function(FunctionType function = nullptr);
	Function(const Func& function);

	const Func& operator=(const Func& rhs);
	const Func& operator=(FunctionType rhs);

	bool operator==(const Func& rhs) const;
	bool operator!=(const Func& rhs) const;
	bool operator==(FunctionType rhs) const;
	bool operator!=(FunctionType rhs) const;

	void set(FunctionType function);
	FunctionType get(void) const;

	ReturnType call(Args... args) const;
	ReturnType call(Args... args);
	bool valid(void) const;

private:
	FunctionType _function;
};

/*!
	\brief Member function pointer binding implementation.
	\tparam ReturnType The function return type.
	\tparam Args The function argument types.
*/
template <class T, class ReturnType, class... Args>
class MemberFunction : public IFunction<ReturnType, Args...>
{
public:
	typedef MemberFunction<T, ReturnType, Args...> Func;
	typedef ReturnType (T::*FunctionType)(Args...);

	MemberFunction(T* object = nullptr, FunctionType function = nullptr);
	MemberFunction(const Func& member_function);

	const Func& operator=(const Func& rhs);

	bool operator==(const Func& rhs) const;
	bool operator!=(const Func& rhs) const;

	T* getObject(void) const;
	FunctionType getFunction(void) const;

	void set(T* object, FunctionType function);
	Pair<T*, FunctionType> get(void) const;

	ReturnType call(Args... args) const;
	ReturnType call(Args... args);
	bool valid(void) const;

private:
	FunctionType _function;
	T* _object;
};

/*!
	\brief Functor binding implementation.
	\tparam ReturnType The function return type.
	\tparam Args The function argument types.
*/
template <class T, class ReturnType, class... Args>
class Functor : public IFunction<ReturnType, Args...>
{
public:
	typedef Functor<T, ReturnType, Args...> Func;

	Functor(const T& functor = T());

	const Func& operator=(const Func& rhs);
	const Func& operator=(const T& rhs);

	bool operator==(const Func& rhs) const;
	bool operator!=(const Func& rhs) const;
	bool operator==(const T& rhs) const;
	bool operator!=(const T& rhs) const;

	const T& getFunctor(void) const;
	T& getFunctor(void);

	ReturnType call(Args... args) const;
	ReturnType call(Args... args);
	bool valid(void) const;

private:
	T _functor;
};

/*!
	\brief Generic class that can bind all three function types.
	\tparam ReturnType The function return type.
	\tparam Args The function argument types.
*/
template <class ReturnType, class... Args>
class FunctionBinder
{
public:
	typedef FunctionBinder<ReturnType, Args...> Binder;
	typedef IFunction<ReturnType, Args...> IFunc;

	FunctionBinder(const Binder& rhs);
	FunctionBinder(void);
	~FunctionBinder(void);

	ReturnType operator()(Args... args) const;
	ReturnType operator()(Args... args);

	const Binder& operator=(const Binder& rhs);
	bool operator==(const Binder& rhs) const;
	bool operator!=(const Binder& rhs) const;

	operator bool(void) const;
	bool valid(void) const;

	const IFunc& GetInterface(void) const;
	IFunc& GetInterface(void);

private:
	char _function_buffer[FUNCTION_BUFFER_SIZE]; // allocate 32 bytes for the function buffer
	unsigned int _object_size;

	FunctionBinder(void* object, unsigned int size);

	template <class T, class RT, class... Ags> friend FunctionBinder<RT, Ags...> Bind(T*, RT (T::*)(Ags...));
	template <class RT, class... Ags> friend FunctionBinder<RT, Ags...> Bind(RT (*)(Ags...));
	template <class T, class RT, class... Ags> friend FunctionBinder<RT, Ags...> Bind(const T&);
};

/*!
	\brief Binds a member function pointer to a FunctionBinder.

	\tparam T The object type whose member function we are binding.
	\tparam ReturnType The function return type.
	\tparam Args The function argument types.

	\param object The object instance.
	\param function The member function we are binding.

	\return A FunctionBinder.
*/
template <class T, class ReturnType, class... Ags>
FunctionBinder<ReturnType, Ags...> Bind(T* object, ReturnType (T::*function)(Ags...));

/*!
	\brief Binds a function pointer to a FunctionBinder.

	\tparam ReturnType The function return type.
	\tparam Args The function argument types.

	\param function The function we are binding.

	\return A FunctionBinder.
*/
template <class ReturnType, class... Ags>
FunctionBinder<ReturnType, Ags...> Bind(ReturnType (*function)(Ags...));

/*!
	\brief Binds a functor to a FunctionBinder.

	\tparam T The functor type.
	\tparam ReturnType The function return type.
	\tparam Args The function argument types.

	\param functor The functor we are binding.

	\return A FunctionBinder.
*/
template <class T, class ReturnType, class... Ags>
FunctionBinder<ReturnType, Ags...> Bind(const T& functor);

#include "Gaff_Function.inl"

NS_END
