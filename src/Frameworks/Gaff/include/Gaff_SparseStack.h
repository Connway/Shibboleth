/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
#include "Gaff_Vector.h"

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class SparseStack final
{
public:
	//class Iterator final
	//{
	//public:
	//	Iterator operator++(int);
	//	Iterator& operator++();

	//	Iterator operator--(int);
	//	Iterator& operator--();

	//private:

	//	int32_t _index;
	//};

	SparseStack(size_t initial_capacity, const Allocator& allocator = Allocator());
	SparseStack(const Allocator& allocator = Allocator());

	template <class... Args>
	int32_t emplace(Args&&... args);

	int32_t push(const T& value);
	int32_t push(T&& value);

	void remove(const T& value);
	void remove(int32_t index);

	void pop(void);

	int32_t find(const T& value) const;

	//int32_t size(void) const;

	//const T& operator[](int32_t index) const;
	//T& operator[](int32_t index);

	//ConstIterator begin(void) const;
	//ConstIterator end(void) const;

	//Iterator begin(void);
	//Iterator end(void);

private:
	Vector<int32_t, Allocator> _free_indices;
	Vector<T, Allocator> _data;

	int32_t allocateIndex(void);
};

#include "Gaff_SparseStack.inl"

NS_END
