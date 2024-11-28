/************************************************************************************
Copyright (C) by Nicholas LaCroix

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
#include "Containers/Shibboleth_Vector.h"
#include "Shibboleth_String.h"
#include <Gaff_IAllocator.h>
#include <Gaff_Hashable.h>
#include <Gaff_Flags.h>

namespace Shibboleth
{
	class ProxyAllocator;
}

NS_REFLECTION

template <class T, class... Args>
using ConstructFuncT = void (*)(T*, Args&&...);

template <class... Args>
using ConstructFunc = void (*)(void*, Args&&...);

template <class T, class... Args>
using FactoryFuncT = T* (*)(Gaff::IAllocator&, Args&&...);

template <class... Args>
using FactoryFunc = void* (*)(Gaff::IAllocator&, Args&&...);

template <class Ret, class... Args>
using TemplateFunc = Ret (*)(Args...);

using VoidFunc = void (*)(void);


class IEnumReflectionDefinition;
class IReflectionDefinition;

struct FunctionArg final
{
	enum class Flag
	{
		Const,
		Reference,
		Pointer,

		Count
	};

	Shibboleth::U8String name{ REFLECTION_ALLOCATOR };
	Gaff::Flags<Flag> flags;

	bool isReference(void) const { return flags.testAll(Flag::Reference); }
	bool isPointer(void) const { return flags.testAll(Flag::Pointer); }
	bool isConst(void) const { return flags.testAll(Flag::Const); }

	Shibboleth::U8String getArgString(bool return_void_string) const
	{
		Shibboleth::U8String arg_str{ REFLECTION_ALLOCATOR };

		if (!name.data()) {
			if (return_void_string) {
				arg_str = u8"void";
			}

			return arg_str;
		}

		if (flags.testAll(Flag::Const)) {
			arg_str += u8"const ";
		}

		arg_str += name.data();

		if (Gaff::EndsWith(name.data(), u8"_t")) {
			arg_str.erase(arg_str.end() - 2, arg_str.end());
		}

		if (flags.testAll(Flag::Pointer)) {
			arg_str += u8'*';
		}

		if (flags.testAll(Flag::Reference)) {
			arg_str += u8'&';
		}

		return arg_str;
	}
};

struct FunctionSignature final
{
	enum class Flag
	{
		Const,

		Count
	};

	Shibboleth::Vector<FunctionArg> args{ REFLECTION_ALLOCATOR };
	FunctionArg return_value;
	Gaff::Flags<Flag> flags;

	bool isConst(void) const { return flags.testAll(Flag::Const); }

	Shibboleth::U8String getSignatureString(const Gaff::U8StringView& name = Gaff::U8StringView{}) const
	{
		Shibboleth::U8String sig_str{ REFLECTION_ALLOCATOR };

		sig_str = return_value.getArgString(true) + u8' ';

		if (name.data()) {
			sig_str += name.data();
		}

		sig_str += u8'(';

		for (const FunctionArg& arg : args) {
			sig_str += arg.getArgString(false);

			if (&arg != &args.back()) {
				sig_str += u8", ";
			}
		}

		sig_str += u8')';

		if (flags.testAll(Flag::Const)) {
			sig_str += u8" const";
		}

		return sig_str;
	}
};

template <class T>
FunctionArg GetFunctionArg(void)
{
	using NoPtr = typename std::remove_pointer<T>::type;
	using NoRef = typename std::remove_reference<NoPtr>::type;
	using V = typename std::remove_const<NoRef>::type;

	FunctionArg arg;

	if constexpr (std::is_const_v<NoRef>) {
		arg.flags.set(FunctionArg::Flag::Const);
	}

	if constexpr (std::is_pointer_v<NoPtr>) {
		arg.flags.set(FunctionArg::Flag::Pointer);
	}

	if constexpr (std::is_reference_v<NoPtr>) {
		arg.flags.set(FunctionArg::Flag::Reference);
	}

	if constexpr (!std::is_void_v<T>) {
		arg.name = Hash::ClassHashable<V>::GetName().data.data();
	}

	return arg;
}

template <class First, class... Rest>
void GetFunctionArgsHelper(Shibboleth::Vector<FunctionArg>& args)
{
	args.emplace_back(GetFunctionArg<First>());

	if constexpr (sizeof...(Rest) > 0) {
		GetFunctionArgsHelper<Rest...>(args);
	}
}

template <class... Args>
void GetFunctionArgs(Shibboleth::Vector<FunctionArg>& args)
{
	if constexpr (sizeof...(Args) > 0) {
		GetFunctionArgsHelper<Args...>(args);
	}
}


class IReflectionFunctionBase
{
public:
	IReflectionFunctionBase(void) {}
	virtual ~IReflectionFunctionBase(void) {}

	virtual int32_t numArgs(void) const = 0;
	virtual bool isConst(void) const = 0;

	virtual const void* getFunctionPointer(void) const = 0;
	virtual size_t getFunctionPointerSize(void) const = 0;

	virtual bool isExtensionFunction(void) const { return false; }

	virtual FunctionSignature getSignature(void) const = 0;
};

template <class Ret, class... Args>
class IReflectionFunction : public IReflectionFunctionBase
{
public:
	int32_t numArgs(void) const override final
	{
		return static_cast<int32_t>(sizeof...(Args));
	}

	virtual Ret call(const void* object, Args&&... args) const = 0;
	virtual Ret call(void* object, Args&&... args) const = 0;

	FunctionSignature getSignature(void) const override final
	{
		FunctionSignature signature;

		GetFunctionArgs<Args...>(signature.args);
		signature.return_value = GetFunctionArg<Ret>();

		if (isConst()) {
			signature.flags.set(FunctionSignature::Flag::Const);
		}

		return signature;
	}
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

	virtual int32_t numArgs(void) const = 0;
	virtual IReflectionStaticFunctionBase* clone(void) const = 0; // $TODO: Delete me.

	virtual FunctionSignature getSignature(void) const = 0;

protected:
	VoidFunc _func = nullptr;
};

template <class Ret, class... Args>
class ReflectionStaticFunction  final : public IReflectionStaticFunctionBase
{
public:
	using Func = Ret (*)(Args...);

	explicit ReflectionStaticFunction(Func func):
		IReflectionStaticFunctionBase(reinterpret_cast<VoidFunc>(func))
	{
	}

	int32_t numArgs(void) const override final
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

	FunctionSignature getSignature(void) const override
	{
		FunctionSignature signature;

		GetFunctionArgs<Args...>(signature.args);
		signature.return_value = GetFunctionArg<Ret>();

		return signature;
	}

	// $TODO: Delete me.
	IReflectionStaticFunctionBase* clone(void) const override
	{
		using Type = ReflectionStaticFunction<Ret, Args...>;
		return SHIB_ALLOCT(Type, REFLECTION_ALLOCATOR, getFunc());
	}

};

NS_END
