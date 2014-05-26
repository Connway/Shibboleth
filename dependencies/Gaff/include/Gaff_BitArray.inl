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

template <class Allocator>
BitArray<Allocator>::BitArray(const Allocator& allocator):
	_bit_array(allocator), _used(0), _size(0)
{
}

template <class Allocator>
BitArray<Allocator>::BitArray(unsigned int start_alloc, const Allocator& allocator):
	_bit_array(CalculateBytes(start_alloc), 0, allocator), _used(start_alloc),
	_size(_bit_array.size() * BITS_PER_BYTE)
{
}

template <class Allocator>
BitArray<Allocator>::BitArray(unsigned int start_alloc, bool init_val, const Allocator& allocator):
	_bit_array(CalculateBytes(start_alloc), init_val, allocator), _used(start_alloc),
	_size(_bit_array.size() * BITS_PER_BYTE)
{
}

template <class Allocator>
BitArray<Allocator>::BitArray(const BitArray<Allocator>& rhs):
	_bit_array(rhs._bit_array), _used(rhs._used), _size(rhs._size)
{
}

template <class Allocator>
BitArray<Allocator>::BitArray(BitArray<Allocator>&& rhs):
	_bit_array(Move(rhs._bit_array)), _used(rhs._used), _size(rhs._size)
{
}

template <class Allocator>
BitArray<Allocator>::~BitArray(void)
{
	clear();
}

template <class Allocator>
const BitArray<Allocator>& BitArray<Allocator>::operator=(const BitArray<Allocator>& rhs)
{
	_bit_array = rhs._bit_array;
	_used = rhs._used;
	_size = rhs._size;
}

template <class Allocator>
const BitArray<Allocator>& BitArray<Allocator>::operator=(BitArray<Allocator>&& rhs)
{
	_bit_array = Move(rhs);
	_used = rhs._used;
	_size = rhs._size;

	rhs._used = 0;
	rhs._size = 0;

	return *this;
}

template <class Allocator>
template <class Allocator2>
bool BitArray<Allocator>::operator==(const BitArray<Allocator2>& rhs) const
{
	if (this == &rhs) {
		return true;
	}

	if (_used != rhs._used) {
		return false;
	}

	for (unsigned int i = 0; i < _used; ++i) {
		if ((*this)[i] != rhs[i]) {
			return false;
		}
	}

	return true;
}

template <class Allocator>
template <class Allocator2>
bool BitArray<Allocator>::operator!=(const BitArray<Allocator2>& rhs) const
{
	return !(*this == rhs);
}

template <class Allocator>
bool BitArray<Allocator>::operator[](unsigned int index) const
{
	assert(index < _used);
	unsigned int array_index, shift;
	CalculateIndexAndShift(index, array_index, shift);
	return (_bit_array[array_index] & (1 << shift)) != 0;
}

template <class Allocator>
void BitArray<Allocator>::setBit(unsigned int index, bool value)
{
	assert(index < _used);

	if (value) {
		set(index);
	} else {
		unset(index);
	}
}

template <class Allocator>
void BitArray<Allocator>::unset(unsigned int index)
{
	assert(index < _used);
	unsigned int array_index, shift;
	CalculateIndexAndShift(index, array_index, shift);
	_bit_array[array_index] &= ~(1 << shift);
}

template <class Allocator>
void BitArray<Allocator>::set(unsigned int index)
{
	assert(index < _used);
	unsigned int array_index, shift;
	CalculateIndexAndShift(index, array_index, shift);
	_bit_array[array_index] |= (1 << shift);
}

template <class Allocator>
void BitArray<Allocator>::clear(void)
{
	_bit_array.clear();
	_used = 0;
	_size = 0;
}

template <class Allocator>
void BitArray<Allocator>::clearNoFree(void)
{
	_bit_array.clearNoFree();
	_used = 0;
}

template <class Allocator>
bool BitArray<Allocator>::first(void) const
{
	assert(_used);
	return (*this)[0];
}

template <class Allocator>
bool BitArray<Allocator>::last(void) const
{
	assert(_used);
	return (*this)[_used - 1];
}

template <class Allocator>
bool BitArray<Allocator>::empty(void) const
{
	return !_used;
}

template <class Allocator>
unsigned int BitArray<Allocator>::size(void) const
{
	return _used;
}

template <class Allocator>
unsigned int BitArray<Allocator>::capacity(void) const
{
	return _size;
}

template <class Allocator>
void BitArray<Allocator>::push(bool value)
{
	if (_used == _size) {
		if (_size == 0) {
			reserve(1);
		} else {
			reserve(_size * 2);
		}
	}

	++_used; // Avoids assert in set()/unset()

	// I doubt this will be an issue, but this could be optimized
	// by using a table of member function pointers.
	// Would look like _push_funcs[value](_used);
	if (value) {
		set(_used - 1);
	} else {
		unset(_used - 1);
	}
}

template <class Allocator>
void BitArray<Allocator>::pop(void)
{
	assert(_used);
	--_used;
}

// This function is probably crazy expensive for what it does
template <class Allocator>
void BitArray<Allocator>::insert(bool value, unsigned int index)
{
	assert(index <= _size);

	if (_used + 1 > _size) {
		if (_size == 0) {
			reserve(1);
		} else {
			reserve(_size * 2);
		}
	}

	++_used; // Doing this here so that calls to set()/unset() won't assert

	// Shift all the bits up one
	for (unsigned int i = _used - 2; i >= index; --i) {
		bool temp = (*this)[i];

		if (temp) {
			set(i + 1);
		} else {
			unset(i + 1);
		}
	}

	if (value) {
		set(index);
	} else {
		unset(index);
	}
}

template <class Allocator>
void BitArray<Allocator>::erase(unsigned int index)
{
	assert(index < _used && _used > 0);

	// Shift all the bits down one
	for (unsigned int i = index; i < _used - 1; ++i) {
		bool temp = (*this)[i + 1];

		if (temp) {
			set(i);
		} else {
			unset(i);
		}
	}

	--_used;
}

template <class Allocator>
void BitArray<Allocator>::resize(unsigned int new_size)
{
	if (new_size == _size) {
		return;
	}

	_bit_array.resize(CalculateBytes(new_size));
	_used = _size = _bit_array.size() * BITS_PER_BYTE;
}

template <class Allocator>
void BitArray<Allocator>::reserve(unsigned int reserve_size)
{
	if (reserve_size <= _size) {
		return;
	}

	_bit_array.resize(CalculateBytes(reserve_size));
	_size = _bit_array.size() * BITS_PER_BYTE;
}

template <class Allocator>
void BitArray<Allocator>::setAllocator(const Allocator& allocator)
{
	_bit_array.setAllocator(allocator);
	_allocator = allocator;
}

template <class Allocator>
void BitArray<Allocator>::CalculateIndexAndShift(unsigned int index, unsigned int& array_index, unsigned int& shift)
{
	array_index = index / BITS_PER_BYTE;
	shift = index - array_index * BITS_PER_BYTE;
}

template <class Allocator>
unsigned int BitArray<Allocator>::CalculateBytes(unsigned int bits)
{
	float final_bits = (float)bits / (float)BITS_PER_BYTE;
	final_bits = ceilf(final_bits);
	return (unsigned int)final_bits;
}
