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

#pragma once

#ifndef EASTL_VECTOR_DEFAULT_NAME
	#define EASTL_VECTOR_DEFAULT_NAME "Untagged"
#endif

#include "Gaff_DefaultAllocator.h"
#include <EASTL/vector.h>

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
using Vector = eastl::vector<T, Allocator>;

template <class T, class Allocator>
bool EmplaceBackUnique(Vector<T, Allocator>& vec, T&& value)
{
	if (Find(vec, value) == vec.end()) {
		vec.emplace_back(std::move(value));
		return true;
	}

	return false;
}

template <class T, class Allocator>
bool PushBackUnique(Vector<T, Allocator>& vec, const T& value)
{
	if (Find(vec, value) == vec.end()) {
		vec.push_back(value);
		return true;
	}

	return false;
}

NS_END
