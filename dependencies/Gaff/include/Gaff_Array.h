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
#include "Gaff_Predicates.h"
#include <cstring>

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class Array
{
public:
	using Iterator = T*;

	explicit Array(const Allocator& allocator = Allocator());
	explicit Array(size_t start_capacity, const Allocator& allocator = Allocator());
	Array(size_t start_size, const T& init_val, const Allocator& allocator = Allocator());
	Array(const T* data, size_t size, const Allocator& allocator = Allocator());
	Array(const Array<T, Allocator>& rhs);
	Array(Array<T, Allocator>&& rhs);
	~Array(void);

	const Array<T, Allocator>& operator=(const Array<T, Allocator>& rhs);
	const Array<T, Allocator>& operator=(Array<T, Allocator>&& rhs);

	template <class Allocator2>
	bool operator==(const Array<T, Allocator2>& rhs) const;

	template <class Allocator2>
	bool operator!=(const Array<T, Allocator2>& rhs) const;

	const T& operator[](size_t index) const;
	T& operator[](size_t index);

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

	void append(const T* array, size_t count);
	void append(const Array<T, Allocator>& array);
	void append(Array<T, Allocator>&& array);

	void push(T&& data);
	void push(const T& data);
	void pop(void);

	template <class... Args>
	Iterator emplace(const Iterator it, Args&&... args);

	template <class... Args>
	void emplace(size_t index, Args&&... args);

	template <class... Args>
	void emplacePush(Args&&... args);

	Iterator insert(const Iterator it, const T& data);
	Iterator insert(const Iterator it, T&& data);
	void insert(size_t index, const T& data);
	void insert(size_t index, T&& data);

	Iterator erase(const Iterator it);
	void erase(size_t index);

	Iterator fastErase(const Iterator it);
	void fastErase(size_t index);

	void resizeFast(size_t new_size, const T& init_val);
	void resizeFast(size_t new_size);
	void resize(size_t new_size, const T& init_val);
	void resize(size_t new_size);
	void reserve(size_t reserve_size);
	void trim(void);

	template < class T2, class Pred = Equal<T, T2> >
	const Iterator linearSearch(const Iterator range_begin, const Iterator range_end, const T2& value, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Equal<T, T2> >
	Iterator linearSearch(Iterator range_begin, Iterator range_end, const T2& value, const Pred& pred = Pred());

	template < class T2, class Pred = Equal<T, T2> >
	size_t linearSearch(size_t range_begin, size_t range_end, const T2& value, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Equal<T, T2> >
	const Iterator linearSearch(const T2& value, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Equal<T, T2> >
	Iterator linearSearch(const T2& value, const Pred& pred = Pred());

	template < class T2, class Pred = Less<T, T2> >
	const Iterator binarySearch(const Iterator range_begin, const Iterator range_end, const T2& value, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Less<T, T2> >
	Iterator binarySearch(Iterator range_begin, Iterator range_end, const T2& value, const Pred& pred = Pred());

	template < class T2, class Pred = Less<T, T2> >
	size_t binarySearch(size_t range_begin, size_t range_end, const T2& value, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Less<T, T2> >
	const Iterator binarySearch(const T2& value, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Less<T, T2> >
	Iterator binarySearch(const T2& value, const Pred& pred = Pred());

	size_t size(void) const;
	size_t capacity(void) const;

	void setAllocator(const Allocator& allocator);

private:
	Allocator _allocator;
	T* _array;
	size_t _used, _size;

	void resizeHelper(size_t new_size, const T& init_val);
	void resizeHelper(size_t new_size);
};

template < class T, class T2, class Pred = Equal<T, T2> >
const T* LinearSearch(const T* range_begin, const T* range_end, const T2& value, const Pred& pred = Pred());

template < class T, class T2, class Pred = Equal<T, T2> >
T* LinearSearch(T* range_begin, T* range_end, const T2& value, const Pred& pred = Pred());

template < class T, class T2, class Pred = Equal<T, T2> >
size_t LinearSearch(const T* data, size_t range_begin, size_t range_end, const T2& value, const Pred& pred = Pred());

template < class T, class T2, class Pred = Less<T, T2> >
const T* BinarySearch(const T* range_begin, const T* range_end, const T2& value, const Pred& pred = Pred());

template < class T, class T2, class Pred = Less<T, T2> >
T* BinarySearch(T* range_begin, T* range_end, const T2& value, const Pred& pred = Pred());

template < class T, class T2, class Pred = Less<T, T2> >
size_t BinarySearch(const T* data, size_t range_begin, size_t range_end, const T2& value, const Pred& pred = Pred());

#include "Gaff_Array.inl"

NS_END
