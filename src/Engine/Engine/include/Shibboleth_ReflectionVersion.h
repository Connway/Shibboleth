/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

	template <size_t size, class... Attrs>
	ReflectionVersionEnum& entry(const char8_t (&name)[size], T value);

	template <size_t size, class... Attrs>
	ReflectionVersionEnum& entry(const char (&name)[size], T value);

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
	using StackCtorFunc = void (*)(void*, const FunctionStackEntry*, int32_t);

	ReflectionVersionClass& friendlyName(const char8_t* name);

	template <class Base>
	ReflectionVersionClass& base(const char8_t* name);

	template <class Base>
	ReflectionVersionClass& base(void);

	ReflectionVersionClass& stackCtor(StackCtorFunc func);

	template <class... Args>
	ReflectionVersionClass& ctor(Gaff::Hash64 factory_hash);

	template <class... Args>
	ReflectionVersionClass& ctor(void);

	template <class Var, size_t size, class... Attrs>
	ReflectionVersionClass& var(const char8_t (&name)[size], Var T::*ptr, const Attrs&... attributes);

	template <class Var, size_t size, class... Attrs>
	ReflectionVersionClass& var(const char (&name)[size], Var T::*ptr, const Attrs&... attributes);

	template <class Ret, class Var, size_t size, class... Attrs>
	ReflectionVersionClass& var(const char8_t (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes);

	template <class Ret, class Var, size_t size, class... Attrs>
	ReflectionVersionClass& var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionVersionClass& var(const char8_t (&name)[name_size], Ret (*getter)(const T&), void (*setter)(T&, Var), const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionVersionClass& var(const char (&name)[name_size], Ret (*getter)(const T&), void (*setter)(T&, Var), const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char8_t (&name)[size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char (&name)[size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char8_t (&name)[size], Ret (T::*ptr)(Args...), const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& func(const char (&name)[size], Ret (T::*ptr)(Args...), const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& staticFunc(const char8_t (&name)[size], Ret (*func)(Args...), const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionVersionClass& staticFunc(const char (&name)[size], Ret (*func)(Args...), const Attrs&... attributes);

	template <class Other>
	ReflectionVersionClass& opAdd(void);
	template <class Other>
	ReflectionVersionClass& opSub(void);
	template <class Other>
	ReflectionVersionClass& opMul(void);
	template <class Other>
	ReflectionVersionClass& opDiv(void);
	template <class Other>
	ReflectionVersionClass& opMod(void);

	template <class Other>
	ReflectionVersionClass& opBitAnd(void);
	template <class Other>
	ReflectionVersionClass& opBitOr(void);
	template <class Other>
	ReflectionVersionClass& opBitXor(void);
	template <class Other>
	ReflectionVersionClass& opBitShiftLeft(void);
	template <class Other>
	ReflectionVersionClass& opBitShiftRight(void);

	template <class Other>
	ReflectionVersionClass& opAnd(void);
	template <class Other>
	ReflectionVersionClass& opOr(void);

	template <class Other>
	ReflectionVersionClass& opEqual(void);
	template <class Other>
	ReflectionVersionClass& opLessThan(void);
	template <class Other>
	ReflectionVersionClass& opGreaterThan(void);
	template <class Other>
	ReflectionVersionClass& opLessThanOrEqual(void);
	template <class Other>
	ReflectionVersionClass& opGreaterThanOrEqual(void);

	template <class... Args>
	ReflectionVersionClass& opCall(void);

	template <class Other>
	ReflectionVersionClass& opIndex(void);

	ReflectionVersionClass& opAdd(void);
	ReflectionVersionClass& opSub(void);
	ReflectionVersionClass& opMul(void);
	ReflectionVersionClass& opDiv(void);
	ReflectionVersionClass& opMod(void);

	ReflectionVersionClass& opBitAnd(void);
	ReflectionVersionClass& opBitOr(void);
	ReflectionVersionClass& opBitXor(void);
	ReflectionVersionClass& opBitNot(void);
	ReflectionVersionClass& opBitShiftLeft(void);
	ReflectionVersionClass& opBitShiftRight(void);

	ReflectionVersionClass& opAnd(void);
	ReflectionVersionClass& opOr(void);

	ReflectionVersionClass& opEqual(void);
	ReflectionVersionClass& opLessThan(void);
	ReflectionVersionClass& opGreaterThan(void);
	ReflectionVersionClass& opLessThanOrEqual(void);
	ReflectionVersionClass& opGreaterThanOrEqual(void);

	ReflectionVersionClass& opMinus(void);
	ReflectionVersionClass& opPlus(void);

	template <int32_t (*to_string_func)(const T&, char8_t*, int32_t)>
	ReflectionVersionClass& opToString(void);

	template <class... Attrs>
	ReflectionVersionClass& classAttrs(const Attrs&... attributes);

	ReflectionVersionClass& version(uint32_t version);

	ReflectionVersionClass& serialize(LoadFunc serialize_load, SaveFunc serialize_save = nullptr);
	ReflectionVersionClass& setInstanceHash(InstanceHashFunc hash_func);

	Gaff::Hash64 getHash(void) const;

	void finish(void);

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
