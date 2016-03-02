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
#include "Gaff_Assert.h"
#include "Gaff_Array.h"
#include <cmath>

NS_GAFF

/*!
	\brief An array implementation for storing bits.
	\tparam Allocator The allocator we will use to allocate memory.
*/
template <class Allocator = DefaultAllocator>
class BitArray
{
public:
	BitArray(const Allocator& allocator = Allocator());
	explicit BitArray(size_t start_size, const Allocator& allocator = Allocator());
	BitArray(size_t start_size, bool init_val, const Allocator& allocator = Allocator());
	BitArray(const BitArray<Allocator>& rhs);
	BitArray(BitArray<Allocator>&& rhs);
	~BitArray(void);

	const BitArray<Allocator>& operator=(const BitArray<Allocator>& rhs);
	const BitArray<Allocator>& operator=(BitArray<Allocator>&& rhs);

	bool operator==(const BitArray<Allocator>& rhs) const;
	bool operator!=(const BitArray<Allocator>& rhs) const;

	bool operator[](size_t index) const;

	void setBit(size_t index, bool value);

	void unset(size_t index);
	void set(size_t index);

	void clear(void);
	void clearNoFree(void);

	bool first(void) const;
	bool last(void) const;

	bool empty(void) const;

	size_t size(void) const;
	size_t capacity(void) const;

	void push(bool value);
	void pop(void);
	void insert(bool value, size_t index);
	void erase(size_t index);

	void resize(size_t new_size);
	void reserve(size_t reserve_size);

	void setAllocator(const Allocator& allocator);

private:
	Array<unsigned char, Allocator> _bit_array;
	size_t _used;
	size_t _size;

	static void CalculateIndexAndShift(size_t index, size_t& array_index, size_t& shift);
	static size_t CalculateBytes(size_t bits);
};

#include "Gaff_BitArray.inl"

NS_END
