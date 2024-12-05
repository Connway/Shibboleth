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
#include "Shibboleth_ReflectionMacros.h"
#include <Gaff_Assert.h>
#include <Gaff_Utils.h>
#include <Gaff_Hash.h>
#include <Gaff_Ops.h>

namespace Shibboleth
{
	class ISerializeReader;
	class ISerializeWriter;
}

NS_REFLECTION

struct FunctionStackEntry;

template <class T>
class ReflectionVersionEnum final
{
public:
	template <class... Attrs>
	ReflectionVersionEnum& enumAttrs(const Attrs&... attributes);

	template <size_t name_size, class... Attrs>
	ReflectionVersionEnum& entry(const char8_t (&name)[name_size], T value);

	template <size_t name_size, class... Attrs>
	ReflectionVersionEnum& entry(const char (&name)[name_size], T value);

	void finish(void);

	Gaff::Hash64 getHash(void) const;

private:
	Gaff::Hash64 _hash = Gaff::k_init_hash64;

	template <class First, class... Rest>
	Gaff::Hash64 getAttributeHashes(Gaff::Hash64 hash, const First& first, const Rest&... rest) const;
};

template <class T>
class ReflectionVersionClass final
{
public:
	using LoadFunc = bool (*)(const Shibboleth::ISerializeReader&, T&);
	using SaveFunc = void (*)(Shibboleth::ISerializeWriter&, const T&);
	using InstanceHashFunc = Gaff::Hash64 (*)(const T&, Gaff::Hash64);

	template <class Base>
	ReflectionVersionClass& base(const char8_t* name);

	template <class Base>
	ReflectionVersionClass& base(void);

	template <class... Args>
	ReflectionVersionClass& ctor(Gaff::Hash64 factory_hash);

	template <class... Args>
	ReflectionVersionClass& ctor(void);

	template <class Var, size_t name_size, class... Attrs>
	ReflectionVersionClass& var(const char8_t (&name)[name_size], Var T::*ptr, const Attrs&... attributes);

	template <class Var, size_t name_size, class... Attrs>
	ReflectionVersionClass& var(const char (&name)[name_size], Var T::*ptr, const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionVersionClass& varFunc(const char8_t (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionVersionClass& varFunc(const char (&name)[name_size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char8_t (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char (&name)[name_size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char8_t (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char (&name)[name_size], Ret (T::*ptr)(Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char8_t (&name)[name_size], Ret (*ptr)(const T&, Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char (&name)[name_size], Ret (*ptr)(const T&, Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char8_t (&name)[name_size], Ret (*ptr)(T&, Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char (&name)[name_size], Ret (*ptr)(T&, Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& staticFunc(const char8_t (&name)[name_size], Ret (*func)(Args...), const Attrs&... attributes);

	template <size_t name_size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& staticFunc(const char (&name)[name_size], Ret (*func)(Args...), const Attrs&... attributes);


	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opAdd(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opSub(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opMul(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opDiv(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opMod(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opBitAnd(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opBitOr(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opBitXor(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionVersionClass& opBitShiftLeft(const Attrs&... attributes);
	template <class Other, class... Attrs>
	ReflectionVersionClass& opBitShiftRight(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opAnd(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opOr(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opEqual(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opNotEqual(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opLessThan(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opGreaterThan(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opLessThanOrEqual(const Attrs&... attributes);
	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opGreaterThanOrEqual(const Attrs&... attributes);

	template <bool is_const, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& opCall(const Attrs&... attributes);

	template <class Ret, class Other, class... Attrs>
	ReflectionVersionClass& opIndex(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionVersionClass& opBitNot(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionVersionClass& opNegate(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionVersionClass& opMinus(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionVersionClass& opPlus(const Attrs&... attributes);

	template <int32_t (*to_string_func)(const T&, char8_t*, int32_t), class... Attrs>
	ReflectionVersionClass& opToString(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opComparison(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionVersionClass& opPreIncrement(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionVersionClass& opPostIncrement(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionVersionClass& opPreDecrement(const Attrs&... attributes);

	template <class... Attrs>
	ReflectionVersionClass& opPostDecrement(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opAssignment(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opAddAssignment(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opSubAssignment(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opMulAssignment(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opDivAssignment(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opModAssignment(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opBitAndAssignment(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opBitOrAssignment(const Attrs&... attributes);

	template <class Other = T, class... Attrs>
	ReflectionVersionClass& opBitXorAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionVersionClass& opBitShiftLeftAssignment(const Attrs&... attributes);

	template <class Other, class... Attrs>
	ReflectionVersionClass& opBitShiftRightAssignment(const Attrs&... attributes);


	template <class... Attrs>
	ReflectionVersionClass& classAttrs(const Attrs&... attributes);

	ReflectionVersionClass& version(uint32_t version);

	ReflectionVersionClass& serialize(LoadFunc serialize_load, SaveFunc serialize_save = nullptr);
	ReflectionVersionClass& setInstanceHash(InstanceHashFunc hash_func);

	Gaff::Hash64 getHash(void) const;

	void finish(void);

	ReflectionVersionClass& baseOnlyStart(void);
	ReflectionVersionClass& baseOnlyEnd(void);

private:
	Gaff::Hash64 _hash = Gaff::k_init_hash64;

	template <class First, class... Rest>
	Gaff::Hash64 getAttributeHashes(Gaff::Hash64 hash, const First& first, const Rest&... rest) const;
};

template <class T, bool is_enum>
struct ReflectionVersionHelper;

template <class T>
struct ReflectionVersionHelper<T, true> final
{
	using Type = ReflectionVersionEnum<T>;
};

template <class T>
struct ReflectionVersionHelper<T, false> final
{
	using Type = ReflectionVersionClass<T>;
};

template <class T>
using ReflectionVersion = typename ReflectionVersionHelper<T, std::is_enum<T>::value>::Type;

#define REF_VER_BUILTIN(type) \
	template <> \
	class ReflectionVersionClass<type> final \
	{ \
	public: \
		Gaff::Hash64 getHash(void) const { return Gaff::FNV1aHash64Const(#type); } \
		void finish(void) {} \
	}

REF_VER_BUILTIN(int8_t);
REF_VER_BUILTIN(int16_t);
REF_VER_BUILTIN(int32_t);
REF_VER_BUILTIN(int64_t);
REF_VER_BUILTIN(uint8_t);
REF_VER_BUILTIN(uint16_t);
REF_VER_BUILTIN(uint32_t);
REF_VER_BUILTIN(uint64_t);
REF_VER_BUILTIN(float);
REF_VER_BUILTIN(double);
REF_VER_BUILTIN(bool);

NS_END

#include "Shibboleth_ReflectionVersion.inl"
