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

#include "Gaff_Math.h"
#include <EASTL/string_view.h>
#include <EASTL/string.h>

NS_GAFF

using Hash64 = uint64_t;
using Hash32 = uint32_t;

constexpr Hash64 INIT_HASH64 = 14695981039346656037ULL;
constexpr Hash32 INIT_HASH32 = 2166136261U;

using HashFunc64 = Hash64 (*)(const char*, size_t);
using HashFunc32 = Hash32 (*)(const char*, size_t);

Hash64 FNV1aHash64(const char* key, size_t len, Hash64 init);
Hash64 FNV1Hash64(const char* key, size_t len, Hash64 init);
Hash32 FNV1aHash32(const char* key, size_t len, Hash32 init);
Hash32 FNV1Hash32(const char* key, size_t len, Hash32 init);

// These functions just call the above functions with INIT_HASH64/32
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
	return FNV1Hash64VAdd(INIT_HASH64, args...);
}

template <class... Args>
Hash64 FNV1aHash64V(const Args*... args)
{
	return FNV1aHash64VAdd(INIT_HASH64, args...);
}

template <class... Args>
Hash32 FNV1Hash32V(const Args*... args)
{
	return FNV1Hash32VAdd(INIT_HASH32, args...);
}

template <class... Args>
Hash32 FNV1aHash32V(const Args*... args)
{
	return FNV1aHash32VAdd(INIT_HASH32, args...);
}

template <class T>
Hash64 FNV1aHash64T(const T& value, Hash64 init = INIT_HASH64)
{
	return FNV1aHash64(reinterpret_cast<const char*>(&value), sizeof(T), init);
}

template <class T>
Hash64 FNV1Hash64T(const T& value, Hash64 init = INIT_HASH64)
{
	return FNV1Hash64(reinterpret_cast<const char*>(&value), sizeof(T), init);
}

template <class T>
Hash32 FNV1aHash32T(const T& value, Hash32 init = INIT_HASH32)
{
	return FNV1aHash32(reinterpret_cast<const char*>(&value), sizeof(T), init);
}

template <class T>
Hash32 FNV1Hash32T(const T& value, Hash32 init = INIT_HASH32)
{
	return FNV1Hash32(reinterpret_cast<const char*>(&value), sizeof(T), init);
}

constexpr Hash64 FNV1aHash64Const(const char* key, size_t len, Hash64 init)
{
	return (len) ? FNV1aHash64Const(key + 1, len - 1, (init ^ static_cast<Hash64>(*key)) * 1099511628211ULL) : init;
}

constexpr Hash64 FNV1Hash64Const(const char* key, size_t len, Hash64 init)
{
	return (len) ? FNV1Hash64Const(key + 1, len - 1, (init ^ 1099511628211ULL) * static_cast<Hash64>(*key)) : init;
}

constexpr Hash32 FNV1aHash32Const(const char* key, size_t len, Hash32 init = INIT_HASH32)
{
	return (len) ? FNV1aHash32Const(key + 1, len - 1, (init ^ static_cast<Hash32>(*key)) * 16777619U) : init;
}

constexpr Hash32 FNV1Hash32Const(const char* key, size_t len, Hash32 init = INIT_HASH32)
{
	return (len) ? FNV1Hash32Const(key + 1, len - 1, (init ^ 16777619U) * static_cast<Hash32>(*key)) : init;
}

template <size_t size>
constexpr Hash64 FNV1aHash64Const(const char (&key)[size], Hash64 init = INIT_HASH64)
{
	return FNV1aHash64Const(key, size - 1, init);
}

template <size_t size>
constexpr Hash64 FNV1Hash64Const(const char (&key)[size], Hash64 init = INIT_HASH64)
{
	return FNV1Hash64Const(key, size - 1, init);
}

template <size_t size>
constexpr Hash32 FNV1aHash32Const(const char (&key)[size], Hash32 init = INIT_HASH32)
{
	return FNV1aHash32Const(key, size - 1, init);
}

template <size_t size>
constexpr Hash32 FNV1Hash32Const(const char (&key)[size], Hash32 init = INIT_HASH32)
{
	return FNV1Hash32Const(key, size - 1, init);
}

constexpr Hash64 FNV1aHash64Const(eastl::string_view key, Hash64 init = INIT_HASH64)
{
	return FNV1aHash64Const(key.data(), key.size(), init);
}

constexpr Hash64 FNV1Hash64Const(eastl::string_view key, Hash64 init = INIT_HASH64)
{
	return FNV1Hash64Const(key.data(), key.size(), init);
}

constexpr Hash32 FNV1aHash32Const(eastl::string_view key, Hash32 init = INIT_HASH32)
{
	return FNV1aHash32Const(key.data(), key.size(), init);
}

constexpr Hash32 FNV1Hash32Const(eastl::string_view key, Hash32 init = INIT_HASH32)
{
	return FNV1Hash32Const(key.data(), key.size(), init);
}

template <class T>
Hash64 FNV1aHash64String(const T* string, Hash64 init = INIT_HASH64)
{
	return FNV1aHash64(reinterpret_cast<const char*>(string), eastl::CharStrlen(string) * sizeof(T), init);
}

template <class T>
Hash64 FNV1Hash64String(const T* string, Hash64 init = INIT_HASH64)
{
	return FNV1Hash64(reinterpret_cast<const char*>(string), eastl::CharStrlen(string) * sizeof(T), init);
}

template <class T>
Hash32 FNV1aHash32String(const T* string, Hash32 init = INIT_HASH32)
{
	return FNV1aHash32(reinterpret_cast<const char*>(string), eastl::CharStrlen(string) * sizeof(T), init);
}

template <class T>
Hash32 FNV1Hash32String(const T* string, Hash32 init = INIT_HASH32)
{
	return FNV1Hash32(reinterpret_cast<const char*>(string), eastl::CharStrlen(string) * sizeof(T), init);
}

template <class T>
constexpr Hash64 FNV1aHash64StringConst(const T* string, Hash64 init = INIT_HASH64)
{
	return (*string) ? FNV1aHash64StringConst(string + 1, (init ^ static_cast<Hash64>(*string)) * 1099511628211ULL) : init;
}

template <class T>
constexpr Hash64 FNV1Hash64StringConst(const T* string, Hash64 init = INIT_HASH64)
{
	return (*string) ? FNV1Hash64StringConst(string + 1, (init ^ 1099511628211ULL) * static_cast<Hash64>(*string)) : init;
}

template <class T>
constexpr Hash32 FNV1aHash32StringConst(const T* string, Hash32 init = INIT_HASH32)
{
	return (*string) ? FNV1aHash32StringConst(string + 1, (init ^ static_cast<Hash32>(*string)) * 16777619U) : init;
}

template <class T>
constexpr Hash32 FNV1Hash32StringConst(const T* string, Hash32 init = INIT_HASH32)
{
	return (*string) ? FNV1Hash32StringConst(string + 1, (init ^ 16777619U) * static_cast<Hash32>(*string)) : init;
}

NS_END
