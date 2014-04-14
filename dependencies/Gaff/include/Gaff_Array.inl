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

#define ARRAY_ITERATOR Array<T, Allocator>::Iterator

////////////////////
//    Iterator    //
////////////////////
template <class T, class Allocator>
Array<T, Allocator>::Iterator::Iterator(const Iterator& it):
	_element(it._element)
{
}

template <class T, class Allocator>
Array<T, Allocator>::Iterator::Iterator(void):
	_element(nullptr)
{
}

template <class T, class Allocator>
const typename ARRAY_ITERATOR& Array<T, Allocator>::Iterator::operator++(void) const
{
	++_element;
	return *this;
}

template <class T, class Allocator>
const typename ARRAY_ITERATOR& Array<T, Allocator>::Iterator::operator--(void) const
{
	--_element;
	return *this;
}

template <class T, class Allocator>
typename ARRAY_ITERATOR Array<T, Allocator>::Iterator::operator++(int) const
{
	Iterator it = *this;
	++_element;
	return it;
}

template <class T, class Allocator>
typename ARRAY_ITERATOR Array<T, Allocator>::Iterator::operator--(int) const
{
	Iterator it = *this;
	--_element;
	return it;
}

template <class T, class Allocator>
const typename ARRAY_ITERATOR& Array<T, Allocator>::Iterator::operator+=(int rhs) const
{
	_element += rhs;
	return *this;
}

template <class T, class Allocator>
const typename ARRAY_ITERATOR& Array<T, Allocator>::Iterator::operator-=(int rhs) const
{
	_element -= rhs;
	return *this;
}

template <class T, class Allocator>
typename ARRAY_ITERATOR Array<T, Allocator>::Iterator::operator+(int rhs) const
{
	return Iterator(_element + rhs);
}

template <class T, class Allocator>
typename ARRAY_ITERATOR Array<T, Allocator>::Iterator::operator-(int rhs) const
{
	return Iterator(_element - rhs);
}

template <class T, class Allocator>
typename OffsetType Array<T, Allocator>::Iterator::operator+(const typename ARRAY_ITERATOR& rhs) const
{
	return _element + rhs._element;
}

template <class T, class Allocator>
typename OffsetType Array<T, Allocator>::Iterator::operator-(const typename ARRAY_ITERATOR& rhs) const
{
	return _element - rhs._element;
}

template <class T, class Allocator>
bool Array<T, Allocator>::Iterator::operator==(const typename ARRAY_ITERATOR& rhs) const
{
	return _element == rhs._element;
}

template <class T, class Allocator>
bool Array<T, Allocator>::Iterator::operator!=(const typename ARRAY_ITERATOR& rhs) const
{
	return _element != rhs._element;
}

template <class T, class Allocator>
const typename ARRAY_ITERATOR& Array<T, Allocator>::Iterator::operator=(const typename ARRAY_ITERATOR& rhs) const
{
	_element = rhs._element;
	return *this;
}

template <class T, class Allocator>
const T& Array<T, Allocator>::Iterator::operator*(void) const
{
	return *_element;
}

template <class T, class Allocator>
T& Array<T, Allocator>::Iterator::operator*(void)
{
	return *_element;
}

template <class T, class Allocator>
const T* Array<T, Allocator>::Iterator::operator->(void) const
{
	return _element;
}

template <class T, class Allocator>
T* Array<T, Allocator>::Iterator::operator->(void)
{
	return _element;
}

template <class T, class Allocator>
Array<T, Allocator>::Iterator::Iterator(T* element):
	_element(element)
{
}

/////////////////
//    Array    //
/////////////////
template <class T, class Allocator>
Array<T, Allocator>::Array(const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(0), _size(0)
{
}

template <class T, class Allocator>
Array<T, Allocator>::Array(unsigned int start_alloc, const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(0), _size(start_alloc)
{
	_array = (T*)_allocator.alloc(sizeof(T) * start_alloc);
}

template <class T, class Allocator>
Array<T, Allocator>::Array(unsigned int start_alloc, const T& init_val, const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(start_alloc), _size(start_alloc)
{
	_array = (T*)_allocator.alloc(sizeof(T) * start_alloc);

	for (unsigned int i = 0; i < start_alloc; ++i) {
		construct(_array + i, init_val);
	}
}

template <class T, class Allocator>
Array<T, Allocator>::Array(unsigned int size, const T* data, const Allocator& allocator):
	_allocator(allocator), _array(nullptr), _used(size), _size(size)
{
	_array = (T*)_allocator.alloc(sizeof(T) * size);

	for (unsigned int i = 0; i < size; ++i) {
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

	for (unsigned int i = 0; i < rhs._used; ++i) {
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

	for (unsigned int i = 0; i < _used; ++i) {
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
const T& Array<T, Allocator>::operator[](unsigned int index) const
{
	assert(index < _used);
	return _array[index];
}

template <class T, class Allocator>
T& Array<T, Allocator>::operator[](unsigned int index)
{
	assert(index < _used);
	return _array[index];
}

template <class T, class Allocator>
void Array<T, Allocator>::clear(void)
{
	if (_array) {
		for (unsigned int i = 0; i < _used; ++i) {
			deconstruct(_array + i);
		}

		_allocator.free(_array);
		_used = _size = 0;
		_array = nullptr;
	}
}

template <class T, class Allocator>
void Array<T, Allocator>::clearNoFree(void)
{
	if (_array) {
		for (unsigned int i = 0; i < _used; ++i) {
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
typename ARRAY_ITERATOR Array<T, Allocator>::begin(void) const
{
	return Iterator(_array);
}

template <class T, class Allocator>
typename ARRAY_ITERATOR Array<T, Allocator>::end(void) const
{
	return Iterator(_array + _used);
}

template <class T, class Allocator>
typename ARRAY_ITERATOR Array<T, Allocator>::rbegin(void) const
{
	return Iterator(_array + _used - 1);
}

template <class T, class Allocator>
typename ARRAY_ITERATOR Array<T, Allocator>::rend(void) const
{
	return Iterator(_array - 1);
}

template <class T, class Allocator>
bool Array<T, Allocator>::empty(void) const
{
	return _used == 0;
}

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
	assert(_used);
	deconstruct(_array + _used - 1);
	--_used;
}

template <class T, class Allocator>
typename ARRAY_ITERATOR Array<T, Allocator>::moveInsert(T&& data, const typename ARRAY_ITERATOR& it)
{
	assert(it._element >= _array && it._element <= _array + _used);
	unsigned int index = (unsigned int)(it._element - _array);
	insert(Move(data), index);
	return Iterator(_array + index);
}

template <class T, class Allocator>
void Array<T, Allocator>::moveInsert(T&& data, unsigned int index)
{
	assert(index <= _size);

	if (_used + 1 > _size) {
		reserve((_size == 0) ? 1 : _size * 2);
	}

	for (unsigned int i = _used; i > index; --i) {
		memcpy(_array + i, _array + i - 1, sizeof(T));
	}

	moveConstruct(_array + index, data);
	++_used;
}

template <class T, class Allocator>
typename ARRAY_ITERATOR Array<T, Allocator>::insert(const T& data, const typename ARRAY_ITERATOR& it)
{
	assert(it._element >= _array && it._element <= _array + _used);
	unsigned int index = (unsigned int)(it._element - _array);
	insert(data, index);
	return Iterator(_array + index);
}

template <class T, class Allocator>
void Array<T, Allocator>::insert(const T& data, unsigned int index)
{
	assert(index <= _size);

	if (_used + 1 > _size) {
		reserve((_size == 0) ? 1 : _size * 2);
	}

	for (unsigned int i = _used; i > index; --i) {
		memcpy(_array + i, _array + i - 1, sizeof(T));
	}

	construct(_array + index, data);
	++_used;
}

template <class T, class Allocator>
typename ARRAY_ITERATOR Array<T, Allocator>::erase(const typename ARRAY_ITERATOR& it)
{
	assert(it._element >= _array && it._element < _array + _used);
	// Gets rid of compiler warning. Making assumption that in 64-bit, array sizes will not exceed the maximum value of an unsigned int.
	unsigned int index = (unsigned int)(it._element - _array);
	erase(index);
	return Iterator(_array + index);
}

template <class T, class Allocator>
void Array<T, Allocator>::erase(unsigned int index)
{
	assert(index < _used && _used > 0);

	deconstruct(_array + index);

	for (unsigned int i = index; i < _used - 1; ++i) {
		memcpy(_array + i, _array + i + 1, sizeof(T));
	}

	--_used;
}

template <class T, class Allocator>
void Array<T, Allocator>::resize(unsigned int new_size)
{
	if (new_size == _size) {
		return;
	}

	T* old_data = _array;

	_array = (T*)_allocator.alloc(sizeof(T) * new_size);

	for (unsigned int i = _used; i < new_size; ++i) {
		construct(_array + i);
	}

	if (old_data) {
		if (new_size < _size) {
			for (unsigned int i = new_size; i < _size; ++i) {
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
void Array<T, Allocator>::reserve(unsigned int reserve_size)
{
	if (reserve_size <= _size) {
		return;
	}

	T* old_data = _array;

	_array = (T*)_allocator.alloc(sizeof(T) * reserve_size);
	_size = reserve_size;

	if (old_data) {
		memcpy(_array, old_data, sizeof(T) * _used);
		_allocator.free(old_data);
	}
}

template <class T, class Allocator>
template <class T2, class Pred>
typename ARRAY_ITERATOR Array<T, Allocator>::linearSearch(const typename ARRAY_ITERATOR& range_begin, const typename ARRAY_ITERATOR& range_end, const T2& data, const Pred& pred) const
{
	assert(range_begin._element >= _array && range_begin._element <= _array + _used);
	assert(range_end._element >= _array && range_end._element <= _array + _used);
	assert(range_begin._element <= range_end._element);

	// Gets rid of compiler warning. Making assumption that in 64-bit, array sizes will not exceed the maximum value of an unsigned int.
	unsigned int index1 = (unsigned int)(range_begin._element - _array);
	unsigned int index2 = (unsigned int)(range_end._element - _array);

	int result = linearSearch(index1, index2, data, pred);
	Iterator ret = end();

	if (result > -1) {
		ret = Iterator(_array + result);
	}

	return ret;
}

template <class T, class Allocator>
template <class T2, class Pred>
int Array<T, Allocator>::linearSearch(unsigned int range_begin, unsigned int range_end, const T2& data, const Pred& pred) const
{
	assert(range_begin <= range_end && range_end <= _used);

	for (unsigned int i = range_begin; i < range_end; ++i) {
		if (pred(_array[i], data)) {
			return i;
		}
	}

	return -1;
}

template <class T, class Allocator>
template <class T2, class Pred>
typename ARRAY_ITERATOR Array<T, Allocator>::linearSearch(const T2& data, const Pred& pred) const
{
	return linearSearch(begin(), end(), data, pred);
}

template <class T, class Allocator>
template <class T2, class Pred>
typename ARRAY_ITERATOR Array<T, Allocator>::binarySearch(
	const typename ARRAY_ITERATOR& range_begin,
	const typename ARRAY_ITERATOR& range_end,
	const T2& data,
	const Pred& pred) const
{
	assert(range_begin._element >= _array && range_begin._element <= _array + _used);
	assert(range_end._element >= _array && range_end._element <= _array + _used);
	assert(range_begin._element <= range_end._element);

	unsigned int index1 = range_begin._element - _array;
	unsigned int index2 = range_end._element - _array;

	int result = binarySearch(index1, index2, data, pred);
	return Iterator(_array + result);
}

template <class T, class Allocator>
template <class T2, class Pred>
int Array<T, Allocator>::binarySearch(
	unsigned int range_begin,
	unsigned int range_end,
	const T2& data,
	const Pred& pred) const
{
	assert(range_end <= _used && range_begin <= range_end);

	unsigned int mid = 0;

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

template <class T, class Allocator>
template <class T2, class Pred>
typename ARRAY_ITERATOR Array<T, Allocator>::binarySearch(const T2& data, const Pred& pred) const
{
	return binarySearch(begin(), end(), data, pred);
}

template <class T, class Allocator>
unsigned int Array<T, Allocator>::size(void) const
{
	return _used;
};

template <class T, class Allocator>
unsigned int Array<T, Allocator>::capacity(void) const
{
	return _size;
}
