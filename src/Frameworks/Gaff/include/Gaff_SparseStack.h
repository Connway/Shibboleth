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

#include "Gaff_DefaultAllocator.h"
#include "Gaff_BitVector.h"
#include "Gaff_Vector.h"
#include "Gaff_Assert.h"
#include "Gaff_Math.h"

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class SparseStack final
{
public:
	class Iterator final
	{
	public:
		Iterator(const Iterator& iterator) = default;
		Iterator(void) = default;

		Iterator& operator=(const Iterator& rhs) = default;

		SparseStack* getStack(void) const;
		int32_t getIndex(void) const;
		bool valid(void) const;

		Iterator operator+(int32_t offset) const;
		Iterator& operator+=(int32_t offset);
		Iterator operator-(int32_t offset) const;
		Iterator& operator-=(int32_t offset);

		Iterator operator++(int);
		Iterator& operator++();

		Iterator operator--(int);
		Iterator& operator--();

		bool operator==(const Iterator& rhs) const;
		bool operator!=(const Iterator& rhs) const;
		bool operator<=(const Iterator& rhs) const;
		bool operator<(const Iterator& rhs) const;
		bool operator>=(const Iterator& rhs) const;
		bool operator>(const Iterator& rhs) const;

		const T* operator->(void) const;
		T* operator->(void);

		T& operator*(void) const;
		T& operator*(void);

	private:
		SparseStack*  _stack = nullptr;
		int32_t _index = -1;

		Iterator(SparseStack* stack, int32_t index);

		friend class SparseStack;
	};

	class ConstIterator final
	{
	public:
		ConstIterator(const ConstIterator& iterator):
			_iterator(iterator._iterator)
		{
		}

		ConstIterator(const Iterator& iterator):
			_iterator(iterator)
		{
		}

		ConstIterator(void) = default;

		ConstIterator& operator=(const ConstIterator& rhs) = default;
		ConstIterator& operator=(const Iterator& rhs) { _iterator = rhs; return *this; }

		const SparseStack* getStack(void) const { return _iterator.getStack(); }
		int32_t getIndex(void) const { return _iterator.getIndex(); }
		bool valid(void) const { return _iterator.valid(); }

		ConstIterator operator+(int32_t offset) const { return ConstIterator(_iterator + offset); }
		ConstIterator& operator+=(int32_t offset) { _iterator += offset; return *this; }
		ConstIterator operator-(int32_t offset) const { return ConstIterator(_iterator - offset); }
		ConstIterator& operator-=(int32_t offset) { _iterator -= offset; return *this; }

		ConstIterator operator++(int) { return ConstIterator(_iterator++); }
		ConstIterator& operator++() { ++_iterator; return *this; }

		ConstIterator operator--(int) { return ConstIterator(_iterator--); }
		ConstIterator& operator--() { --_iterator; return *this; }

		bool operator==(const ConstIterator& rhs) const { return _iterator == rhs._iterator; }
		bool operator!=(const ConstIterator& rhs) const { return _iterator != rhs._iterator; }
		bool operator<=(const ConstIterator& rhs) const { return _iterator <= rhs._iterator; }
		bool operator<(const ConstIterator& rhs) const { return _iterator < rhs._iterator; }
		bool operator>=(const ConstIterator& rhs) const { return _iterator >= rhs._iterator; }
		bool operator>(const ConstIterator& rhs) const { return _iterator > rhs._iterator; }

		bool operator==(const Iterator& rhs) const { return _iterator == rhs; }
		bool operator!=(const Iterator& rhs) const { return _iterator != rhs; }
		bool operator<=(const Iterator& rhs) const { return _iterator <= rhs; }
		bool operator<(const Iterator& rhs) const { return _iterator < rhs; }
		bool operator>=(const Iterator& rhs) const { return _iterator >= rhs; }
		bool operator>(const Iterator& rhs) const { return _iterator > rhs; }

		const T* operator->(void) const { return &(*_iterator); }
		const T& operator*(void) const { return *_iterator; }

	private:
		Iterator _iterator;

		friend class SparseStack;
	};


	SparseStack(size_t initial_capacity, const Allocator& allocator = Allocator());
	SparseStack(const Allocator& allocator = Allocator());

	template <class... Args>
	int32_t emplace(Args&&... args);

	int32_t push(const T& value);
	int32_t push(T&& value);

	void remove(const T& value);
	void removeAt(int32_t index);

	void pop(void);

	// $TODO: Implement compact() function.

	bool validIndex(int32_t index) const;
	bool empty(void) const;

	template <class U, class Predicate>
	int32_t find(const U& value, Predicate predicate) const;

	template <class U>
	int32_t find(const U& value) const;

	int32_t getValidSize(void) const;

	ConstIterator begin(void) const;
	ConstIterator end(void) const;

	Iterator begin(void);
	Iterator end(void);

	const T& operator[](int32_t index) const;
	T& operator[](int32_t index);

private:
	BitVector<Allocator> _free_indices;
	Vector<T, Allocator> _data;

	int32_t _first_free_index = -1;
	int32_t _free_index_count = 0;

	int32_t getPreviousIndex(int32_t index) const;
	int32_t getNextIndex(int32_t index) const;
	int32_t allocateIndex(void);
};

#include "Gaff_SparseStack.inl"

NS_END
