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
