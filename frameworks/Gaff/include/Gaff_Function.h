/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#pragma once

#include "Gaff_Assert.h"
#include "Gaff_Pair.h"
#include <cstring>
#include <tuple>
#include <new>

#define FUNCTION_BUFFER_SIZE 256

NS_GAFF

template <class ReturnType, class... Args>
class IFunction
{
public:
	typedef IFunction<ReturnType, Args...> Func;

	IFunction(void) {}
	virtual ~IFunction(void) {}

	ReturnType operator()(Args... args)
	{
		return call(args...);
	}

	operator bool() const
	{
		return valid();
	}

	virtual ReturnType call(Args... args) = 0;
	virtual void copy(char* buffer) const = 0;
	virtual bool valid(void) const = 0;

};

#ifdef PLATFORM_WINDOWS
template <class ReturnType, class... Args>
class STDCallFunction : public IFunction<ReturnType, Args...>
{
public:
	typedef ReturnType (__stdcall *FunctionType)(Args...);
	typedef STDCallFunction<ReturnType, Args...> Func;

	STDCallFunction(FunctionType function = nullptr);
	STDCallFunction(const Func& function);

	const Func& operator=(const Func& rhs);
	const Func& operator=(FunctionType rhs);

	bool operator==(const Func& rhs) const;
	bool operator!=(const Func& rhs) const;
	bool operator==(FunctionType rhs) const;
	bool operator!=(FunctionType rhs) const;

	void set(FunctionType function);
	FunctionType get(void) const;

	ReturnType call(Args... args);
	void copy(char* buffer) const;
	bool valid(void) const;

private:
	FunctionType _function;
};
#endif

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

	ReturnType call(Args... args);
	void copy(char* buffer) const;
	bool valid(void) const;

private:
	FunctionType _function;
};

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

	ReturnType call(Args... args);
	void copy(char* buffer) const;
	bool valid(void) const;

private:
	FunctionType _function;
	T* _object;
};

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

	ReturnType call(Args... args);
	void copy(char* buffer) const;
	bool valid(void) const;

private:
	T _functor;
};

template <class ReturnType, class... Args>
class FunctionBinder
{
public:
	typedef FunctionBinder<ReturnType, Args...> Binder;
	typedef IFunction<ReturnType, Args...> IFunc;

	FunctionBinder(const Binder& rhs);
	FunctionBinder(void);
	~FunctionBinder(void);

	ReturnType operator()(Args... args);

	const Binder& operator=(const Binder& rhs);
	bool operator==(const Binder& rhs) const;
	bool operator!=(const Binder& rhs) const;

	operator bool(void) const;
	bool valid(void) const;

	const IFunc& getInterface(void) const;
	IFunc& getInterface(void);

private:
	char _function_buffer[FUNCTION_BUFFER_SIZE]; // allocate 32 bytes for the function buffer
	size_t _object_size;

	FunctionBinder(void* object, size_t size);

	template <class T, class RT, class... As> friend FunctionBinder<RT, As...> Bind(T*, RT (T::*)(As...));
	template <class RT, class... As> friend FunctionBinder<RT, As...> Bind(RT (*)(As...));
	template <class T, class RT, class... As> friend FunctionBinder<RT, As...> Bind(const T&);
	template <class T, class... As> friend T* Construct(T*, As&&...);

#ifdef PLATFORM_WINDOWS
	template <class RT, class... As> friend FunctionBinder<RT, As...> BindSTDCall(RT (__stdcall*)(As...));
#endif
};

#ifdef PLATFORM_WINDOWS
	template <class ReturnType, class... Args>
	FunctionBinder<ReturnType, Args...> BindSTDCall(ReturnType(__stdcall *function)(Args...));
#endif

template <class T, class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...> Bind(T* object, ReturnType (T::*function)(Args...));

template <class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...> Bind(ReturnType (*function)(Args...));

template <class T, class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...> Bind(const T& functor);

#include "Gaff_Function.inl"

NS_END
