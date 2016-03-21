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

#define ARRAY_ITERATOR typename Array<T, Allocator>::Iterator

template <class T, class Allocator>
Array<T, Allocator>::Array(const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(0), _size(0)
{
}

template <class T, class Allocator>
Array<T, Allocator>::Array(size_t start_capacity, const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(0), _size(start_capacity)
{
	_array = GAFF_ALLOC_CAST(T*, sizeof(T) * start_capacity, _allocator);
}

template <class T, class Allocator>
Array<T, Allocator>::Array(size_t start_size, const T& init_val, const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(start_size), _size(start_size)
{
	_array = GAFF_ALLOC_CAST(T*, sizeof(T) * start_size, _allocator);

	for (size_t i = 0; i < start_size; ++i) {
		construct(_array + i, init_val);
	}
}

template <class T, class Allocator>
Array<T, Allocator>::Array(const T* data, size_t size, const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(size), _size(size)
{
	_array = GAFF_ALLOC_CAST(T*, sizeof(T) * size, _allocator);

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
		_array = GAFF_ALLOC_CAST(T*, sizeof(T) * rhs._size, _allocator);
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
	GAFF_ASSERT(index < _used);
	return _array[index];
}

template <class T, class Allocator>
T& Array<T, Allocator>::operator[](size_t index)
{
	GAFF_ASSERT(index < _used);
	return _array[index];
}

template <class T, class Allocator>
void Array<T, Allocator>::clear(void)
{
	if (_array) {
		for (size_t i = 0; i < _used; ++i) {
			deconstruct(_array + i);
		}

		GAFF_FREE(static_cast<void*>(_array), _allocator);
		_used = _size = 0;
		_array = nullptr;
	}
}

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
	GAFF_ASSERT(_used);
	return _array[0];
}

template <class T, class Allocator>
T& Array<T, Allocator>::first(void)
{
	GAFF_ASSERT(_used);
	return _array[0];
}

template <class T, class Allocator>
const T& Array<T, Allocator>::last(void) const
{
	GAFF_ASSERT(_used);
	return _array[_used - 1];
}

template <class T, class Allocator>
T& Array<T, Allocator>::last(void)
{
	GAFF_ASSERT(_used);
	return _array[_used - 1];
}

template <class T, class Allocator>
const T* Array<T, Allocator>::getArray(void) const
{
	return _array;
}

template <class T, class Allocator>
T* Array<T, Allocator>::getArray(void)
{
	return _array;
}

template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::begin(void) const
{
	return Iterator(_array);
}

template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::end(void) const
{
	return Iterator(_array + _used);
}

template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::rbegin(void) const
{
	return Iterator(_array + _used - 1);
}

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

template <class T, class Allocator>
void Array<T, Allocator>::append(const T* array, size_t count)
{
	reserve(_used + count);

	for (size_t i = 0; i < count; ++i) {
		construct(_array + _used + i, array[i]);
	}

	_used += count;
}

template <class T, class Allocator>
void Array<T, Allocator>::append(const Array<T, Allocator>& array)
{
	append(array.getArray(), array.size());
}

template <class T, class Allocator>
void Array<T, Allocator>::append(Array<T, Allocator>&& array)
{
	reserve(_used + array.size());

	for (size_t i = 0; i < array.size(); ++i) {
		construct(_array + _used + i, std::move(array[i]));
	}

	_used += array.size();
	array.clear();
}

template <class T, class Allocator>
void Array<T, Allocator>::push(T&& data)
{
	if (_used == _size) {
		if (_size == 0) {
			reserve(1);
		} else {
			reserve(_size * 2);
		}
	}

	construct(_array + _used, std::move(data));
	++_used;
}

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

template <class T, class Allocator>
void Array<T, Allocator>::pop(void)
{
	GAFF_ASSERT(_used);
	deconstruct(_array + _used - 1);
	--_used;
}

template <class T, class Allocator>
template <class... Args>
ARRAY_ITERATOR Array<T, Allocator>::emplace(const ARRAY_ITERATOR it, Args&&... args)
{
	GAFF_ASSERT(it >= _array && it <= _array + _used);
	size_t index = static_cast<size_t>(it - _array);
	emplace(index, std::forward<Args>(args)...);
	return Iterator(_array + index);
}

template <class T, class Allocator>
template <class... Args>
void Array<T, Allocator>::emplace(size_t index, Args&&... args)
{
	GAFF_ASSERT(index <= _size);

	if (_used + 1 > _size) {
		reserve((_size == 0) ? 1 : _size * 2);
	}

	for (size_t i = _used; i > index; --i) {
		memcpy(_array + i, _array + i - 1, sizeof(T));
	}

	construct(_array + index, std::forward<Args>(args)...);
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

	construct(_array + _used, std::forward<Args>(args)...);
	++_used;
}

template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::insert(const ARRAY_ITERATOR it, const T& data)
{
	GAFF_ASSERT(it >= _array && it <= _array + _used);
	size_t index = static_cast<size_t>(it - _array);
	insert(index, data);
	return Iterator(_array + index);
}

template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::insert(const ARRAY_ITERATOR it, T&& data)
{
	GAFF_ASSERT(it >= _array && it <= _array + _used);
	size_t index = static_cast<size_t>(it - _array);
	insert(index, std::move(data));
	return Iterator(_array + index);
}

template <class T, class Allocator>
void Array<T, Allocator>::insert(size_t index, const T& data)
{
	GAFF_ASSERT(index <= _size);

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
void Array<T, Allocator>::insert(size_t index, T&& data)
{
	GAFF_ASSERT(index <= _size);

	if (_used + 1 > _size) {
		reserve((_size == 0) ? 1 : _size * 2);
	}

	for (size_t i = _used; i > index; --i) {
		memcpy(_array + i, _array + i - 1, sizeof(T));
	}

	construct(_array + index, std::move(data));
	++_used;
}

template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::erase(const ARRAY_ITERATOR it)
{
	GAFF_ASSERT(it >= _array && it < _array + _used);

	size_t index = static_cast<size_t>(it - _array);
	erase(index);

	return Iterator(_array + index);
}

template <class T, class Allocator>
void Array<T, Allocator>::erase(size_t index)
{
	GAFF_ASSERT(index < _used && _used > 0);

	deconstruct(_array + index);

	for (size_t i = index; i < _used - 1; ++i) {
		memcpy(_array + i, _array + i + 1, sizeof(T));
	}

	--_used;
}

template <class T, class Allocator>
ARRAY_ITERATOR Array<T, Allocator>::fastErase(const ARRAY_ITERATOR it)
{
	GAFF_ASSERT(it >= _array && it < _array + _used);

	size_t index = static_cast<size_t>(it - _array);
	fastErase(index);

	return Iterator(_array + index);
}

template <class T, class Allocator>
void Array<T, Allocator>::fastErase(size_t index)
{
	GAFF_ASSERT(index < _used && _used > 0);

	deconstruct(_array + index);
	memcpy(_array + index, _array + _used - 1, sizeof(T));

	--_used;
}

template <class T, class Allocator>
void Array<T, Allocator>::resizeFast(size_t new_size, const T& init_val)
{
	if (new_size == _size) {
		return;

	} else if (new_size < _size) {
		for (size_t i = new_size; i < _used; ++i) {
			deconstruct(_array + i);
		}

		_used = new_size;
		return;
	}

	resizeHelper(new_size, init_val);
}

template <class T, class Allocator>
void Array<T, Allocator>::resizeFast(size_t new_size)
{
	if (new_size == _size) {
		return;

	} else if (new_size < _size) {
		for (size_t i = new_size; i < _used; ++i) {
			deconstruct(_array + i);
		}

		_used = new_size;
		return;
	}

	resizeHelper(new_size);
}

template <class T, class Allocator>
void Array<T, Allocator>::resize(size_t new_size, const T& init_val)
{
	if (new_size == _size) {
		return;
	}

	resizeHelper(new_size, init_val);
}

template <class T, class Allocator>
void Array<T, Allocator>::resize(size_t new_size)
{
	if (new_size == _size) {
		return;
	}

	resizeHelper(new_size);
}

template <class T, class Allocator>
void Array<T, Allocator>::reserve(size_t reserve_size)
{
	if (reserve_size <= _size) {
		return;
	}

	T* old_data = _array;

	_array = GAFF_ALLOC_CAST(T*, sizeof(T) * reserve_size, _allocator);
	_size = reserve_size;

	if (old_data) {
		memcpy(
			reinterpret_cast<void*>(_array),
			reinterpret_cast<void*>(old_data),
			sizeof(T) * _used
		);

		GAFF_FREE(reinterpret_cast<void*>(old_data), _allocator);
	}
}

template <class T, class Allocator>
void Array<T, Allocator>::trim(void)
{
	if (_used != _size) {
		resize(_used);
	}
}

template <class T, class Allocator>
template <class T2, class Pred>
const ARRAY_ITERATOR Array<T, Allocator>::linearSearch(const ARRAY_ITERATOR range_begin, const ARRAY_ITERATOR range_end, const T2& value, const Pred& pred) const
{
	GAFF_ASSERT(range_begin >= _array && range_begin <= _array + _used);
	GAFF_ASSERT(range_end >= _array && range_end <= _array + _used);

	const Iterator it = LinearSearch(range_begin, range_end, value, pred);
	return (it) ? it : end();
}

template <class T, class Allocator>
template <class T2, class Pred>
ARRAY_ITERATOR Array<T, Allocator>::linearSearch(ARRAY_ITERATOR range_begin, ARRAY_ITERATOR range_end, const T2& value, const Pred& pred)
{
	GAFF_ASSERT(range_begin >= _array && range_begin <= _array + _used);
	GAFF_ASSERT(range_end >= _array && range_end <= _array + _used);

	const Iterator it = LinearSearch(range_begin, range_end, value, pred);
	return (it) ? it : end();
}

template <class T, class Allocator>
template <class T2, class Pred>
size_t Array<T, Allocator>::linearSearch(size_t range_begin, size_t range_end, const T2& value, const Pred& pred) const
{
	GAFF_ASSERT(range_end <= _used);
	return LinearSearch(_array, range_begin, range_end, value, pred);
}

template <class T, class Allocator>
template <class T2, class Pred>
const ARRAY_ITERATOR Array<T, Allocator>::linearSearch(const T2& value, const Pred& pred) const
{
	return linearSearch(begin(), end(), value, pred);
}

template <class T, class Allocator>
template <class T2, class Pred>
ARRAY_ITERATOR Array<T, Allocator>::linearSearch(const T2& value, const Pred& pred)
{
	return linearSearch(begin(), end(), value, pred);
}

template <class T, class Allocator>
template <class T2, class Pred>
const ARRAY_ITERATOR Array<T, Allocator>::binarySearch(
	const ARRAY_ITERATOR range_begin,
	const ARRAY_ITERATOR range_end,
	const T2& value,
	const Pred& pred) const
{
	GAFF_ASSERT(range_begin >= _array && range_begin <= _array + _used);
	GAFF_ASSERT(range_end >= _array && range_end <= _array + _used);

	return BinarySearch(range_begin, range_end, value, pred);
}

template <class T, class Allocator>
template <class T2, class Pred>
ARRAY_ITERATOR Array<T, Allocator>::binarySearch(
	ARRAY_ITERATOR range_begin,
	ARRAY_ITERATOR range_end,
	const T2& value,
	const Pred& pred)
{
	GAFF_ASSERT(range_begin >= _array && range_begin <= _array + _used);
	GAFF_ASSERT(range_end >= _array && range_end <= _array + _used);

	return BinarySearch(range_begin, range_end, value, pred);
}

template <class T, class Allocator>
template <class T2, class Pred>
size_t Array<T, Allocator>::binarySearch(
	size_t range_begin,
	size_t range_end,
	const T2& value,
	const Pred& pred) const
{
	GAFF_ASSERT(range_end <= _used);
	return BinarySearch(_array, range_begin, range_end, value, pred);
}

template <class T, class Allocator>
template <class T2, class Pred>
const ARRAY_ITERATOR Array<T, Allocator>::binarySearch(const T2& value, const Pred& pred) const
{
	return BinarySearch(begin(), end(), value, pred);
}

template <class T, class Allocator>
template <class T2, class Pred>
ARRAY_ITERATOR Array<T, Allocator>::binarySearch(const T2& value, const Pred& pred)
{
	return BinarySearch(begin(), end(), value, pred);
}

template <class T, class Allocator>
void Array<T, Allocator>::reverse(void)
{
	if (_used > 1) {
		Reverse(_array, _used);
	}
}

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

template <class T, class Allocator>
void Array<T, Allocator>::resizeHelper(size_t new_size, const T& init_val)
{
	T* old_data = _array;

	_array = GAFF_ALLOC_CAST(T*, sizeof(T) * new_size, _allocator);

	for (size_t i = _used; i < new_size; ++i) {
		construct(_array + i, init_val);
	}

	if (old_data) {
		if (new_size < _used) {
			for (size_t i = new_size; i < _used; ++i) {
				deconstruct(old_data + i);
			}
		}

		memcpy(
			reinterpret_cast<void*>(_array),
			reinterpret_cast<void*>(old_data),
			sizeof(T) * _used
		);

		GAFF_FREE(reinterpret_cast<void*>(old_data), _allocator);
	}

	_size = new_size;
	_used = new_size;
}

template <class T, class Allocator>
void Array<T, Allocator>::resizeHelper(size_t new_size)
{
	T* old_data = _array;

	_array = GAFF_ALLOC_CAST(T*, sizeof(T) * new_size, _allocator);

	for (size_t i = _used; i < new_size; ++i) {
		construct(_array + i);
	}

	if (old_data) {
		if (new_size < _used) {
			for (size_t i = new_size; i < _used; ++i) {
				deconstruct(old_data + i);
			}
		}

		memcpy(
			reinterpret_cast<void*>(_array),
			reinterpret_cast<void*>(old_data),
			sizeof(T) * _used
		);

		GAFF_FREE(reinterpret_cast<void*>(old_data), _allocator);
	}

	_size = new_size;
	_used = new_size;
}

// Helper functions
template <class T, class T2, class Pred>
const T* LinearSearch(const T* range_begin, const T* range_end, const T2& value, const Pred& pred)
{
	GAFF_ASSERT(range_begin <= range_end);

	for (; range_begin < range_end; ++range_begin) {
		if (pred(*range_begin, value)) {
			return range_begin;
		}
	}

	return nullptr;
}

template <class T, class T2, class Pred>
T* LinearSearch(T* range_begin, T* range_end, const T2& value, const Pred& pred)
{
	GAFF_ASSERT(range_begin <= range_end);

	for (; range_begin < range_end; ++range_begin) {
		if (pred(*range_begin, value)) {
			return range_begin;
		}
	}

	return nullptr;
}

template <class T, class T2, class Pred>
size_t LinearSearch(const T* data, size_t range_begin, size_t range_end, const T2& value, const Pred& pred)
{
	GAFF_ASSERT(range_begin <= range_end);

	for (size_t i = range_begin; i < range_end; ++i) {
		if (pred(data[i], value)) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

template <class T, class T2, class Pred>
const T* BinarySearch(const T* range_begin, const T* range_end, const T2& value, const Pred& pred)
{
	GAFF_ASSERT(range_begin <= range_end);

	while (range_begin != range_end) {
		T* mid = range_begin + static_cast<size_t>(range_end - range_begin) / 2;

		if (pred(*mid, value)) {
			range_begin = mid + 1;
		} else {
			range_end = mid;
		}
	}

	return range_begin;
}

template <class T, class T2, class Pred>
T* BinarySearch(T* range_begin, T* range_end, const T2& value, const Pred& pred)
{
	GAFF_ASSERT(range_begin <= range_end);

	while (range_begin != range_end) {
		T* mid = range_begin + static_cast<size_t>(range_end - range_begin) / 2;

		if (pred(*mid, value)) {
			range_begin = mid + 1;
		} else {
			range_end = mid;
		}
	}

	return range_begin;
}

template <class T, class T2, class Pred>
size_t BinarySearch(const T* data, size_t range_begin, size_t range_end, const T2& value, const Pred& pred)
{
	GAFF_ASSERT(range_begin <= range_end);

	while (range_begin != range_end) {
		size_t mid = range_begin + (range_end - range_begin) / 2;

		if (pred(data[mid], value)) {
			range_begin = mid + 1;
		} else {
			range_end = mid;
		}
	}

	return range_begin;
}

template <class T>
void Reverse(T* data, size_t size)
{
	size_t beg = 0;
	size_t end = size - 1;

	while (beg < end) {
		char temp[sizeof(T)];

		// Use memcpy to avoid calling constructor/destructor
		memcpy(temp, _array + beg, sizeof(T));
		memcpy(_array + beg, _array + end, sizeof(T));
		memcpy(_array + end, temp, sizeof(T));

		++beg;
		--end;
	}
}
