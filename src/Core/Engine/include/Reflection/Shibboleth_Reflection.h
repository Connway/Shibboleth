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

#include "Shibboleth_ReflectionManager.h"
#include "Shibboleth_ReflectionMacros.h"
#include "Shibboleth_ReflectionBase.h"
#include "Shibboleth_IApp.h"

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
static Gaff::Hash64 HashStringViewInstanceHash(const Gaff::HashStringView<T, HashType, HashingFunc>& hash_string, Gaff::Hash64 init)
{
	if (hash_string.getBuffer()) {
		return Gaff::FNV1aHash64String(hash_string.getBuffer(), init);
	} else {
		return Gaff::FNV1aHash64T(hash_string.getHash(), init);
	}
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static bool LoadHashString(const Shibboleth::ISerializeReader& reader, Gaff::HashString<T, HashType, HashingFunc, Allocator, true>& out)
{
	if (!reader.isString()) {
		return false;
	}

	const char8_t* const str = reader.readString();
	out = str;
	reader.freeString(str);

	return true;
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static bool LoadHashString(const Shibboleth::ISerializeReader& reader, Gaff::HashString<T, HashType, HashingFunc, Allocator, false>& out)
{
	if (!reader.isString() && !reader.isUInt32() && !reader.isUInt64()) {
		return false;
	}

	if (reader.isString()) {
		const char8_t* const str = reader.readString();
		out = str;
		reader.freeString(str);

	} else {
		if constexpr (std::is_same<HashType, Gaff::Hash32>::value) {
			out = Gaff::HashString<T, HashType, HashingFunc, Allocator, false>(HashType(static_cast<T>(reader.readUInt32())));
		} else if constexpr (std::is_same<HashType, Gaff::Hash64>::value) {
			out = Gaff::HashString<T, HashType, HashingFunc, Allocator, false>(HashType(static_cast<T>(reader.readUInt64())));
		} else {
			GAFF_TEMPLATE_STATIC_ASSERT(false, "Unknown hash type in LoadHashString.");
		}
	}

	return true;
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc>
static bool LoadHashStringView(const Shibboleth::ISerializeReader& reader, Gaff::HashStringView<T, HashType, HashingFunc>& out)
{
	if (!reader.isString()) {
		return false;
	}

	const char8_t* const str = reader.readString();
	out = Gaff::HashStringView<T, HashType, HashingFunc>(str);
	reader.freeString(str);

	return true;
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static void SaveHashString(Shibboleth::ISerializeWriter& writer, const Gaff::HashString<T, HashType, HashingFunc, Allocator, true>& value)
{
	writer.writeString(value.getBuffer());
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc, class Allocator>
static void SaveHashString(Shibboleth::ISerializeWriter& writer, const Gaff::HashString<T, HashType, HashingFunc, Allocator, false>& value)
{
	writer.writeUInt64(value.getHash().getHash());
}

template <class T, class HashType, Gaff::HashFunc<HashType> HashingFunc>
static void SaveHashStringView(Shibboleth::ISerializeWriter& writer, const Gaff::HashStringView<T, HashType, HashingFunc>& value)
{
	if (value.getBuffer()) {
		writer.writeString(value.getBuffer());
	} else {
		writer.writeUInt64(value.getHash().getHash());
	}
}

template <class Allocator>
static Gaff::Hash64 HashStringInstance(const Gaff::U8String<Allocator>& string, Gaff::Hash64 init)
{
	return Gaff::FNV1aHash64String(string.data(), init);
}

template <class Allocator>
static bool LoadString(const Shibboleth::ISerializeReader& reader, Gaff::U8String<Allocator>& out)
{
	if (!reader.isString()) {
		return false;
	}

	const char8_t* const str = reader.readString();
	out = str;
	reader.freeString(str);

	return true;
}

template <class Allocator>
static void SaveString(Shibboleth::ISerializeWriter& writer, const Gaff::U8String<Allocator>& value)
{
	writer.writeString(value.data());
}

NS_END

NS_HASHABLE
	GAFF_CLASS_HASHABLE(Shibboleth::ISerializeReader);
	GAFF_CLASS_HASHABLE(Shibboleth::ISerializeWriter);
	GAFF_CLASS_HASHABLE(Refl::IReflectionDefinition);
	GAFF_CLASS_HASHABLE(Refl::IAttribute);
	GAFF_CLASS_HASHABLE(char8_t);
NS_END


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

SHIB_REFLECTION_DECLARE(Gaff::Hash32)
SHIB_REFLECTION_DECLARE(Gaff::Hash64)

SHIB_REFLECTION_DECLARE(Shibboleth::U8String)
SHIB_REFLECTION_DECLARE(Shibboleth::HashString32<>)
SHIB_REFLECTION_DECLARE(Shibboleth::HashString64<>)
SHIB_REFLECTION_DECLARE(Shibboleth::HashStringNoString32<>)
SHIB_REFLECTION_DECLARE(Shibboleth::HashStringNoString64<>)
SHIB_REFLECTION_DECLARE(Shibboleth::HashStringView32<>)
SHIB_REFLECTION_DECLARE(Shibboleth::HashStringView64<>)

#include "Shibboleth_ReflectionDefinition.inl"
#include "Shibboleth_ReflectionDefinitionFunction.inl"
#include "Reflection/Shibboleth_ReflectionDefinitionVariable.inl"
