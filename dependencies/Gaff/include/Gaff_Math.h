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

#pragma once

#include "Gaff_Defines.h"

NS_GAFF

const float Pi = 3.141592654f;
const float DegToRad = Pi / 180.0f; //!< Scalar for converting an angle from Degrees to Radians.
const float RadToDeg = 180.0f / Pi; //!< Scalar for converting an angle from Radians to Degrees;

/*!
	\brief Returns the minimum of \a v1 and \a v2
*/
template <class T>
const T& Min(const T& v1, const T& v2)
{
	return (v1 < v2) ? v1 : v2;
}

/*!
	\brief Returns the maximum of \a v1 and \a v2
*/
template <class T>
const T& Max(const T& v1, const T& v2)
{
	return (v1 > v2) ? v1 : v2;
}

/*!
	\brief Clamps \a val to the range [\a min_bound, \a max_bound].
*/
template <class T>
const T& Clamp(const T& val, const T& min_bound, const T& max_bound)
{
	return Min(Max(val, min_bound), max_bound);
}

/*!
	\brief Linearly interpolates between \a begin and \a end.
	\note \a t must be in the range [0, 1].
*/
template <class T>
T Lerp(const T& begin, const T& end, float t)
{
	return begin + t * (end - begin);
}

/*!
	\brief Returns whether \a val is in the range [\a min_val, \a max_val].
*/
template <class T>
bool Between(const T& val, const T& min_val, const T& max_val)
{
	return val >= min_val && val <= max_val;
}

#define INIT_HASH64 14695981039346656037ULL
#define INIT_HASH32 2166136261U

template <class T>
inline unsigned long long FNV1aHash64T(const T* value, unsigned long long init = INIT_HASH64)
{
	return FNV1aHash64(reinterpret_cast<const char*>(value), sizeof(T), init);
}

template <class T>
inline unsigned long long FNV1Hash64T(const T* value, unsigned long long init = INIT_HASH64)
{
	return FNV1Hash64(reinterpret_cast<const char*>(value), sizeof(T), init);
}

template <class T>
inline unsigned int FNV1aHash32T(const T* value, unsigned int init = INIT_HASH32)
{
	return FNV1aHash32(reinterpret_cast<const char*>(value), sizeof(T), init);
}

template <class T>
inline unsigned int FNV1Hash32T(const T* value, unsigned int init = INIT_HASH32)
{
	return FNV1Hash32(reinterpret_cast<const char*>(value), sizeof(T), init);
}

typedef unsigned long long (*HashFunc64)(const char* data, size_t len);
typedef unsigned int (*HashFunc32)(const char* data, size_t len);

unsigned long long FNV1aHash64(const char* key, size_t len, unsigned long long init = INIT_HASH64);
unsigned long long FNV1Hash64(const char* key, size_t len, unsigned long long init = INIT_HASH64);
unsigned int FNV1aHash32(const char* key, size_t len, unsigned int init = INIT_HASH32);
unsigned int FNV1Hash32(const char* key, size_t len, unsigned int init = INIT_HASH32);

// These functions assume just call the above functiosn with INIT_HASH64/32
INLINE unsigned long long FNV1aHash64Wrapper(const char* key, size_t len);
INLINE unsigned long long FNV1Hash64Wrapper(const char* key, size_t len);
INLINE unsigned int FNV1aHash32Wrapper(const char* key, size_t len);
INLINE unsigned int FNV1Hash32Wrapper(const char* key, size_t len);

float BinomialCoefficient(float n, float k);
float BernsteinPolynomial(float t, float n, float k);

//static constexpr unsigned long long FNVBasis64 = 14695981039346656037ULL;
//static constexpr unsigned long long FNVPrime64 = 1099511628211ULL;
//static constexpr unsigned int FNVBasis32 = 2166136261U;
//static constexpr unsigned int FNVPrime32 = 16777619U;

//constexpr unsigned long long FNV1aHash64Const(const char* key, unsigned long long value = FNVBasis64)
//{
//	return (*key) ? FNV1aHash64Const(key + 1, (value ^ (unsigned long long)*key) * FNVPrime64) : value;
//}

NS_END
