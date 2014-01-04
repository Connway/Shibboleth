/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#ifdef min
	#undef min
#endif

#ifdef max
	#undef max
#endif

NS_GAFF

const float Pi = 3.141592654f;

template <class T>
T min(T v1, T v2)
{
	return (v1 < v2) ? v1 : v2;
}

template <class T>
T max(T v1, T v2)
{
	return (v1 > v2) ? v1 : v2;
}

template <class T>
T clamp(T val, T min_bound, T max_bound)
{
	return min(max(val, min_bound), max_bound);
}

template <class T>
T lerp(T begin, T end, float t)
{
	return begin + t * (end - begin);
}

// Not really sure this is a good candidate for inlining,
// but I don't want to have a compiled cpp file for Gaff_Math.h.
// Inlining this removes redefinition linker warning.
inline unsigned int FNVHash (const char* key, int len)
{
	unsigned int hash = 2166136261;

	for (int i = 0; i < len; ++i) {
		hash = (hash * 16777619) ^ key[i];
	}

	return hash;
}

typedef unsigned int (*HashFunc)(const char* data, int len);

NS_END
