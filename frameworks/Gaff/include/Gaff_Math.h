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
const float DegToRad = Pi / 180.0f;
const float RadToDeg = 180.0f / Pi;

template <class T>
const T& Min(const T& v1, const T& v2)
{
	return (v1 < v2) ? v1 : v2;
}

template <class T>
const T& Max(const T& v1, const T& v2)
{
	return (v1 > v2) ? v1 : v2;
}

template <class T>
const T& Clamp(const T& val, const T& min_bound, const T& max_bound)
{
	return Min(Max(val, min_bound), max_bound);
}

template <class T>
T Lerp(const T& begin, const T& end, float t)
{
	return begin + t * (end - begin);
}

template <class T>
bool Between(const T& val, const T& min_val, const T& max_val)
{
	return val >= min_val && val <= max_val;
}

NS_END
