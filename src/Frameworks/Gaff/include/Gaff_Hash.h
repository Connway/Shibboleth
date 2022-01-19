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

#include "Gaff_Defines.h"
#include <EASTL/string_view.h>
#include <EASTL/string.h>

NS_GAFF

using Hash64Storage = uint64_t;
using Hash32Storage = uint32_t;

constexpr Hash64Storage k_magic_number_hash64_storage = 1099511628211ULL;
constexpr Hash64Storage k_init_hash64_storage = 14695981039346656037ULL;
constexpr Hash32Storage k_magic_number_hash32_storage = 16777619U;
constexpr Hash32Storage k_init_hash32_storage = 2166136261U;

template <class HashStorage>
struct HashValueHelper;

template <>
struct HashValueHelper<Hash64Storage> final
{
	static constexpr Hash64Storage k_magic_number = k_magic_number_hash64_storage;
	static constexpr Hash64Storage k_init_value = k_init_hash64_storage;
};

template <>
struct HashValueHelper<Hash32Storage> final
{
	static constexpr Hash32Storage k_magic_number = k_magic_number_hash32_storage;
	static constexpr Hash32Storage k_init_value = k_init_hash32_storage;
};



template <class HashStorage>
class Hash final
{
public:
	constexpr explicit Hash(HashStorage hash):
		_hash(hash)
	{
	}

	constexpr Hash(const Hash<HashStorage>& hash) = default;
	constexpr Hash(void) = default;

	constexpr Hash& operator=(const Hash<HashStorage>& rhs) = default;

	constexpr bool operator==(Hash<HashStorage> rhs) const { return _hash == rhs._hash; }
	constexpr bool operator!=(Hash<HashStorage> rhs) const { return _hash != rhs._hash; }
	constexpr bool operator<(Hash<HashStorage> rhs) const { return _hash < rhs._hash; }
	constexpr bool operator<=(Hash<HashStorage> rhs) const { return _hash <= rhs._hash; }
	constexpr bool operator>(Hash<HashStorage> rhs) const { return _hash > rhs._hash; }
	constexpr bool operator>=(Hash<HashStorage> rhs) const { return _hash >= rhs._hash; }

	constexpr HashStorage getHash(void) const { return _hash; }
	constexpr void setHash(HashStorage hash) { _hash = hash; }

private:
	HashStorage _hash = HashValueHelper<HashStorage>::k_init_value;
};

using Hash64 = Hash<Hash64Storage>;
using Hash32 = Hash<Hash32Storage>;

constexpr Hash64 k_init_hash64(k_init_hash64_storage);
constexpr Hash32 k_init_hash32(k_init_hash32_storage);

using HashFunc64 = Hash64 (*)(const char*, size_t);
using HashFunc32 = Hash32 (*)(const char*, size_t);

Hash64 FNV1aHash64(const char* key, size_t len, Hash64 init);
Hash64 FNV1Hash64(const char* key, size_t len, Hash64 init);
Hash32 FNV1aHash32(const char* key, size_t len, Hash32 init);
Hash32 FNV1Hash32(const char* key, size_t len, Hash32 init);

// These functions just call the above functions with k_init_hash64/32
Hash64 FNV1aHash64(const char* key, size_t len);
Hash64 FNV1Hash64(const char* key, size_t len);
Hash32 FNV1aHash32(const char* key, size_t len);
Hash32 FNV1Hash32(const char* key, size_t len);

template <class T>
Hash64 FNV1Hash64VAdd(Hash64 init, const T* front)
{
	return FNV1Hash64(reinterpret_cast<const char*>(front), sizeof(T), init);
}

template <class T, class... Args>
Hash64 FNV1Hash64VAdd(Hash64 init, const T* front, const Args*... args)
{
	init = FNV1Hash64(reinterpret_cast<const char*>(front), sizeof(T), init);
	return FNV1Hash64VAdd(init, args...);
}

template <class T>
Hash64 FNV1aHash64VAdd(Hash64 init, const T* front)
{
	return FNV1aHash64(reinterpret_cast<const char*>(front), sizeof(T), init);
}

template <class T, class... Args>
Hash64 FNV1aHash64VAdd(Hash64 init, const T* front, const Args*... args)
{
	init = FNV1aHash64(reinterpret_cast<const char*>(front), sizeof(T), init);
	return FNV1aHash64VAdd(init, args...);
}

template <class T>
Hash32 FNV1Hash32VAdd(Hash32 init, const T* front)
{
	return FNV1Hash32(reinterpret_cast<const char*>(front), sizeof(T), init);
}

template <class T, class... Args>
Hash32 FNV1Hash32VAdd(Hash32 init, const T* front, const Args*... args)
{
	init = FNV1Hash32(reinterpret_cast<const char*>(front), sizeof(T), init);
	return FNV1Hash32VAdd(init, args...);
}

template <class T>
Hash32 FNV1aHash32VAdd(Hash32 init, const T* front)
{
	return FNV1aHash32(reinterpret_cast<const char*>(front), sizeof(T), init);
}

template <class T, class... Args>
Hash32 FNV1aHash32VAdd(Hash32 init, const T* front, const Args*... args)
{
	init = FNV1aHash32(reinterpret_cast<const char*>(front), sizeof(T), init);
	return FNV1aHash32VAdd(init, args...);
}

template <class... Args>
Hash64 FNV1Hash64V(const Args*... args)
{
	return FNV1Hash64VAdd(k_init_hash64, args...);
}

template <class... Args>
Hash64 FNV1aHash64V(const Args*... args)
{
	return FNV1aHash64VAdd(k_init_hash64, args...);
}

template <class... Args>
Hash32 FNV1Hash32V(const Args*... args)
{
	return FNV1Hash32VAdd(k_init_hash32, args...);
}

template <class... Args>
Hash32 FNV1aHash32V(const Args*... args)
{
	return FNV1aHash32VAdd(k_init_hash32, args...);
}

template <class T>
Hash64 FNV1aHash64T(const T* value, size_t len, Hash64 init = k_init_hash64)
{
	return FNV1aHash64(reinterpret_cast<const char*>(value), sizeof(T) * len, init);
}

template <class T>
Hash64 FNV1Hash64T(const T* value, size_t len, Hash64 init = k_init_hash64)
{
	return FNV1Hash64(reinterpret_cast<const char*>(value), sizeof(T) * len, init);
}

template <class T>
Hash32 FNV1aHash32T(const T* value, size_t len, Hash32 init = k_init_hash32)
{
	return FNV1aHash32(reinterpret_cast<const char*>(value), sizeof(T) * len, init);
}

template <class T>
Hash32 FNV1Hash32T(const T* value, size_t len, Hash32 init = k_init_hash32)
{
	return FNV1Hash32(reinterpret_cast<const char*>(value), sizeof(T) * len, init);
}

template <class T>
Hash64 FNV1aHash64T(const T& value, Hash64 init = k_init_hash64)
{
	return FNV1aHash64(reinterpret_cast<const char*>(&value), sizeof(T), init);
}

template <class T>
Hash64 FNV1Hash64T(const T& value, Hash64 init = k_init_hash64)
{
	return FNV1Hash64(reinterpret_cast<const char*>(&value), sizeof(T), init);
}

template <class T>
Hash32 FNV1aHash32T(const T& value, Hash32 init = k_init_hash32)
{
	return FNV1aHash32(reinterpret_cast<const char*>(&value), sizeof(T), init);
}

template <class T>
Hash32 FNV1Hash32T(const T& value, Hash32 init = k_init_hash32)
{
	return FNV1Hash32(reinterpret_cast<const char*>(&value), sizeof(T), init);
}

constexpr Hash64 FNV1aHash64Const(const char* key, size_t len, Hash64 init = k_init_hash64)
{
	Hash64Storage hash = init.getHash();

	for (size_t i = 0; i < len; ++i) {
		hash = (hash ^ static_cast<Hash64Storage>(key[i])) * k_magic_number_hash64_storage;
	}

	return Hash64(hash);
}

constexpr Hash64 FNV1Hash64Const(const char* key, size_t len, Hash64 init = k_init_hash64)
{
	Hash64Storage hash = init.getHash();

	for (size_t i = 0; i < len; ++i) {
		hash = (hash ^ k_magic_number_hash64_storage) * static_cast<Hash64Storage>(key[i]);
	}

	return Hash64(hash);
}

constexpr Hash32 FNV1aHash32Const(const char* key, size_t len, Hash32 init = k_init_hash32)
{
	Hash32Storage hash = init.getHash();

	for (size_t i = 0; i < len; ++i) {
		hash = (hash ^ static_cast<Hash32Storage>(key[i])) * k_magic_number_hash32_storage;
	}

	return Hash32(hash);
}

constexpr Hash32 FNV1Hash32Const(const char* key, size_t len, Hash32 init = k_init_hash32)
{
	Hash32Storage hash = init.getHash();

	for (size_t i = 0; i < len; ++i) {
		hash = (hash * k_magic_number_hash32_storage) ^ static_cast<Hash32Storage>(key[i]);
	}

	return Hash32(hash);
}

template <size_t size>
constexpr Hash64 FNV1aHash64Const(const char (&key)[size], Hash64 init = k_init_hash64)
{
	return FNV1aHash64Const(key, size - 1, init);
}

template <size_t size>
constexpr Hash64 FNV1Hash64Const(const char (&key)[size], Hash64 init = k_init_hash64)
{
	return FNV1Hash64Const(key, size - 1, init);
}

template <size_t size>
constexpr Hash32 FNV1aHash32Const(const char (&key)[size], Hash32 init = k_init_hash32)
{
	return FNV1aHash32Const(key, size - 1, init);
}

template <size_t size>
constexpr Hash32 FNV1Hash32Const(const char (&key)[size], Hash32 init = k_init_hash32)
{
	return FNV1Hash32Const(key, size - 1, init);
}

template <class T>
constexpr Hash64 FNV1aHash64Const(eastl::basic_string_view<T> key, Hash64 init = k_init_hash64)
{
	return FNV1aHash64Const(key.data(), key.size(), init);
}

constexpr Hash64 FNV1Hash64Const(eastl::string_view key, Hash64 init = k_init_hash64)
{
	return FNV1Hash64Const(key.data(), key.size(), init);
}

constexpr Hash32 FNV1aHash32Const(eastl::string_view key, Hash32 init = k_init_hash32)
{
	return FNV1aHash32Const(key.data(), key.size(), init);
}

constexpr Hash32 FNV1Hash32Const(eastl::string_view key, Hash32 init = k_init_hash32)
{
	return FNV1Hash32Const(key.data(), key.size(), init);
}

template <class T>
Hash64 FNV1aHash64String(const T* string, Hash64 init = k_init_hash64)
{
	return FNV1aHash64(reinterpret_cast<const char*>(string), eastl::CharStrlen(string) * sizeof(T), init);
}

template <class T>
Hash64 FNV1Hash64String(const T* string, Hash64 init = k_init_hash64)
{
	return FNV1Hash64(reinterpret_cast<const char*>(string), eastl::CharStrlen(string) * sizeof(T), init);
}

template <class T>
Hash32 FNV1aHash32String(const T* string, Hash32 init = k_init_hash32)
{
	return FNV1aHash32(reinterpret_cast<const char*>(string), eastl::CharStrlen(string) * sizeof(T), init);
}

template <class T>
Hash32 FNV1Hash32String(const T* string, Hash32 init = k_init_hash32)
{
	return FNV1Hash32(reinterpret_cast<const char*>(string), eastl::CharStrlen(string) * sizeof(T), init);
}

constexpr Hash64Storage FNV1aHash64StringConstHelper(Hash64Storage hash, char character)
{
	return (hash ^ static_cast<Hash64Storage>(character)) * k_magic_number_hash64_storage;
}

constexpr Hash64Storage FNV1aHash64StringConstHelper(Hash64Storage hash, wchar_t character)
{
	const char left = static_cast<char>((0xFF00 & character) >> 8);
	const char right = static_cast<char>(0x00FF & character);
	hash = FNV1aHash64StringConstHelper(hash, left);
	hash = FNV1aHash64StringConstHelper(hash, right);

	return  hash;
}

constexpr Hash64Storage FNV1aHash64StringConstHelper(Hash64Storage hash, char16_t character)
{
	const char left = static_cast<char>((0xFF00 & character) >> 8);
	const char right = static_cast<char>(0x00FF & character);
	hash = FNV1aHash64StringConstHelper(hash, left);
	hash = FNV1aHash64StringConstHelper(hash, right);

	return  hash;
}

constexpr Hash64Storage FNV1aHash64StringConstHelper(Hash64Storage hash, char32_t character)
{
	const char part_1 = static_cast<char>((0xFF000000 & character) >> 24);
	const char part_2 = static_cast<char>((0x00FF0000 & character) >> 16);
	const char part_3 = static_cast<char>((0x0000FF00 & character) >> 8);
	const char part_4 = static_cast<char>(0x000000FF & character);
	hash = FNV1aHash64StringConstHelper(hash, part_1);
	hash = FNV1aHash64StringConstHelper(hash, part_2);
	hash = FNV1aHash64StringConstHelper(hash, part_3);
	hash = FNV1aHash64StringConstHelper(hash, part_4);

	return  hash;
}

constexpr Hash64Storage FNV1Hash64StringConstHelper(Hash64Storage hash, char character)
{
	return hash = (hash ^ k_magic_number_hash64_storage) * static_cast<Hash64Storage>(character);
}

constexpr Hash64Storage FNV1Hash64StringConstHelper(Hash64Storage hash, wchar_t character)
{
	const char left = static_cast<char>((0xFF00 & character) >> 8);
	const char right = static_cast<char>(0x00FF & character);
	hash = FNV1Hash64StringConstHelper(hash, left);
	hash = FNV1Hash64StringConstHelper(hash, right);

	return  hash;
}

constexpr Hash64Storage FNV1Hash64StringConstHelper(Hash64Storage hash, char16_t character)
{
	const char left = static_cast<char>((0xFF00 & character) >> 8);
	const char right = static_cast<char>(0x00FF & character);
	hash = FNV1Hash64StringConstHelper(hash, left);
	hash = FNV1Hash64StringConstHelper(hash, right);

	return  hash;
}

constexpr Hash64Storage FNV1Hash64StringConstHelper(Hash64Storage hash, char32_t character)
{
	const char part_1 = static_cast<char>((0xFF000000 & character) >> 24);
	const char part_2 = static_cast<char>((0x00FF0000 & character) >> 16);
	const char part_3 = static_cast<char>((0x0000FF00 & character) >> 8);
	const char part_4 = static_cast<char>(0x000000FF & character);
	hash = FNV1Hash64StringConstHelper(hash, part_1);
	hash = FNV1Hash64StringConstHelper(hash, part_2);
	hash = FNV1Hash64StringConstHelper(hash, part_3);
	hash = FNV1Hash64StringConstHelper(hash, part_4);

	return  hash;
}

constexpr Hash32Storage FNV1aHash32StringConstHelper(Hash32Storage hash, char character)
{
	return (hash ^ static_cast<Hash32Storage>(character)) * k_magic_number_hash32_storage;
}

constexpr Hash32Storage FNV1aHash32StringConstHelper(Hash32Storage hash, wchar_t character)
{
	const char left = static_cast<char>((0xFF00 & character) >> 8);
	const char right = static_cast<char>(0x00FF & character);
	hash = FNV1aHash32StringConstHelper(hash, left);
	hash = FNV1aHash32StringConstHelper(hash, right);

	return  hash;
}

constexpr Hash32Storage FNV1aHash32StringConstHelper(Hash32Storage hash, char16_t character)
{
	const char left = static_cast<char>((0xFF00 & character) >> 8);
	const char right = static_cast<char>(0x00FF & character);
	hash = FNV1aHash32StringConstHelper(hash, left);
	hash = FNV1aHash32StringConstHelper(hash, right);

	return  hash;
}

constexpr Hash32Storage FNV1aHash32StringConstHelper(Hash32Storage hash, char32_t character)
{
	const char part_1 = static_cast<char>((0xFF000000 & character) >> 24);
	const char part_2 = static_cast<char>((0x00FF0000 & character) >> 16);
	const char part_3 = static_cast<char>((0x0000FF00 & character) >> 8);
	const char part_4 = static_cast<char>(0x000000FF & character);
	hash = FNV1aHash32StringConstHelper(hash, part_1);
	hash = FNV1aHash32StringConstHelper(hash, part_2);
	hash = FNV1aHash32StringConstHelper(hash, part_3);
	hash = FNV1aHash32StringConstHelper(hash, part_4);

	return  hash;
}

constexpr Hash32Storage FNV1Hash32StringConstHelper(Hash32Storage hash, char character)
{
	return hash = (hash ^ k_magic_number_hash32_storage) * static_cast<Hash32Storage>(character);
}

constexpr Hash32Storage FNV1Hash32StringConstHelper(Hash32Storage hash, wchar_t character)
{
	const char left = static_cast<char>((0xFF00 & character) >> 8);
	const char right = static_cast<char>(0x00FF & character);
	hash = FNV1Hash32StringConstHelper(hash, left);
	hash = FNV1Hash32StringConstHelper(hash, right);

	return  hash;
}

constexpr Hash32Storage FNV1Hash32StringConstHelper(Hash32Storage hash, char16_t character)
{
	const char left = static_cast<char>((0xFF00 & character) >> 8);
	const char right = static_cast<char>(0x00FF & character);
	hash = FNV1Hash32StringConstHelper(hash, left);
	hash = FNV1Hash32StringConstHelper(hash, right);

	return  hash;
}

constexpr Hash32Storage FNV1Hash32StringConstHelper(Hash32Storage hash, char32_t character)
{
	const char part_1 = static_cast<char>((0xFF000000 & character) >> 24);
	const char part_2 = static_cast<char>((0x00FF0000 & character) >> 16);
	const char part_3 = static_cast<char>((0x0000FF00 & character) >> 8);
	const char part_4 = static_cast<char>(0x000000FF & character);
	hash = FNV1Hash32StringConstHelper(hash, part_1);
	hash = FNV1Hash32StringConstHelper(hash, part_2);
	hash = FNV1Hash32StringConstHelper(hash, part_3);
	hash = FNV1Hash32StringConstHelper(hash, part_4);

	return  hash;
}

template <class T>
constexpr Hash64 FNV1aHash64StringConst(const T* string, Hash64 init = k_init_hash64)
{
	Hash64Storage hash = init.getHash();

	while (*string) {
		hash = FNV1aHash64StringConstHelper(hash, *string);
		++string;
	}

	return Hash64(hash);
}

template <class T>
constexpr Hash64 FNV1Hash64StringConst(const T* string, Hash64 init = k_init_hash64)
{
	Hash64Storage hash = init.getHash();

	while (*string) {
		hash = FNV1Hash64StringConstHelper(hash, *string);
		++string;
	}

	return Hash64(hash);
}

template <class T>
constexpr Hash32 FNV1aHash32StringConst(const T* string, Hash32 init = k_init_hash32)
{
	Hash32Storage hash = init.getHash();

	while (*string) {
		hash = FNV1aHash32StringConstHelper(hash, *string);
		++string;
	}

	return Hash32(hash);
}

template <class T>
constexpr Hash32 FNV1Hash32StringConst(const T* string, Hash32 init = k_init_hash32)
{
	Hash32Storage hash = init.getHash();

	while (*string) {
		hash = FNV1Hash32StringConstHelper(hash, *string);
		++string;
	}

	return Hash32(hash);
}

NS_END
