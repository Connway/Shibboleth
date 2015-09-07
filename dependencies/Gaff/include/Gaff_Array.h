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

/*! \file */

#pragma once

#include "Gaff_DefaultAllocator.h"
#include "Gaff_IncludeAssert.h"
#include "Gaff_Predicates.h"
#include <cstring>

NS_GAFF

/*!
	\brief A dynamically allocated array. Similar to std::vector.

	\tparam T The type the array will store.
	\tparam Allocator The allocator we will use to create/resize/free the array.

	\note push()/pop() operates on the end of the array. (LIFO)
*/
template <class T, class Allocator = DefaultAllocator>
class Array
{
public:
	typedef T* Iterator;

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
	void moveAppend(Array<T, Allocator>&& array);

	void movePush(T&& data);
	void push(const T& data);
	void pop(void);

	template <class... Args>
	Iterator emplaceInsert(const Iterator it, Args&&... args);

	template <class... Args>
	void emplaceInsert(size_t index, Args&&... args);

	template <class... Args>
	Iterator emplaceMoveInsert(const Iterator it, Args&&... args);

	template <class... Args>
	void emplaceMoveInsert(size_t index, Args&&... args);

	template <class... Args>
	void emplacePush(Args&&... args);

	template <class... Args>
	void emplaceMovePush(Args&&... args);

	/*!
		\brief Inserts \a data to the position \a it using the move constructor.

		\param data The data we are moving into the array.
		\param it The position we are inserting \a data into.

		\return Returns an iterator to the newly inserted element.

		\note This is the same as calling moveInsert(\a data, \a it - begin()).
	*/
	Iterator moveInsert(T&& data, const Iterator it);
	void moveInsert(T&& data, size_t index);

	/*!
		\brief Inserts \a data to the position \a it using the copy constructor.

		\param data The data we are moving into the array.
		\param it The position we are inserting \a data into.

		\return Returns an iterator to the newly inserted element.

		\note This is the same as calling insert(\a data, \a it - begin()).
	*/
	Iterator insert(const T& data, const Iterator it);
	void insert(const T& data, size_t index);

	/*!
		Removes the element at position \a it. Shifts all elements above the removed position down one.

		\brief Removes the element at position \a it.

		\param it The position to remove.

		\return An iterator to the element that occupies our old position.

		\note This is the same as calling erase(\a it - begin()).
	*/
	Iterator erase(const Iterator it);
	void erase(size_t index);

	/*!
		Removes the element at position \a it.
		It avoids shifting by copying the last element into the erased position.

		\brief Removes the element at position \a it without shifting.

		\param it The position to remove.

		\return An iterator to the element that occupies our old position.

		\note This is the same as calling fastErase(\a it - begin()). Avoid using this function if your array must be sorted!
	*/
	Iterator fastErase(const Iterator it);
	void fastErase(size_t index);

	void resize(size_t new_size, const T& init_val);
	void resize(size_t new_size);
	void reserve(size_t reserve_size);
	void trim(void);

	/*!
		\brief Linearly searches each element in the range until the predicate is satisfied.

		\param range_begin The start of the range to search.
		\param range_end The end of the range to search.
		\param data The data we are searching for.
		\param pred The predicate we must satisfy in order to complete the search.

		\tparam T2 The type of the data we are searching for.
		\tparam Pred The type of the predicate we are using to satisfy our search. Must overload operator()!

		\return Returns an iterator to an element if the predicate is satisfied, otherwise returns end().

		\note \a Pred defaults to Less if no predicate is given. Search range is [range_begin, range_end), non-inclusive.
	*/
	template < class T2, class Pred = Equal<T, T2> >
	Iterator linearSearch(const Iterator range_begin, const Iterator range_end, const T2& data, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Equal<T, T2> >
	size_t linearSearch(size_t range_begin, size_t range_end, const T2& data, const Pred& pred = Pred()) const;

	/*!
		\brief Linearly searches each element until the predicate is satisfied.

		\param data The data we are searching for.
		\param pred The predicate we must satisfy in order to complete the search.

		\tparam T2 The type of the data we are searching for.
		\tparam Pred The type of the predicate we are using to satisfy our search. Must overload operator()!

		\return Returns an iterator to an element if the predicate is satisfied, otherwise returns end().

		\note This is the same as calling linearSearch(begin(), end(), \a data, \a pred).
	*/
	template < class T2, class Pred = Equal<T, T2> >
	Iterator linearSearch(const T2& data, const Pred& pred = Pred()) const;

	/*!
		\brief Does a binary search over the range.

		\param range_begin The start of the range to search.
		\param range_end The end of the range to search.
		\param data The data we are searching for.
		\param pred The predicate used by the binary search to determine which direction it should move in.

		\tparam T2 The type of the data we are searching for.
		\tparam Pred The type of the predicate we are using to satisfy our search. Must overload operator() if a functor!

		\return An iterator to the found element. If the element is not found, the iterator is where \a data should be inserted.

		\note The array must be sorted with the predicate used to search. Search range is [\a range_begin, \a range_end), non-inclusive.
	*/
	template < class T2, class Pred = Less<T, T2> >
	Iterator binarySearch(const Iterator range_begin, const Iterator range_end, const T2& data, const Pred& pred = Pred()) const;

	template < class T2, class Pred = Less<T, T2> >
	size_t binarySearch(size_t range_begin, size_t range_end, const T2& data, const Pred& pred = Pred()) const;

	/*!
		\brief Does a binary search over the entire array.

		\param data The data we are searching for.
		\param pred The predicate used by the binary search to determine which direction it should move in.

		\tparam T2 The type of the data we are searching for.
		\tparam Pred The type of the predicate we are using to satisfy our search. Must overload operator() if a functor!

		\return An iterator to the found element. If the element is not found, the iterator is where \a data should be inserted.

		\note The array must be sorted with the predicate used to search. Search range is [range_begin, range_end), non-inclusive. This is the same as calling binarySearch(begin(), end(), \a data, \a pred).
	*/
	template < class T2, class Pred = Less<T, T2> >
	Iterator binarySearch(const T2& data, const Pred& pred = Pred()) const;

	size_t size(void) const;
	size_t capacity(void) const;

	void setAllocator(const Allocator& allocator);

private:
	Allocator _allocator;
	T* _array;
	size_t _used, _size;
};

#include "Gaff_Array.inl"

NS_END
