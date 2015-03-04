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

template <class T, class Allocator>
Queue<T, Allocator>::Queue(const Allocator& allocator):
	_allocator(allocator), _used(0), _size(0),
	_array(nullptr), _begin(nullptr), _end(nullptr)
{
}

template <class T, class Allocator>
Queue<T, Allocator>::Queue(size_t start_alloc, const Allocator& allocator) :
	_allocator(allocator), _used(0), _size(start_alloc)
{
	_array = (T*)_allocator.alloc(sizeof(T) * start_alloc);
	_begin = _end = _array + (_size / 2);
}

template <class T, class Allocator>
Queue<T, Allocator>::Queue(const T* data, size_t size, const Allocator& allocator) :
	_allocator(allocator), _used(0), _size(size)
{
	_array = (T*)_allocator.alloc(sizeof(T) * size);
	_begin = _end = _array + (size / 2);

	for (size_t i = 0; i < size; ++i) {
		push(data[i]);
	}
}

template <class T, class Allocator>
Queue<T, Allocator>::Queue(const Queue<T, Allocator>& rhs):
	_allocator(rhs._allocator)
{
	*this = rhs;
}

template <class T, class Allocator>
Queue<T, Allocator>::Queue(Queue<T, Allocator>&& rhs):
	_allocator(rhs._allocator), _used(rhs._used),
	_size(rhs._size), _array(rhs._array),
	_begin(rhs._begin), _end(rhs._end)
{
	rhs._array = rhs._begin = rhs._end = nullptr;
	rhs._used = rhs._size = 0;
}

template <class T, class Allocator>
Queue<T, Allocator>::~Queue(void)
{
	clear();
}

template <class T, class Allocator>
const Queue<T, Allocator>& Queue<T, Allocator>::operator=(const Queue<T, Allocator>& rhs)
{
	if (this == &rhs) {
		return *this;
	}

	clear();

	if (rhs._size == 0) {
		return *this;
	}

	T* begin = rhs._begin;

	_array = (T*)_allocator.alloc(sizeof(T) * rhs._size);
	_begin = _end = _array + (rhs._size / 2);
	_size = rhs._size;
	_used = 0;

	for (size_t i = 0; i < rhs._used; ++i) {
		push(*begin);
		rhs.increment(&begin);
	}

	return *this;
}

template <class T, class Allocator>
const Queue<T, Allocator>& Queue<T, Allocator>::operator=(Queue<T, Allocator>&& rhs)
{
	clear();

	_used = rhs._used;
	_size = rhs._size;
	_array = rhs._array;

	rhs._array = rhs._begin = rhs._end = nullptr;
	rhs._used = rhs._size = 0;

	return *this;
}

template <class T, class Allocator>
bool Queue<T, Allocator>::operator==(const Queue<T, Allocator>& rhs) const
{
	if (_used != rhs._used) {
		return false;
	}

	T* our_begin = _begin;
	T* rhs_begin = rhs._begin;

	for (size_t i = 0; i < _used; ++i) {
		if (*our_begin != *rhs_begin) {
			return false;
		}

		increment(&our_begin);
		rhs.increment(&rhs_begin);
	}

	return true;
}

template <class T, class Allocator>
bool Queue<T, Allocator>::operator!=(const Queue<T, Allocator>& rhs) const
{
	return !(*this == rhs);
}

template <class T, class Allocator>
void Queue<T, Allocator>::clear(void)
{
	if (_array) {
		for (size_t i = 0; i < _used; ++i) {
			deconstruct(_begin);
			increment(&_begin);
		}

		_allocator.free(_array);
		_used = _size = 0;
		_array = _begin = _end = nullptr;
	}
}

/*!
	\brief Similar to Array::resize(), but does not initialize extra elements to a default value.

	\param new_size The new size of the queue.

	\note
		If \a new_size is less than current size, it will erase data.
		If \a new_size is greater than the current size, size will not change, but capacity will.
*/
template <class T, class Allocator>
void Queue<T, Allocator>::reallocate(size_t new_size)
{
	if (new_size == 0) {
		new_size = 1;
	}

	size_t old_used = _used;
	size_t old_size = _size;
	T* old_data = _array;
	T* begin = _begin;

	_array = reinterpret_cast<T*>(_allocator.alloc(sizeof(T) * new_size));
	_begin = _end = _array + (new_size / 2);
	_size = new_size;
	_used = 0;

	for (size_t i = 0; i < old_used; ++i) {
		movePush(Move(*begin));
		increment(&begin, old_data, old_size);
	}

	if (old_data) {
		_allocator.free(old_data);
	}
}

template <class T, class Allocator>
void Queue<T, Allocator>::movePush(T&& data)
{
	if (_used == _size) {
		reallocate(_size * 2);
	}

	moveConstruct(_end, Move(data));
	increment(&_end);
	++_used;
}

template <class T, class Allocator>
void Queue<T, Allocator>::push(const T& data)
{
	if (_used == _size) {
		reallocate(_size * 2);
	}

	construct(_end, data);
	increment(&_end);
	++_used;
}

template <class T, class Allocator>
void Queue<T, Allocator>::pop(void)
{
	assert(_used > 0);
	deconstruct(_begin);
	increment(&_begin);
	--_used;
}

template <class T, class Allocator>
template <class... Args>
void Queue<T, Allocator>::emplacePush(Args&&... args)
{
	if (_used == _size) {
		reallocate(_size * 2);
	}

	construct(_end, args...);
	increment(&_end);
	++_used;
}

template <class T, class Allocator>
template <class... Args>
void Queue<T, Allocator>::emplaceMovePush(Args&&... args)
{
	if (_used == _size) {
		reallocate(_size * 2);
	}

	moveConstruct(_end, Move(args)...);
	increment(&_end);
	++_used;
}

template <class T, class Allocator>
const T& Queue<T, Allocator>::first(void) const
{
	assert(_used > 0);
	return *_begin;
}

template <class T, class Allocator>
T& Queue<T, Allocator>::first(void)
{
	assert(_used > 0);
	return *_begin;
}

template <class T, class Allocator>
const T& Queue<T, Allocator>::last(void) const
{
	assert(_used > 0);
	T* end = _end;
	decrement(&end);
	return *end;
}

template <class T, class Allocator>
T& Queue<T, Allocator>::last(void)
{
	assert(_used > 0);
	T* end = _end;
	decrement(&end);
	return *end;
}

template <class T, class Allocator>
size_t Queue<T, Allocator>::size(void) const
{
	return _used;
}

template <class T, class Allocator>
size_t Queue<T, Allocator>::capacity(void) const
{
	return _size;
}

template <class T, class Allocator>
bool Queue<T, Allocator>::empty(void) const
{
	return _used == 0;
}

template <class T, class Allocator>
void Queue<T, Allocator>::setAllocator(const Allocator& allocator)
{
	_allocator = allocator;
}

template <class T, class Allocator>
void Queue<T, Allocator>::increment(T** element, T* array, size_t array_size) const
{
	if (!array) {
		array_size = _size;
		array = _array;
	}

	T* data = *element;

	if (data == array + array_size - 1) {
		*element = array;
	} else {
		*element = data + 1;
	}
}

template <class T, class Allocator>
void Queue<T, Allocator>::decrement(const T** element, T* array, size_t array_size) const
{
	if (!array) {
		array_size = _size;
		array = _array;
	}

	T* data = *element;

	if (data == array) {
		*element = array + array_size - 1;
	} else {
		*element = data - 1;
	}
}

template <class T, class Allocator>
void Queue<T, Allocator>::decrement(T** element, T* array, size_t array_size)
{
	if (!array) {
		array_size = _size;
		array = _array;
	}

	T* data = *element;

	if (data == array) {
		*element = array + array_size - 1;
	} else {
		*element = data - 1;
	}
}
