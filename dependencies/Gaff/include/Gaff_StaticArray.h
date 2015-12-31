/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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
#include "Gaff_IncludeAssert.h"

NS_GAFF

template <class T, size_t array_size>
class StaticArray
{
public:
	static_assert(array_size > 0, "StaticArray size must be larger than zero.");
	using Iterator = T*;

	StaticArray(const T& init_val);
	StaticArray(const StaticArray<T, array_size>& rhs);
	StaticArray(StaticArray<T, array_size>&& rhs);
	StaticArray(void);
	~StaticArray(void);

	const StaticArray<T, array_size>& operator=(const StaticArray<T, array_size>& rhs);
	const StaticArray<T, array_size>& operator=(StaticArray<T, array_size>&& rhs);

	bool operator==(const StaticArray<T, array_size>& rhs) const;
	bool operator!=(const StaticArray<T, array_size>& rhs) const;

	const T& operator[](size_t index) const;
	T& operator[](size_t index);

	const T& first(void) const;
	T& first(void);

	const T& last(void) const;
	T& last(void);

	const T* getArray(void) const;
	T* getArray(void);

	Iterator begin(void) const;
	Iterator end(void) const;
	Iterator rbegin(void) const;
	Iterator rend(void) const;

	size_t size(void) const;

private:
	T _array[array_size];
};

#include "Gaff_StaticArray.inl"

NS_END
