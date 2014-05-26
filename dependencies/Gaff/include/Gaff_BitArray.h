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

#pragma once

#include "Gaff_DefaultAllocator.h"
#include "Gaff_IncludeAssert.h"
#include "Gaff_Array.h"
#include <cmath>

NS_GAFF

template <class Allocator = DefaultAllocator>
class BitArray
{
public:
	BitArray(const Allocator& allocator = Allocator());
	explicit BitArray(unsigned int start_alloc, const Allocator& allocator = Allocator());
	BitArray(unsigned int start_alloc, bool init_val, const Allocator& allocator = Allocator());
	BitArray(const BitArray<Allocator>& rhs);
	BitArray(BitArray<Allocator>&& rhs);
	~BitArray(void);

	//template <class Allocator2>
	//const BitArray<Allocator>& operator=(const BitArray<Allocator2>& rhs);

	const BitArray<Allocator>& operator=(const BitArray<Allocator>& rhs);
	const BitArray<Allocator>& operator=(BitArray<Allocator>&& rhs);

	template <class Allocator2>
	bool operator==(const BitArray<Allocator2>& rhs) const;

	template <class Allocator2>
	bool operator!=(const BitArray<Allocator2>& rhs) const;

	bool operator[](unsigned int index) const;

	void setBit(unsigned int index, bool value);

	void unset(unsigned int index);
	void set(unsigned int index);

	void clear(void);
	void clearNoFree(void);

	bool first(void) const;
	bool last(void) const;

	bool empty(void) const;

	unsigned int size(void) const;
	unsigned int capacity(void) const;

	void push(bool value);
	void pop(void);
	void insert(bool value, unsigned int index);
	void erase(unsigned int index);

	void resize(unsigned int new_size);
	void reserve(unsigned int reserve_size);

	void setAllocator(const Allocator& allocator);

private:
	Array<unsigned char, Allocator> _bit_array;
	unsigned int _used;
	unsigned int _size;

	static void CalculateIndexAndShift(unsigned int index, unsigned int& array_index, unsigned int& shift);
	static unsigned int CalculateBytes(unsigned int bits);
};

#include "Gaff_BitArray.inl"

NS_END
