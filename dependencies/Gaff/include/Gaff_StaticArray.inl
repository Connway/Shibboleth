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

#define STATIC_ARRAY_ITERATOR typename StaticArray<T, array_size>::Iterator

template <class T, size_t array_size>
StaticArray<T, array_size>::StaticArray(const T& init_val)
{
	for (size_t i = 0; i < array_size; ++i) {
		_array[i] = init_val;
	}
}

template <class T, size_t array_size>
StaticArray<T, array_size>::StaticArray(const StaticArray<T, array_size>& rhs)
{
	for (size_t i = 0; i < array_size; ++i) {
		_array[i] = rhs._array[i];
	}
}

template <class T, size_t array_size>
StaticArray<T, array_size>::StaticArray(StaticArray<T, array_size>&& rhs)
{
	for (size_t i = 0; i < array_size; ++i) {
		_array[i] = std::move(rhs._array[i]);
	}
}

template <class T, size_t array_size>
StaticArray<T, array_size>::StaticArray(void)
{
}

template <class T, size_t array_size>
StaticArray<T, array_size>::~StaticArray(void)
{
}

template <class T, size_t array_size>
const StaticArray<T, array_size>& StaticArray<T, array_size>::operator=(const StaticArray<T, array_size>& rhs)
{
	for (size_t i = 0; i < array_size; ++i) {
		_array[i] = rhs._array[i];
	}
}

template <class T, size_t array_size>
const StaticArray<T, array_size>& StaticArray<T, array_size>::operator=(StaticArray<T, array_size>&& rhs)
{
	for (size_t i = 0; i < array_size; ++i) {
		_array[i] = std::move(rhs._array[i]);
	}
}

template <class T, size_t array_size>
bool StaticArray<T, array_size>::operator==(const StaticArray<T, array_size>& rhs) const
{
	for (size_t i = 0; i < array_size; ++i) {
		if (_array[i] != rhs._array[i]) {
			return false;
		}
	}

	return true;
}

template <class T, size_t array_size>
bool StaticArray<T, array_size>::operator!=(const StaticArray<T, array_size>& rhs) const
{
	for (size_t i = 0; i < array_size; ++i) {
		if (_array[i] == rhs._array[i]) {
			return false;
		}
	}

	return true;
}

template <class T, size_t array_size>
const T& StaticArray<T, array_size>::operator[](size_t index) const
{
	assert(index < array_size);
	return _array[index];
}

template <class T, size_t array_size>
T& StaticArray<T, array_size>::operator[](size_t index)
{
	assert(index < array_size);
	return _array[index];
}

template <class T, size_t array_size>
const T& StaticArray<T, array_size>::first(void) const
{
	return _array[0];
}

template <class T, size_t array_size>
T& StaticArray<T, array_size>::first(void)
{
	return _array[0];
}

template <class T, size_t array_size>
const T& StaticArray<T, array_size>::last(void) const
{
	return _array[array_size - 1];
}

template <class T, size_t array_size>
T& StaticArray<T, array_size>::last(void)
{
	return _array[array_size - 1];
}

template <class T, size_t array_size>
const T* StaticArray<T, array_size>::getArray(void) const
{
	return _array;
}

template <class T, size_t array_size>
T* StaticArray<T, array_size>::getArray(void)
{
	return _array;
}

template <class T, size_t array_size>
STATIC_ARRAY_ITERATOR StaticArray<T, array_size>::begin(void) const
{
	return _array;
}

template <class T, size_t array_size>
STATIC_ARRAY_ITERATOR StaticArray<T, array_size>::end(void) const
{
	return _array + array_size;
}

template <class T, size_t array_size>
STATIC_ARRAY_ITERATOR StaticArray<T, array_size>::rbegin(void) const
{
	return _array + array_size - 1;
}

template <class T, size_t array_size>
STATIC_ARRAY_ITERATOR StaticArray<T, array_size>::rend(void) const
{
	return _array - 1;
}

template <class T, size_t array_size>
size_t StaticArray<T, array_size>::size(void) const
{
	return array_size;
	
}

template <class T, size_t array_size>
template <class T2, class Pred>
STATIC_ARRAY_ITERATOR StaticArray<T, array_size>::linearSearch(
	const STATIC_ARRAY_ITERATOR range_begin,
	const STATIC_ARRAY_ITERATOR range_end,
	const T2& data,
	const Pred& pred) const
{
	assert(range_begin >= _array && range_begin <= _array + array_size);
	assert(range_end >= _array && range_end <= _array + array_size);
	assert(range_begin <= range_end);

	size_t index1 = static_cast<size_t>(range_begin - _array);
	size_t index2 = static_cast<size_t>(range_end - _array);

	size_t result = linearSearch(index1, index2, data, pred);
	return (result != SIZE_T_FAIL) ? Iterator(_array + result) : end();
}

template <class T, size_t array_size>
template <class T2, class Pred>
size_t StaticArray<T, array_size>::linearSearch(size_t range_begin, size_t range_end, const T2& data, const Pred& pred) const
{
	assert(range_begin <= range_end && range_end <= array_size);

	for (size_t i = range_begin; i < range_end; ++i) {
		if (pred(_array[i], data)) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

template <class T, size_t array_size>
template <class T2, class Pred>
STATIC_ARRAY_ITERATOR StaticArray<T, array_size>::linearSearch(const T2& data, const Pred& pred) const
{
	return linearSearch(begin(), end(), data, pred);
}

template <class T, size_t array_size>
template <class T2, class Pred>
STATIC_ARRAY_ITERATOR StaticArray<T, array_size>::binarySearch(
	const STATIC_ARRAY_ITERATOR range_begin,
	const STATIC_ARRAY_ITERATOR range_end,
	const T2& data,
	const Pred& pred) const
{
	assert(range_begin >= _array && range_begin <= _array + array_size);
	assert(range_end >= _array && range_end <= _array + array_size);
	assert(range_begin <= range_end);

	size_t index1 = static_cast<size_t>(range_begin - _array);
	size_t index2 = static_cast<size_t>(range_end - _array);

	size_t result = binarySearch(index1, index2, data, pred);
	return (result != SIZE_T_FAIL) ? Iterator(_array + result) : end();
}

template <class T, size_t array_size>
template <class T2, class Pred>
size_t StaticArray<T, array_size>::binarySearch(size_t range_begin, size_t range_end, const T2& data, const Pred& pred) const
{
	assert(range_end <= array_size && range_begin <= range_end);

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

template <class T, size_t array_size>
template <class T2, class Pred>
STATIC_ARRAY_ITERATOR StaticArray<T, array_size>::binarySearch(const T2& data, const Pred& pred) const
{
	return binarySearch(begin(), end(), data, pred);
}
