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

template <class T, class Allocator>
Array2D<T, Allocator>::Array2D(const Allocator& allocator):
	_array(nullptr), _width(0), _height(0), _allocator(allocator)
{
}

template <class T, class Allocator>
Array2D<T, Allocator>::Array2D(size_t width, size_t height, T& init_val, const Allocator& allocator):
	_array(nullptr), _width(width), _height(height), _allocator(allocator)
{
	_array = allocator.template allocArrayT<T>(width * height, init_val);
}

template <class T, class Allocator>
Array2D<T, Allocator>::Array2D(size_t width, size_t height, const Allocator& allocator):
_array(nullptr), _width(width), _height(height), _allocator(allocator)
{
	_array = allocator.template allocArrayT<T>(width * height);
}

template <class T, class Allocator>
Array2D<T, Allocator>::Array2D(const T** data, size_t width, size_t height, const Allocator& allocator):
	_array(nullptr), _width(width), _height(height), _allocator(allocator)
{
	_array = reinterpret_cast<T*>(allocator.alloc(sizeof(T) * width * height));

	for (size_t i = 0; i < height; ++i) {
		for (size_t j = 0; j < width; ++j) {
			construct(_array + i * width + j, data[i][j]);
		}
	}
}

template <class T, class Allocator>
Array2D<T, Allocator>::Array2D(const T* data, size_t width, size_t height, const Allocator& allocator):
	_array(nullptr), _width(width), _height(height), _allocator(allocator)
{
	_array = reinterpret_cast<T*>(allocator.alloc(sizeof(T) * width * height));

	size_t size = width * height;

	for (size_t i = 0; i < size; ++i) {
		construct(_array + i, data[i]);
	}
}

template <class T, class Allocator>
Array2D<T, Allocator>::Array2D(const Array2D<T, Allocator>& rhs):
	_array(nullptr), _width(0), _height(0), _allocator(rhs._allocator)
{
	*this = rhs;
}

template <class T, class Allocator>
Array2D<T, Allocator>::Array2D(Array2D<T, Allocator>&& rhs):
	_array(rhs._array), _width(rhs._width), _height(rhs._height), _allocator(rhs._allocator)
{
	rhs._array = nullptr;
	rhs._width = rhs._height = 0;
}

template <class T, class Allocator>
Array2D<T, Allocator>::~Array2D(void)
{
	clear();
}

template <class T, class Allocator>
const Array2D<T, Allocator>& Array2D<T, Allocator>::operator=(const Array2D<T, Allocator>& rhs)
{
	clear();

	_array = reinterpret_cast<T*>(_allocator.alloc(sizeof(T) * rhs._width * rhs._height));
	_width = rhs._width;
	_height = rhs._height;

	size_t size = _width * _height;

	for (size_t i = 0; i < size; ++i) {
		construct(_array + i, rhs._array[i]);
	}

	return *this;
}

template <class T, class Allocator>
const Array2D<T, Allocator>& Array2D<T, Allocator>::operator=(Array2D<T, Allocator>&& rhs)
{
	_array = rhs._array;
	_width = rhs._width;
	_height = rhs._height;

	rhs._array = nullptr;
	rhs._width = rhs._height = 0;

	return *this;
}

template <class T, class Allocator>
const T* Array2D<T, Allocator>::operator[](size_t index) const
{
	assert(index < _height);
	return _array + index * _height;
}

template <class T, class Allocator>
T* Array2D<T, Allocator>::operator[](size_t index)
{
	assert(index < _height);
	return _array + index * _height;
}

template <class T, class Allocator>
void Array2D<T, Allocator>::clear(void)
{
	if (_array) {
		_allocator.freeArrayT(_array, _width * _height);
		_width = _height = 0;
		_array = nullptr;
	}
}

template <class T, class Allocator>
void Array2D<T, Allocator>::resize(size_t width, size_t height)
{
	T* old_array = _array;

	_array = reinterpret_cast<T*>(_allocator.alloc(sizeof(T) * width * height));

	size_t min_width = Min(width, _width);
	size_t min_height = Min(height, _height);

	if (old_array) {
		// copy over old elements and construct new elements
		for (size_t i = 0; i < height; ++i) {
			if (i < min_height) {
				memcpy(_array + i * width, old_array + i * _width, sizeof(T) * min_width);

				for (size_t j = min_width; j < width; ++j) {
					construct(_array + i * width + j);
				}

				for (size_t j = min_width; j < _width; ++j) {
					deconstruct(old_array + i * _width + j);
				}
			}
		}

		_allocator.free(old_array);
	}

	_width = width;
	_height = height;
}

template <class T, class Allocator>
const T& Array2D<T, Allocator>::at(size_t x, size_t y) const
{
	assert(x < _width && y < _height);
	return _array[y * _width + x];
}

template <class T, class Allocator>
T& Array2D<T, Allocator>::at(size_t x, size_t y)
{
	assert(x < _width && y < _height);
	return _array[y * _width + x];
}
