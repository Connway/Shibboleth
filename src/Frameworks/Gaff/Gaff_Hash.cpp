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

#include "Gaff_Hash.h"

NS_GAFF

Hash64 FNV1aHash64(const char* key, size_t len, Hash64 init)
{
	Hash64Storage hash = init.getHash();

	for (size_t i = 0; i < len; ++i) {
		hash = (hash ^ static_cast<Hash64Storage>(key[i])) * k_magic_number_hash64_storage;
	}

	return Hash64(hash);
}

Hash64 FNV1Hash64(const char* key, size_t len, Hash64 init)
{
	Hash64Storage hash = init.getHash();

	for (size_t i = 0; i < len; ++i) {
		hash = (hash ^ k_magic_number_hash64_storage) * static_cast<Hash64Storage>(key[i]);
	}

	return Hash64(hash);
}

Hash32 FNV1aHash32(const char* key, size_t len, Hash32 init)
{
	Hash32Storage hash = init.getHash();

	for (size_t i = 0; i < len; ++i) {
		hash = (hash ^ static_cast<Hash32Storage>(key[i])) * k_magic_number_hash32_storage;
	}

	return Hash32(hash);
}

Hash32 FNV1Hash32(const char* key, size_t len, Hash32 init)
{
	Hash32Storage hash = init.getHash();

	for (size_t i = 0; i < len; ++i) {
		hash = (hash * k_magic_number_hash32_storage) ^ static_cast<Hash32Storage>(key[i]);
	}

	return Hash32(hash);
}

Hash64 FNV1aHash64(const char* key, size_t len)
{
	return FNV1aHash64(key, len, k_init_hash64);
}

Hash64 FNV1Hash64(const char* key, size_t len)
{
	return FNV1Hash64(key, len, k_init_hash64);
}

Hash32 FNV1aHash32(const char* key, size_t len)
{
	return FNV1aHash32(key, len, k_init_hash32);
}

Hash32 FNV1Hash32(const char* key, size_t len)
{
	return FNV1Hash32(key, len, k_init_hash32);
}

NS_END
