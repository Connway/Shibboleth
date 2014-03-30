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
#include "Gaff_Predicates.h"
#include <cstring>

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class Array
{
public:
	class Iterator
	{
	public:
		Iterator(const Iterator& it);
		Iterator(void);

		const Iterator& operator++(void) const;
		const Iterator& operator--(void) const;
		Iterator operator++(int) const;
		Iterator operator--(int) const;

		const Iterator& operator+=(int rhs) const;
		const Iterator& operator-=(int rhs) const;
		Iterator operator+(int rhs) const;
		Iterator operator-(int rhs) const;

		OffsetType operator+(const Iterator& rhs) const;
		OffsetType operator-(const Iterator& rhs) const;

		bool operator==(const Iterator& rhs) const;
		bool operator!=(const Iterator& rhs) const;

		const Iterator& operator=(const Iterator& rhs) const;

		const T& operator*(void) const;
		T& operator*(void);

		const T* operator->(void) const;
		T* operator->(void);

	private:
		Iterator(T* element);

		mutable T* _element;
		friend class Array<T, Allocator>;
	};

	explicit Array(const Allocator& allocator = Allocator());
	explicit Array(unsigned int start_alloc, const Allocator& allocator = Allocator());
	Array(unsigned int start_alloc, const T& init_val, const Allocator& allocator = Allocator());
	Array(unsigned int size, const T* data, const Allocator& allocator = Allocator());
	Array(const Array<T, Allocator>& rhs);
	Array(Array<T, Allocator>&& rhs);
	~Array(void);

	// For some reason, this is compiling, but debugger says it's using the old version of operator=? ... what
	//template <class Allocator2>
	//const Array<T, Allocator>& operator=(const Array<T, Allocator2>& rhs);

	const Array<T, Allocator>& operator=(const Array<T, Allocator>& rhs);
	const Array<T, Allocator>& operator=(Array<T, Allocator>&& rhs);

	template <class Allocator2>
	bool operator==(const Array<T, Allocator2>& rhs) const;

	template <class Allocator2>
	bool operator!=(const Array<T, Allocator2>& rhs) const;

	const T& operator[](unsigned int index) const;
	T& operator[](unsigned int index);

	void clear(void);
	void clearNoFree(void);

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

	bool empty(void) const;

	void push(T&& data);
	void push(const T& data);
	void push(T& data);
	void pop(void);
	Iterator insert(const T& data, const Iterator& it);
	void insert(const T& data, unsigned int index);
	Iterator erase(const Iterator& it);
	void erase(unsigned int index);

	void resize(unsigned int new_size);
	void reserve(unsigned int reserve_size);

	template < class T2, class Pred = Equal<T, T2> >
	Iterator linearSearch(const Iterator& range_beg, const Iterator& range_end, const T2& data, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Equal<T, T2> >
	int linearSearch(unsigned int range_beg, unsigned int range_end, const T2& data, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Equal<T, T2> >
	Iterator linearSearch(const T2& data, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Less<T, T2> >
	Iterator binarySearch(const Iterator& range_beg, const Iterator& range_end, const T2& data, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Less<T, T2> >
	int binarySearch(unsigned int range_beg, unsigned int range_end, const T2& data, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Less<T, T2> >
	Iterator binarySearch(const T2& data, const Pred& pred = Pred()) const;

	unsigned int size(void) const;
	unsigned int capacity(void) const;

private:
	Allocator _allocator;
	T* _array;
	unsigned int _used, _size;
};

#include "Gaff_Array.inl"

NS_END
