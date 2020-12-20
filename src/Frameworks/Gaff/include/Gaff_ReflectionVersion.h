/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
#include "Gaff_Assert.h"
#include "Gaff_Hash.h"
#include "Gaff_Ops.h"

NS_GAFF

class ISerializeReader;
class ISerializeWriter;
struct FunctionStackEntry;

template <class T>
class ReflectionVersion final
{
public:
	using LoadFunc = bool (*)(const ISerializeReader&, T&);
	using SaveFunc = void (*)(ISerializeWriter&, const T&);
	using InstanceHashFunc = Hash64 (*)(const T&, Hash64);
	using StackCtorFunc = void (*)(void*, const FunctionStackEntry*, int32_t);

	ReflectionVersion& friendlyName(const char* name);

	template <class Base>
	ReflectionVersion& base(const char* name);

	template <class Base>
	ReflectionVersion& base(void);

	ReflectionVersion& stackCtor(StackCtorFunc func);

	template <class... Args>
	ReflectionVersion& ctor(Hash64 factory_hash);

	template <class... Args>
	ReflectionVersion& ctor(void);

	template <class Var, size_t size, class... Attrs>
	ReflectionVersion& var(const char (&name)[size], Var T::*ptr, const Attrs&... attributes);

	template <class Ret, class Var, size_t size, class... Attrs>
	ReflectionVersion& var(const char (&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var), const Attrs&... attributes);

	template <class Ret, class Var, size_t name_size, class... Attrs>
	ReflectionVersion& var(const char (&name)[name_size], Ret (*getter)(const T&), void (*setter)(T&, Var), const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionVersion& func(const char (&name)[size], Ret (T::*ptr)(Args...) const, const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionVersion& func(const char (&name)[size], Ret (T::*ptr)(Args...), const Attrs&... attributes);

	template <size_t size, class Ret, class... Args, class... Attrs>
	ReflectionVersion& staticFunc(const char (&name)[size], Ret (*func)(Args...), const Attrs&... attributes);

	template <class Other>
	ReflectionVersion& opAdd(void);
	template <class Other>
	ReflectionVersion& opSub(void);
	template <class Other>
	ReflectionVersion& opMul(void);
	template <class Other>
	ReflectionVersion& opDiv(void);
	template <class Other>
	ReflectionVersion& opMod(void);

	template <class Other>
	ReflectionVersion& opBitAnd(void);
	template <class Other>
	ReflectionVersion& opBitOr(void);
	template <class Other>
	ReflectionVersion& opBitXor(void);
	template <class Other>
	ReflectionVersion& opBitShiftLeft(void);
	template <class Other>
	ReflectionVersion& opBitShiftRight(void);

	template <class Other>
	ReflectionVersion& opAnd(void);
	template <class Other>
	ReflectionVersion& opOr(void);

	template <class Other>
	ReflectionVersion& opEqual(void);
	template <class Other>
	ReflectionVersion& opLessThan(void);
	template <class Other>
	ReflectionVersion& opGreaterThan(void);
	template <class Other>
	ReflectionVersion& opLessThanOrEqual(void);
	template <class Other>
	ReflectionVersion& opGreaterThanOrEqual(void);

	template <class... Args>
	ReflectionVersion& opCall(void);

	template <class Other>
	ReflectionVersion& opIndex(void);

	ReflectionVersion& opAdd(void);
	ReflectionVersion& opSub(void);
	ReflectionVersion& opMul(void);
	ReflectionVersion& opDiv(void);
	ReflectionVersion& opMod(void);

	ReflectionVersion& opBitAnd(void);
	ReflectionVersion& opBitOr(void);
	ReflectionVersion& opBitXor(void);
	ReflectionVersion& opBitNot(void);
	ReflectionVersion& opBitShiftLeft(void);
	ReflectionVersion& opBitShiftRight(void);

	ReflectionVersion& opAnd(void);
	ReflectionVersion& opOr(void);

	ReflectionVersion& opEqual(void);
	ReflectionVersion& opLessThan(void);
	ReflectionVersion& opGreaterThan(void);
	ReflectionVersion& opLessThanOrEqual(void);
	ReflectionVersion& opGreaterThanOrEqual(void);

	ReflectionVersion& opMinus(void);
	ReflectionVersion& opPlus(void);

	template <int32_t (*to_string_func)(const T&, char*, int32_t)>
	ReflectionVersion& opToString(void);

	template <class... Attrs>
	ReflectionVersion& classAttrs(const Attrs&...);

	ReflectionVersion& version(uint32_t version);

	ReflectionVersion& serialize(LoadFunc serialize_load, SaveFunc serialize_save = nullptr);
	ReflectionVersion& setInstanceHash(InstanceHashFunc hash_func);

	// Enum
	template <size_t size, class... Attrs>
	ReflectionVersion& entry(const char(&name)[size], T value);

	Hash64 getHash(void) const;

	void finish(void);

private:
	Hash64 _hash = INIT_HASH64;

	template <class First, class... Rest>
	Hash64 getAttributeHashes(Hash64 hash, const First& first, const Rest&... rest) const;
};

#define REF_VER_BUILTIN(type) \
	template <> \
	class ReflectionVersion<type> final \
	{ \
	public: \
		Hash64 getHash(void) const { return Gaff::FNV1aHash64Const(#type); } \
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

#include "Gaff_ReflectionVersion.inl"
