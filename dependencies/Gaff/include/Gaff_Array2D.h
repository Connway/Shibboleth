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

#include "Gaff_DefaultAllocator.h"
#include "Gaff_IncludeAssert.h"
#include <cstring>

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class Array2D
{
public:
	using RowIterator = T*;

	explicit Array2D(const Allocator& allocator = Allocator());
	Array2D(size_t width, size_t height, T& init_val, const Allocator& allocator = Allocator());
	Array2D(size_t width, size_t height, const Allocator& allocator = Allocator());

	Array2D(const T** data, size_t width, size_t height, const Allocator& allocator = Allocator());
	// Assumes contiguous array is width * height in size.
	Array2D(const T* data, size_t width, size_t height, const Allocator& allocator = Allocator());

	Array2D(const Array2D<T, Allocator>& rhs);
	Array2D(Array2D<T, Allocator>&& rhs);

	~Array2D(void);

	const Array2D<T, Allocator>& operator=(const Array2D<T, Allocator>& rhs);
	const Array2D<T, Allocator>& operator=(Array2D<T, Allocator>&& rhs);

	// operator is used as array[y][x] for contiguous access
	const T* operator[](size_t index) const;
	T* operator[](size_t index);

	void clear(void);

	void resize(size_t width, size_t height);

	const T& at(size_t x, size_t y) const;
	T& at(size_t x, size_t y);

	size_t width(void) const;
	size_t height(void) const;

private:
	T* _array;
	size_t _width;
	size_t _height;

	Allocator _allocator;
};

#include "Gaff_Array2D.inl"

NS_END
