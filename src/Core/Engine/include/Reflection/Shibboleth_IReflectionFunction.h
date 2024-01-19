/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Shibboleth_ReflectionDefines.h"
#include <Gaff_IAllocator.h>
#include <Gaff_Flags.h>

namespace Shibboleth
{
	class ProxyAllocator;
}

NS_REFLECTION

class IEnumReflectionDefinition;
class IReflectionDefinition;

struct FunctionStackEntry final
{
	enum class Flag
	{
		IsReference,
		IsStringU8,
		IsString,
		IsArray,
		IsMap,

		Count
	};

	struct ArrayData final
	{
		void* data;
		int32_t size;
	};

	union Value final
	{
		ArrayData arr;
		void* vp;
		bool b;
		int8_t i8;
		int16_t i16;
		int32_t i32;
		int64_t i64;
		uint8_t u8;
		uint16_t u16;
		uint32_t u32;
		uint64_t u64;
		float f;
		double d;
	};

	FunctionStackEntry(void)
	{
		// Zero out everything.
		value.arr.data = nullptr;
		value.arr.size = 0;
	}

	const IEnumReflectionDefinition* enum_ref_def = nullptr;
	const IReflectionDefinition* ref_def = nullptr;
	Gaff::Flags<Flag> flags;
	Value value;
};

class IFunctionStackAllocator : public Gaff::IAllocator
{
public:
	using IAllocator::alloc;

	virtual void* alloc(const IReflectionDefinition& ref_def) = 0;
};


class IReflectionFunctionBase
{
public:
	IReflectionFunctionBase(void) {}
	virtual ~IReflectionFunctionBase(void) {}

	virtual bool callStack(const void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const = 0;
	virtual bool callStack(void* object, const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const = 0;

	virtual int32_t numArgs(void) const = 0;
	virtual bool isConst(void) const = 0;
	virtual bool isBase(void) const { return false; }
	virtual const IReflectionDefinition& getBaseRefDef(void) const = 0;
};

template <class Ret, class... Args>
class IReflectionFunction : public IReflectionFunctionBase
{
public:
	int32_t numArgs(void) const override
	{
		return static_cast<int32_t>(sizeof...(Args));
	}

	virtual Ret call(const void* object, Args&&... args) const = 0;
	virtual Ret call(void* object, Args&&... args) const = 0;
};

class IReflectionStaticFunctionBase
{
public:
	using VoidFunc = void (*)(void);

	explicit IReflectionStaticFunctionBase(VoidFunc func):
		_func(func)
	{
	}

	virtual ~IReflectionStaticFunctionBase(void) {}

	VoidFunc getFunc(void) const { return _func; }

	virtual bool callStack(const FunctionStackEntry* args, int32_t num_args, FunctionStackEntry& ret, IFunctionStackAllocator& allocator) const = 0;

	virtual int32_t numArgs(void) const = 0;
	virtual IReflectionStaticFunctionBase* clone(Shibboleth::ProxyAllocator& allocator) const = 0;

protected:
	VoidFunc _func = nullptr;
};

template <class Ret, class... Args>
class IReflectionStaticFunction : public IReflectionStaticFunctionBase
{
public:
	using Func = Ret (*)(Args...);

	explicit IReflectionStaticFunction(Func func):
		IReflectionStaticFunctionBase(reinterpret_cast<VoidFunc>(func))
	{
	}

	int32_t numArgs(void) const override
	{
		return static_cast<int32_t>(sizeof...(Args));
	}

	Func getFunc(void) const
	{
		return reinterpret_cast<Func>(_func);
	}

	Ret call(Args&&... args) const
	{
		return reinterpret_cast<Func>(_func)(std::forward<Args>(args)...);
	}
};

NS_END
