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

#define ARRAY_ITERATOR typename Array<T, Allocator>::Iterator

template <class T, class Allocator>
Array<T, Allocator>::Array(const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(0), _size(0)
{
}

/*!
	\brief Initializes the array to capacity \a start_capacity.

	\param start_capacity The starting capacity of the array.

	\note
		This constructor only affects the capacity of the array.
		The size will still be zero.
*/
template <class T, class Allocator>
Array<T, Allocator>::Array(size_t start_capacity, const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(0), _size(start_capacity)
{
	_array = (T*)_allocator.alloc(sizeof(T) * start_capacity);
}

/*!
	\brief Initializes the array to size \a start_size with each element's value set to \a init_val.

	\param start_size The starting size of the array.
	\param init_val The value each element will be initialized with.
*/
template <class T, class Allocator>
Array<T, Allocator>::Array(size_t start_size, const T& init_val, const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(start_size), _size(start_size)
{
	_array = (T*)_allocator.alloc(sizeof(T) * start_size);

	for (size_t i = 0; i < start_size; ++i) {
		construct(_array + i, init_val);
	}
}

/*!
	\brief Initializes the array to \a size using the values in \a data.

	\param data The array of data we are initializing ourselves to.
	\param size The size of \a data.
*/
template <class T, class Allocator>
Array<T, Allocator>::Array(const T* data, size_t size, const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(size), _size(size)
{
	_array = (T*)_allocator.alloc(sizeof(T) * size);

	for (size_t i = 0; i < size; ++i) {
		construct(_array + i, data[i]);
	}
}

template <class T, class Allocator>
Array<T, Allocator>::Array(const Array<T, Allocator>& rhs):
	_allocator(rhs._allocator), _array(nullptr), _used(0), _size(0)
{
	*this = rhs;
}

template <class T, class Allocator>
Array<T, Allocator>::Array(Array<T, Allocator>&& rhs):
	_allocator(rhs._allocator), _array(rhs._array),
	_used(rhs._used), _size(rhs._size)
{
	rhs._used = rhs._size = 0;
	rhs._array = nullptr;
}

template <class T, class Allocator>
Array<T, Allocator>::~Array(void)
{
	clear();
}

template <class T, class Allocator>
const Array<T, Allocator>& Array<T, Allocator>::operator=(const Array<T, Allocator>& rhs)
{
	if (this == &rhs) {
		return *this;
	}

	if (rhs._size == 0) {
		clear();
		return *this;

	} else if (_size != rhs._size) {
		clear();
		_array = (T*)_allocator.alloc(sizeof(T) * rhs._size);
	}

	for (size_t i = 0; i < rhs._used; ++i) {
		construct(_array + i, rhs._array[i]);
	}

	_used = rhs._used;
	_size = rhs._size;

	return *this;
}

template <class T, class Allocator>
const Array<T, Allocator>& Array<T, Allocator>::operator=(Array<T, Allocator>&& rhs)
{
	if (this == &rhs) {
		return *this;
	}

	clear();

	_used = rhs._used;
	_size = rhs._size;
	_array = rhs._array;

	rhs._used = rhs._size = 0;
	rhs._array = nullptr;

	return *this;
}

template <class T, class Allocator>
template <class Allocator2>
bool Array<T, Allocator>::operator==(const Array<T, Allocator2>& rhs) const
{
	if (this == &rhs) {
		return true;
	}

	if (_used != rhs._used) {
		return false;
	}

	for (size_t i = 0; i < _used; ++i) {
		if (_array[i] != rhs._array[i]) {
			return false;
		}
	}

	return true;
}

template <class T, class Allocator>
template <class Allocator2>
bool Array<T, Allocator>::operator!=(const Array<T, Allocator2>& rhs) const
{
	return !(*this == rhs);
}

template <class T, class Allocator>
const T& Array<T, Allocator>::operator[](size_t index) const
{
	assert(index < _used);
	return _array[index];
}

template <class T, class Allocator>
T& Array<T, Allocator>::operator[](size_t index)
{
	assert(index < _used);
	return _array[index];
}

/*!
	Will call the destructor on all active elements and deallocates used memory.
	Size and capacity will be zero.

	\brief Removes all values in the array and frees all used memory.
*/
template <class T, class Allocator>
void Array<T, Allocator>::clear(void)
{
	if (_array) {
		for (size_t i = 0; i < _used; ++i) {
			deconstruct(_array + i);
		}

		_allocator.free((void*)_array);
		_used = _size = 0;
		_array = nullptr;
	}
}

/*!
	Will call the destructor on all active elements, but will hold onto its memory.
	Size will be zero, but capacity will remain the same.

	\brief Removes all values in the array, but keeps its memory.
*/
template <class T, class Allocator>
void Array<T, Allocator>::clearNoFree(void)
{
	if (_array) {
		for (size_t i = 0; i < _used; ++i) {
			deconstruct(_array + i);
		}

		_used = 0;
	}
}

template <class T, class Allocator>
const T& Array<T, Allocator>::first(void) const
{
	assert(_used);
	return _array[0];
}

template <class T, class Allocator>
T& Array<T, Allocator>::first(void)
{
	assert(_used);
	return _array[0];
}

template <class T, class Allocator>
const T& Array<T, Allocator>::last(void) const
{
	assert(_used);
	return _array[_used - 1];
}

template <class T, class Allocator>
T& Array<T, Allocator>::last(void)
{
	assert(_used);
	return _array[_used - 1];
}

/*!
	\brief Returns a raw pointer to the internal array.
	\note Array still owns the memory, do not delete!
*/
template <class T, class Allocator>
const T* Array<T, Allocator>::getArray(void) const
{
	return _array;
}

/*!
	\brief Returns a raw pointer to the internal array.
	\note Array still owns the memory, do not delete!
*/
template <class T, class Allocator>
T* Array<T, Allocator>::getArray(void)
{
	return _array;
}

/*!
	\brief Returns an iterator to the first element.
*/
template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::begin(void) const
{
	return Iterator(_array);
}

/*!
	\brief Returns an iterator to one past the last element.
*/
template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::end(void) const
{
	return Iterator(_array + _used);
}

/*!
	\brief Returns an iterator to the the last element.
	\note Since iterator is just a typdef for T*, reverse iterators must be traversed using operator--.
*/
template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::rbegin(void) const
{
	return Iterator(_array + _used - 1);
}

/*!
	\brief Returns an iterator to one before the first element.
	\note Since iterator is just a typdef for T*, reverse iterators must be traversed using operator--.
*/
template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::rend(void) const
{
	return Iterator(_array - 1);
}

template <class T, class Allocator>
bool Array<T, Allocator>::empty(void) const
{
	return _used == 0;
}

/*!
	\brief Appends the elements of \a array to the end of the array.
*/
template <class T, class Allocator>
void Array<T, Allocator>::append(const Array<T, Allocator>& array)
{
	reserve(_used + array.size());

	for (unsigned int i = 0; i < array.size(); ++i) {
		construct(_array + _used + i, array[i]);
	}

	_used += array.size();
}

/*!
	\brief Same as \a append(), but calls the move constructor on each element.
	\note \a array will have \a clear() called.
*/
template <class T, class Allocator>
void Array<T, Allocator>::moveAppend(Array<T, Allocator>&& array)
{
	reserve(_used + array.size());

	for (unsigned int i = 0; i < array.size(); ++i) {
		moveConstruct(_array + _used + i, Gaff::Move(array[i]));
	}

	_used += array.size();
	array.clear();
}

/*!
	\brief Pushes the data to the end of the array using the move constructor.
*/
template <class T, class Allocator>
void Array<T, Allocator>::movePush(T&& data)
{
	if (_used == _size) {
		if (_size == 0) {
			reserve(1);
		} else {
			reserve(_size * 2);
		}
	}

	moveConstruct(_array + _used, Move(data));
	++_used;
}

/*!
	\brief Pushes the data to the end of the array using the copy constructor.
*/
template <class T, class Allocator>
void Array<T, Allocator>::push(const T& data)
{
	if (_used == _size) {
		if (_size == 0) {
			reserve(1);
		} else {
			reserve(_size * 2);
		}
	}

	construct(_array + _used, data);
	++_used;
}

/*!
	\brief Removes the last element of the array.
*/
template <class T, class Allocator>
void Array<T, Allocator>::pop(void)
{
	assert(_used);
	deconstruct(_array + _used - 1);
	--_used;
}

template <class T, class Allocator>
template <class... Args>
ARRAY_ITERATOR Array<T, Allocator>::emplaceInsert(const ARRAY_ITERATOR it, Args&&... args)
{
	assert(it >= _array && it <= _array + _used);
	size_t index = static_cast<size_t>(it - _array);
	emplaceInsert(index, Move(args)...);
	return Iterator(_array + index);
}

template <class T, class Allocator>
template <class... Args>
void Array<T, Allocator>::emplaceInsert(size_t index, Args&&... args)
{
	assert(index <= _size);

	if (_used + 1 > _size) {
		reserve((_size == 0) ? 1 : _size * 2);
	}

	for (size_t i = _used; i > index; --i) {
		memcpy(_array + i, _array + i - 1, sizeof(T));
	}

	construct(_array + index, args...);
	++_used;
}

template <class T, class Allocator>
template <class... Args>
ARRAY_ITERATOR Array<T, Allocator>::emplaceMoveInsert(const ARRAY_ITERATOR it, Args&&... args)
{
	assert(it >= _array && it <= _array + _used);
	size_t index = static_cast<size_t>(it - _array);
	emplaceMoveInsert(index, Move(args)...);
	return Iterator(_array + index);
}

template <class T, class Allocator>
template <class... Args>
void Array<T, Allocator>::emplaceMoveInsert(size_t index, Args&&... args)
{
	assert(index <= _size);

	if (_used + 1 > _size) {
		reserve((_size == 0) ? 1 : _size * 2);
	}

	for (size_t i = _used; i > index; --i) {
		memcpy(_array + i, _array + i - 1, sizeof(T));
	}

	moveConstruct(_array + index, Move(args)...);
	++_used;
}

template <class T, class Allocator>
template <class... Args>
void Array<T, Allocator>::emplacePush(Args&&... args)
{
	if (_used == _size) {
		if (_size == 0) {
			reserve(1);
		} else {
			reserve(_size * 2);
		}
	}

	construct(_array + _used, args...);
	++_used;
}

template <class T, class Allocator>
template <class... Args>
void Array<T, Allocator>::emplaceMovePush(Args&&... args)
{
	if (_used == _size) {
		if (_size == 0) {
			reserve(1);
		} else {
			reserve(_size * 2);
		}
	}

	moveConstruct(_array + _used, Move(args)...);
	++_used;
}

template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::moveInsert(T&& data, const ARRAY_ITERATOR it)
{
	assert(it >= _array && it <= _array + _used);
	size_t index = static_cast<size_t>(it - _array);
	moveInsert(Move(data), index);
	return Iterator(_array + index);
}

/*!
	\brief Inserts \a data to the position \a index using the move constructor.

	\param data The data we are moving into the array.
	\param index The position we are inserting \a data into.
*/
template <class T, class Allocator>
void Array<T, Allocator>::moveInsert(T&& data, size_t index)
{
	assert(index <= _size);

	if (_used + 1 > _size) {
		reserve((_size == 0) ? 1 : _size * 2);
	}

	for (size_t i = _used; i > index; --i) {
		memcpy(_array + i, _array + i - 1, sizeof(T));
	}

	moveConstruct(_array + index, Move(data));
	++_used;
}

template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::insert(const T& data, const ARRAY_ITERATOR it)
{
	assert(it >= _array && it <= _array + _used);
	size_t index = static_cast<size_t>(it - _array);
	insert(data, index);
	return Iterator(_array + index);
}

/*!
	\brief Inserts \a data to the position \a index using the copy constructor.

	\param data The data we are moving into the array.
	\param index The position we are inserting \a data into.
*/
template <class T, class Allocator>
void Array<T, Allocator>::insert(const T& data, size_t index)
{
	assert(index <= _size);

	if (_used == _size) {
		reserve((_size == 0) ? 1 : _size * 2);
	}

	for (size_t i = _used; i > index; --i) {
		memcpy(_array + i, _array + i - 1, sizeof(T));
	}

	construct(_array + index, data);
	++_used;
}

template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::erase(const ARRAY_ITERATOR it)
{
	assert(it >= _array && it < _array + _used);

	size_t index = static_cast<size_t>(it - _array);
	erase(index);

	return Iterator(_array + index);
}

/*!
	Removes the element at position \a index. Shifts all elements above the removed position down one.

	\brief Removes the element at position \a index.

	\param index The position to remove.
*/
template <class T, class Allocator>
void Array<T, Allocator>::erase(size_t index)
{
	assert(index < _used && _used > 0);

	deconstruct(_array + index);

	for (size_t i = index; i < _used - 1; ++i) {
		memcpy(_array + i, _array + i + 1, sizeof(T));
	}

	--_used;
}

template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::fastErase(const ARRAY_ITERATOR it)
{
	assert(it >= _array && it < _array + _used);

	size_t index = static_cast<size_t>(it - _array);
	fastErase(index);

	return Iterator(_array + index);
}

/*!
	Removes the element at position \a index.
	It avoids shifting by copying the last element into the erased position.

	\brief Removes the element at position \a index without shifting.

	\param index The position to remove.

	\note Avoid using this function if your array must be sorted!
*/
template <class T, class Allocator>
void Array<T, Allocator>::fastErase(size_t index)
{
	assert(index < _used && _used > 0);

	deconstruct(_array + index);
	memcpy(_array + index, _array + _used - 1, sizeof(T));

	--_used;
}

template <class T, class Allocator>
void Array<T, Allocator>::resize(size_t new_size)
{
	if (new_size == _size) {
		return;
	}

	T* old_data = _array;

	_array = reinterpret_cast<T*>(_allocator.alloc(sizeof(T) * new_size));

	for (size_t i = _used; i < new_size; ++i) {
		construct(_array + i);
	}

	if (old_data) {
		if (new_size < _used) {
			for (size_t i = new_size; i < _used; ++i) {
				deconstruct(old_data + i);
			}
		}

		memcpy(_array, old_data, sizeof(T) * _used);
		_allocator.free(old_data);
	}

	_size = new_size;
	_used = new_size;
}

template <class T, class Allocator>
void Array<T, Allocator>::reserve(size_t reserve_size)
{
	if (reserve_size <= _size) {
		return;
	}

	T* old_data = _array;

	_array = reinterpret_cast<T*>(_allocator.alloc(sizeof(T) * reserve_size));
	_size = reserve_size;

	if (old_data) {
		memcpy(_array, old_data, sizeof(T) * _used);
		_allocator.free(old_data);
	}
}

/*!
	\brief Resizes array to the exact number of active elements. Makes capacity equal to size.
	\note This is the same as calling resize(size()).
*/
template <class T, class Allocator>
void Array<T, Allocator>::trim(void)
{
	if (_used != _size) {
		resize(_used);
	}
}

#ifndef DOXY_SKIP
template <class T, class Allocator>
template <class T2, class Pred>
ARRAY_ITERATOR Array<T, Allocator>::linearSearch(const ARRAY_ITERATOR range_begin, const ARRAY_ITERATOR range_end, const T2& data, const Pred& pred) const
{
	assert(range_begin >= _array && range_begin <= _array + _used);
	assert(range_end >= _array && range_end <= _array + _used);
	assert(range_begin <= range_end);

	size_t index1 = static_cast<size_t>(range_begin - _array);
	size_t index2 = static_cast<size_t>(range_end - _array);

	size_t result = linearSearch(index1, index2, data, pred);
	return (result != SIZE_T_FAIL) ? Iterator(_array + result) : end();
}
#endif

/*!
	\brief Linearly searches each element in the range until the predicate is satisfied.

	\param range_begin The start of the range to search.
	\param range_end The end of the range to search.
	\param data The data we are searching for.
	\param pred The predicate we must satisfy in order to complete the search.

	\tparam T2 The type of the data we are searching for.
	\tparam Pred The type of the predicate we are using to satisfy our search. Must overload operator()!

	\return Returns the index of the element that satisfied the predicate, otherwise returns SIZE_T_FAIL.

	\note \a Pred defaults to Less if no predicate is given. Search range is [range_begin, range_end), non-inclusive.
*/
template <class T, class Allocator>
template <class T2, class Pred>
size_t Array<T, Allocator>::linearSearch(size_t range_begin, size_t range_end, const T2& data, const Pred& pred) const
{
	assert(range_begin <= range_end && range_end <= _used);

	for (size_t i = range_begin; i < range_end; ++i) {
		if (pred(_array[i], data)) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

#ifndef DOXY_SKIP
template <class T, class Allocator>
template <class T2, class Pred>
ARRAY_ITERATOR Array<T, Allocator>::linearSearch(const T2& data, const Pred& pred) const
{
	return linearSearch(begin(), end(), data, pred);
}
#endif

#ifndef DOXY_SKIP
template <class T, class Allocator>
template <class T2, class Pred>
ARRAY_ITERATOR Array<T, Allocator>::binarySearch(
	const ARRAY_ITERATOR range_begin,
	const ARRAY_ITERATOR range_end,
	const T2& data,
	const Pred& pred) const
{
	assert(range_begin >= _array && range_begin <= _array + _used);
	assert(range_end >= _array && range_end <= _array + _used);
	assert(range_begin <= range_end);

	size_t index1 = static_cast<size_t>(range_begin - _array);
	size_t index2 = static_cast<size_t>(range_end - _array);

	size_t result = binarySearch(index1, index2, data, pred);
	return (result != SIZE_T_FAIL) ? Iterator(_array + result) : end();
}
#endif

/*!
	\brief Does a binary search over the range.

	\param range_begin The start of the range to search.
	\param range_end The end of the range to search.
	\param data The data we are searching for.
	\param pred The predicate used by the binary search to determine which direction it should move in.

	\tparam T2 The type of the data we are searching for.
	\tparam Pred The type of the predicate we are using to satisfy our search. Must overload operator() if a functor!

	\return An index to the found element. If the element is not found, the index is where \a data should be inserted.

	\note The array must be sorted with the predicate used to search. Search range is [range_begin, range_end), non-inclusive.
*/
template <class T, class Allocator>
template <class T2, class Pred>
size_t Array<T, Allocator>::binarySearch(
	size_t range_begin,
	size_t range_end,
	const T2& data,
	const Pred& pred) const
{
	assert(range_end <= _used && range_begin <= range_end);

	size_t mid = 0;

	while (range_begin != range_end) {
		mid = range_begin + (range_end - range_begin) / 2;

		if (pred(_array[mid], data)) {
			range_begin = mid + 1;
		} else {
			range_end = mid;
		}
	}

	return range_begin;
}

#ifndef DOXY_SKIP
template <class T, class Allocator>
template <class T2, class Pred>
ARRAY_ITERATOR Array<T, Allocator>::binarySearch(const T2& data, const Pred& pred) const
{
	return binarySearch(begin(), end(), data, pred);
}
#endif

template <class T, class Allocator>
size_t Array<T, Allocator>::size(void) const
{
	return _used;
}

template <class T, class Allocator>
size_t Array<T, Allocator>::capacity(void) const
{
	return _size;
}

template <class T, class Allocator>
void Array<T, Allocator>::setAllocator(const Allocator& allocator)
{
	_allocator = allocator;
}
