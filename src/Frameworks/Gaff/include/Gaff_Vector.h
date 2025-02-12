/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#ifndef EASTL_VECTOR_DEFAULT_NAME
	#define EASTL_VECTOR_DEFAULT_NAME "Untagged"
#endif

#include "Gaff_DefaultAllocator.h"
#include <EASTL/vector.h>
#include <EASTL/sort.h>

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
using Vector = eastl::vector<T, Allocator>;

template <class T, class V, class Allocator, class Predicate>
typename Vector<T, Allocator>::const_iterator LowerBound(const Vector<T, Allocator>& vec, const V& value, Predicate predicate)
{
	return eastl::lower_bound(vec.begin(), vec.end(), value, predicate);
}

template <class T, class V, class Allocator, class Predicate>
typename Vector<T, Allocator>::iterator LowerBound(Vector<T, Allocator>& vec, const V& value, Predicate predicate)
{
	return eastl::lower_bound(vec.begin(), vec.end(), value, predicate);
}

template <class T, class V, class Allocator>
typename Vector<T, Allocator>::const_iterator LowerBound(const Vector<T, Allocator>& vec, const V& value)
{
	return eastl::lower_bound(vec.begin(), vec.end(), value);
}

template <class T, class V, class Allocator>
typename Vector<T, Allocator>::iterator LowerBound(Vector<T, Allocator>& vec, const V& value)
{
	return eastl::lower_bound(vec.begin(), vec.end(), value);
}

template <class T, class V, class Allocator, class Predicate>
typename Vector<T, Allocator>::const_iterator UpperBound(const Vector<T, Allocator>& vec, const V& value, Predicate predicate)
{
	return eastl::upper_bound(vec.begin(), vec.end(), value, predicate);
}

template <class T, class V, class Allocator, class Predicate>
typename Vector<T, Allocator>::iterator UpperBound(Vector<T, Allocator>& vec, const V& value, Predicate predicate)
{
	return eastl::upper_bound(vec.begin(), vec.end(), value, predicate);
}

template <class T, class V, class Allocator>
typename Vector<T, Allocator>::const_iterator UpperBound(const Vector<T, Allocator>& vec, const V& value)
{
	return eastl::upper_bound(vec.begin(), vec.end(), value);
}

template <class T, class V, class Allocator>
typename Vector<T, Allocator>::iterator UpperBound(Vector<T, Allocator>& vec, const V& value)
{
	return eastl::upper_bound(vec.begin(), vec.end(), value);
}

template <class T, class V, class Allocator, class Predicate>
typename Vector<T, Allocator>::const_iterator Find(const Vector<T, Allocator>& vec, const V& value, Predicate predicate)
{
	return eastl::find(vec.begin(), vec.end(), value, predicate);
}

template <class T, class V, class Allocator, class Predicate>
typename Vector<T, Allocator>::iterator Find(Vector<T, Allocator>& vec, const V& value, Predicate predicate)
{
	return eastl::find(vec.begin(), vec.end(), value, predicate);
}

template <class T, class V, class Allocator>
typename Vector<T, Allocator>::const_iterator Find(const Vector<T, Allocator>& vec, const V& value)
{
	return eastl::find(vec.begin(), vec.end(), value);
}

template <class T, class V, class Allocator>
typename Vector<T, Allocator>::iterator Find(Vector<T, Allocator>& vec, const V& value)
{
	return eastl::find(vec.begin(), vec.end(), value);
}

template <class T, class Allocator>
void Sort(Vector<T, Allocator>& vec)
{
	return eastl::sort(vec.begin(), vec.end());
}

NS_END
