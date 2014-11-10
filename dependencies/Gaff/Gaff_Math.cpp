/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gaff_Math.h"
#include <cmath>

NS_GAFF

/*!
	\brief Computes 64-bit hash using FNV-1a algorithm. See <a href="http://www.isthe.com/chongo/tech/comp/fnv/">this page</a> for more details.
	\param key The buffer we are using to calculate the hash.
	\param len The length of the buffer in bytes.
	\return The calculated hash.
*/
unsigned long long FNV1aHash64(const char* key, unsigned int len)
{
	unsigned long long hash = 14695981039346656037ULL;

	for (unsigned int i = 0; i < len; ++i) {
		hash = (hash ^ (unsigned long long)key[i]) * 1099511628211ULL;
	}

	return hash;
}

/*!
	\brief Computes 64-bit hash using FNV-1 algorithm. See <a href="http://www.isthe.com/chongo/tech/comp/fnv/">this page</a> for more details.
	\param key The buffer we are using to calculate the hash.
	\param len The length of the buffer in bytes.
	\return The calculated hash.
*/
unsigned long long FNV1Hash64(const char* key, unsigned int len)
{
	unsigned long long hash = 14695981039346656037ULL;

	for (unsigned int i = 0; i < len; ++i) {
		hash = (hash ^ 1099511628211ULL) * (unsigned long long)key[i];
	}

	return hash;
}

/*!
	\brief Computes 32-bit hash using FNV-1a algorithm. See <a href="http://www.isthe.com/chongo/tech/comp/fnv/">this page</a> for more details.
	\param key The buffer we are using to calculate the hash.
	\param len The length of the buffer in bytes.
	\return The calculated hash.
*/
unsigned int FNV1aHash32(const char* key, unsigned int len)
{
	unsigned int hash = 2166136261U;

	for (unsigned int i = 0; i < len; ++i) {
		hash = (hash ^ (unsigned int)key[i]) * 16777619U;
	}

	return hash;
}

/*!
	\brief Computes 32-bit hash using FNV-1 algorithm. See <a href="http://www.isthe.com/chongo/tech/comp/fnv/">this page</a> for more details.
	\param key The buffer we are using to calculate the hash.
	\param len The length of the buffer in bytes.
	\return The calculated hash.
*/
unsigned int FNV1Hash32(const char* key, unsigned int len)
{
	unsigned int hash = 2166136261U;

	for (unsigned int i = 0; i < len; ++i) {
		hash = (hash * 16777619U) ^ (unsigned int)key[i];
	}

	return hash;
}

/*!
	\brief See <a href="http://en.wikipedia.org/wiki/Binomial_coefficient">this Wikipedia page</a> for more information.
*/
float BinomialCoefficient(float n, float k)
{
	return tgammaf(n + 1.0f) / (tgammaf(k + 1.0f) * tgammaf(n - k + 1.0f));
}

/*!
	\brief See <a href="http://en.wikipedia.org/wiki/Bernstein_polynomial">this Wikipedia page</a> for more information.
*/
float BernsteinPolynomial(float t, float n, float k)
{
	return BinomialCoefficient(n, k) * powf(t, k) * powf(1.0f - t, n - k);
}

NS_END
