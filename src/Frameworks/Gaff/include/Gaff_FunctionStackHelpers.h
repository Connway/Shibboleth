/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Gaff_ReflectionInterfaces.h"

NS_GAFF

template <class T>
static bool CastNumberToType(const IReflectionDefinition& ref_def, const void* in, T& out);

template <class T>
static bool CastFloatToType(const IReflectionDefinition& ref_def, const void* in, T& out);

template <class T>
static bool CastUnsignedIntegerToType(const IReflectionDefinition& ref_def, const void* in, T& out);

template <class T>
static bool CastSignedIntegerToType(const IReflectionDefinition& ref_def, const void* in, T& out);

template <class T>
static bool CastIntegerToType(const IReflectionDefinition& ref_def, const void* in, T& out);

template <class T>
static bool CastNumberToType(const FunctionStackEntry& entry, T& out);

template <class T>
static bool CastFloatToType(const FunctionStackEntry& entry, T& out);

template <class T>
static bool CastUnsignedIntegerToType(const FunctionStackEntry& entry, T& out);

template <class T>
static bool CastSignedIntegerToType(const FunctionStackEntry& entry, T& out);

template <class T>
static bool CastIntegerToType(const FunctionStackEntry& entry, T& out);


class IFunctionStackAllocator : public IAllocator
{
public:
	using IAllocator::alloc;

	virtual void* alloc(const Gaff::IReflectionDefinition& ref_def) = 0;
};

template <class Callable, class Allocator, class Ret, class First, class... Rest, class... CurrentArgs>
bool CallFunc(
	const Callable& callable,
	void* object,
	const FunctionStackEntry* args, 
	FunctionStackEntry& ret,
	int32_t arg_index,
	IFunctionStackAllocator& allocator,
	CurrentArgs&&... current_args
);

template <class Callable, class Allocator, class Ret, class... CurrentArgs>
bool CallFunc(
	const Callable& callable,
	void* object,
	FunctionStackEntry& ret,
	IFunctionStackAllocator& allocator,
	CurrentArgs&&... current_args
);

template <class Ret, class... Args, class... CurrentArgs>
Ret CallCallable(const IReflectionFunction<Ret, Args...>& func, void* object, CurrentArgs&&... current_args);

template <class Ret, class... Args, class... CurrentArgs>
Ret CallCallable(const IReflectionStaticFunction<Ret, Args...>& func, void*, CurrentArgs&&... current_args);

NS_END

// Not including here. Including in Gaff_ReflectionDefinition.inl.
//#include "Gaff_FunctionStackHelper.inl"
