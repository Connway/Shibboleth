/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

/*!
\brief Computes 64-bit hash using FNV-1a algorithm. See <a href="http://www.isthe.com/chongo/tech/comp/fnv/">this page</a> for more details.
\param key The buffer we are using to calculate the hash.
\param len The length of the buffer in bytes.
\return The calculated hash.
*/
Hash64 FNV1aHash64(const char* key, size_t len, Hash64 init)
{
	for (size_t i = 0; i < len; ++i) {
		init = (init ^ static_cast<Hash64>(key[i])) * 1099511628211ULL;
	}

	return init;
}

/*!
\brief Computes 64-bit hash using FNV-1 algorithm. See <a href="http://www.isthe.com/chongo/tech/comp/fnv/">this page</a> for more details.
\param key The buffer we are using to calculate the hash.
\param len The length of the buffer in bytes.
\return The calculated hash.
*/
Hash64 FNV1Hash64(const char* key, size_t len, Hash64 init)
{
	for (size_t i = 0; i < len; ++i) {
		init = (init ^ 1099511628211ULL) * static_cast<Hash64>(key[i]);
	}

	return init;
}

/*!
\brief Computes 32-bit hash using FNV-1a algorithm. See <a href="http://www.isthe.com/chongo/tech/comp/fnv/">this page</a> for more details.
\param key The buffer we are using to calculate the hash.
\param len The length of the buffer in bytes.
\return The calculated hash.
*/
Hash32 FNV1aHash32(const char* key, size_t len, Hash32 init)
{
	for (size_t i = 0; i < len; ++i) {
		init = (init ^ static_cast<Hash32>(key[i])) * 16777619U;
	}

	return init;
}

/*!
\brief Computes 32-bit hash using FNV-1 algorithm. See <a href="http://www.isthe.com/chongo/tech/comp/fnv/">this page</a> for more details.
\param key The buffer we are using to calculate the hash.
\param len The length of the buffer in bytes.
\return The calculated hash.
*/
Hash32 FNV1Hash32(const char* key, size_t len, Hash32 init)
{
	for (size_t i = 0; i < len; ++i) {
		init = (init * 16777619U) ^ static_cast<Hash32>(key[i]);
	}

	return init;
}

Hash64 FNV1aHash64(const char* key, size_t len)
{
	return FNV1aHash64(key, len, INIT_HASH64);
}

Hash64 FNV1Hash64(const char* key, size_t len)
{
	return FNV1Hash64(key, len, INIT_HASH64);
}

Hash32 FNV1aHash32(const char* key, size_t len)
{
	return FNV1aHash32(key, len, INIT_HASH32);
}

Hash32 FNV1Hash32(const char* key, size_t len)
{
	return FNV1Hash32(key, len, INIT_HASH32);
}

NS_END
