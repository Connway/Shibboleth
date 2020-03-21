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

#ifndef GAFF_REFLECTION_NAMESPACE
	#define GAFF_REFLECTION_NAMESPACE Shibboleth
#endif

#ifndef GAFF_REF_DEF_IS_PTR
	#define GAFF_REF_DEF_IS_PTR
#endif

#ifndef GAFF_REF_DEF_INIT
	#define GAFF_REF_DEF_INIT \
		GAFF_ASSERT(!g_ref_def); \
		const typename Gaff::RefDefInterface<ThisType, ProxyAllocator>* ref_def_interface = nullptr; \
		if constexpr (std::is_enum<ThisType>::value) { \
			ref_def_interface = reinterpret_cast< const typename Gaff::RefDefInterface<ThisType, ProxyAllocator>* >(GetApp().getReflectionManager().getEnumReflection(GetHash())); /* To calm the compiler, even though this should be compiled out ... */ \
		} else { \
			ref_def_interface = reinterpret_cast< const typename Gaff::RefDefInterface<ThisType, ProxyAllocator>* >(GetApp().getReflectionManager().getReflection(GetHash())); /* To calm the compiler, even though this should be compiled out ... */ \
		} \
		g_ref_def = static_cast< typename Gaff::RefDefType<ThisType, ProxyAllocator>* >( \
			const_cast< typename Gaff::RefDefInterface<ThisType, ProxyAllocator>* >(ref_def_interface) \
		); \
		if (g_ref_def) { \
			GAFF_ASSERT_MSG( \
				g_version.getHash() == g_ref_def->getReflectionInstance().getVersion(), \
				"Version hash for %s does not match!", \
				GetName() \
			); \
		} else { \
			ProxyAllocator allocator("Reflection"); \
			g_ref_def = SHIB_ALLOCT(GAFF_SINGLE_ARG(typename Gaff::RefDefType<ThisType, ProxyAllocator>), allocator); \
			g_ref_def->setAllocator(allocator); \
			BuildReflection(*g_ref_def); \
			GetApp().getReflectionManager().registerReflection(g_ref_def); \
		} \
		g_defined = true;
#endif

#include "Shibboleth_ReflectionManager.h"
#include "Shibboleth_ProxyAllocator.h"
#include "Shibboleth_HashString.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_Hashable.h"
#include "Shibboleth_IApp.h"
#include <Gaff_Reflection.h>

#define SHIB_REFLECTION_CLASS_DECLARE GAFF_REFLECTION_CLASS_DECLARE
#define SHIB_REFLECTION_CLASS_DEFINE GAFF_REFLECTION_CLASS_DEFINE
#define SHIB_REFLECTION_DECLARE(type) GAFF_REFLECTION_DECLARE(type, ProxyAllocator)
#define SHIB_REFLECTION_DEFINE_BEGIN(type) GAFF_REFLECTION_DEFINE_BEGIN(type, ProxyAllocator)
#define SHIB_REFLECTION_DEFINE_END GAFF_REFLECTION_DEFINE_END
#define SHIB_REFLECTION_DEFINE(type) \
	SHIB_REFLECTION_DEFINE_BEGIN(type) \
	SHIB_REFLECTION_DEFINE_END(type)

#define SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE_NO_INHERITANCE(type, base) \
	SHIB_REFLECTION_DEFINE_BEGIN(type) \
		.BASE(base) \
		.ctor<>() \
	SHIB_REFLECTION_DEFINE_END(type)

#define SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(type, base) \
	SHIB_REFLECTION_DEFINE_BEGIN(type) \
		.BASE(base) \
	SHIB_REFLECTION_DEFINE_END(type)


#define SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE GAFF_TEMPLATE_REFLECTION_CLASS_DECLARE
#define SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE GAFF_TEMPLATE_REFLECTION_CLASS_DEFINE
#define SHIB_TEMPLATE_REFLECTION_DECLARE(type, ...) GAFF_TEMPLATE_REFLECTION_DECLARE(type, ProxyAllocator, __VA_ARGS__)
#define SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(type, ...) GAFF_TEMPLATE_REFLECTION_DEFINE_BEGIN(type, ProxyAllocator, __VA_ARGS__)
#define SHIB_TEMPLATE_REFLECTION_DEFINE_END GAFF_TEMPLATE_REFLECTION_DEFINE_END


SHIB_REFLECTION_DECLARE(int8_t)
SHIB_REFLECTION_DECLARE(int16_t)
SHIB_REFLECTION_DECLARE(int32_t)
SHIB_REFLECTION_DECLARE(int64_t)
SHIB_REFLECTION_DECLARE(uint8_t)
SHIB_REFLECTION_DECLARE(uint16_t)
SHIB_REFLECTION_DECLARE(uint32_t)
SHIB_REFLECTION_DECLARE(uint64_t)
SHIB_REFLECTION_DECLARE(float)
SHIB_REFLECTION_DECLARE(double)
SHIB_REFLECTION_DECLARE(bool)
SHIB_REFLECTION_DECLARE(U8String)
SHIB_REFLECTION_DECLARE(HashString32<>)
SHIB_REFLECTION_DECLARE(HashString64<>)
SHIB_REFLECTION_DECLARE(HashStringNoString32<>)
SHIB_REFLECTION_DECLARE(HashStringNoString64<>)
SHIB_REFLECTION_DECLARE(HashStringTemp32<>)
SHIB_REFLECTION_DECLARE(HashStringTemp64<>)

NS_SHIBBOLETH

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static Gaff::Hash64 HashStringInstanceHash(const Gaff::HashString<T, HashType, HashingFunc, Allocator, true> & hash_string, Gaff::Hash64 init)
{
	return Gaff::FNV1aHash64String(hash_string.getBuffer(), init);
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static Gaff::Hash64 HashStringInstanceHash(const Gaff::HashString<T, HashType, HashingFunc, Allocator, false> & hash_string, Gaff::Hash64 init)
{
	return Gaff::FNV1aHash64T(hash_string.getHash(), init);
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc>
static Gaff::Hash64 HashStringTempInstanceHash(const Gaff::HashStringTemp<T, HashType, HashingFunc>& hash_string, Gaff::Hash64 init)
{
	if (hash_string.getBuffer()) {
		return Gaff::FNV1aHash64String(hash_string.getBuffer(), init);
	} else {
		return Gaff::FNV1aHash64T(hash_string.getHash(), init);
	}
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static bool LoadHashString(const Gaff::ISerializeReader& reader, Gaff::HashString<T, HashType, HashingFunc, Allocator, true>& out)
{
	if (!reader.isString()) {
		return false;
	}

	const char* const str = reader.readString();
	out = str;
	reader.freeString(str);

	return true;
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static bool LoadHashString(const Gaff::ISerializeReader& reader, Gaff::HashString<T, HashType, HashingFunc, Allocator, false>& out)
{
	if (!reader.isString() && !reader.isUInt32() && !reader.isUInt64()) {
		return false;
	}

	if (reader.isString()) {
		const char* const str = reader.readString();
		out = str;
		reader.freeString(str);

	} else if (reader.isUInt32()) {
		out = Gaff::HashString<T, HashType, HashingFunc, Allocator, false>(static_cast<T>(reader.readUInt32()));

	} else if (reader.isUInt64()) {
		out = Gaff::HashString<T, HashType, HashingFunc, Allocator, false>(static_cast<T>(reader.readUInt64()));
	}

	return true;
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc>
static bool LoadHashStringTemp(const Gaff::ISerializeReader& reader, Gaff::HashStringTemp<T, HashType, HashingFunc>& out)
{
	if (!reader.isString()) {
		return false;
	}

	const char* const str = reader.readString();
	out = Gaff::HashStringTemp<T, HashType, HashingFunc>(str);
	reader.freeString(str);

	return true;
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static void SaveHashString(Gaff::ISerializeWriter& writer, const Gaff::HashString<T, HashType, HashingFunc, Allocator, true>& value)
{
	writer.writeString(value.getBuffer());
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static void SaveHashString(Gaff::ISerializeWriter& writer, const Gaff::HashString<T, HashType, HashingFunc, Allocator, false>& value)
{
	writer.writeUInt64(value.getHash());
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc>
static void SaveHashStringTemp(Gaff::ISerializeWriter& writer, const Gaff::HashStringTemp<T, HashType, HashingFunc>& value)
{
	if (value.getBuffer()) {
		writer.writeString(value.getBuffer());
	} else {
		writer.writeUInt64(value.getHash());
	}
}

template <class Allocator>
static Gaff::Hash64 HashStringInstance(const Gaff::U8String<Allocator>& string, Gaff::Hash64 init)
{
	return Gaff::FNV1aHash64String(string.data(), init);
}

template <class Allocator>
static bool LoadString(const Gaff::ISerializeReader& reader, Gaff::U8String<Allocator>& out)
{
	if (!reader.isString()) {
		return false;
	}

	const char* const str = reader.readString();
	out = str;
	reader.freeString(str);

	return true;
}

template <class Allocator>
static void SaveString(Gaff::ISerializeWriter& writer, const Gaff::U8String<Allocator>& value)
{
	writer.writeString(value.data());
}

NS_END
