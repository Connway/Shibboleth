/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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
SparseStack<T, Allocator>::Iterator::Iterator(SparseStack* stack, int32_t index):
	_stack(stack), _index(index)
{
	GAFF_ASSERT(stack);
}

template <class T, class Allocator>
SparseStack<T, Allocator>* SparseStack<T, Allocator>::Iterator::getStack(void) const
{
	return _stack;
}

template <class T, class Allocator>
int32_t SparseStack<T, Allocator>::Iterator::getIndex(void) const
{
	return _index;
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::Iterator SparseStack<T, Allocator>::Iterator::operator++(int)
{
	Iterator temp = *this;
	++(*this);
	return temp;
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::Iterator& SparseStack<T, Allocator>::Iterator::operator++()
{
	_index = _stack->getNextIndex(_index);
	return *this;
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::Iterator SparseStack<T, Allocator>::Iterator::operator--(int)
{
	Iterator temp = *this;
	--(*this);
	return temp;
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::Iterator& SparseStack<T, Allocator>::Iterator::operator--()
{
	_index = _stack->getPreviousIndex(_index);
	return *this;
}

template <class T, class Allocator>
bool SparseStack<T, Allocator>::Iterator::operator==(const Iterator& rhs) const
{
	return _stack == rhs._stack && _index == rhs._index;
}

template <class T, class Allocator>
bool SparseStack<T, Allocator>::Iterator::operator!=(const Iterator& rhs) const
{
	return _stack != rhs._stack || _index != rhs._index;
}

template <class T, class Allocator>
bool SparseStack<T, Allocator>::Iterator::operator<=(const Iterator& rhs) const
{
	return _stack == rhs._stack && _index <= rhs._index;
}

template <class T, class Allocator>
bool SparseStack<T, Allocator>::Iterator::operator<(const Iterator& rhs) const
{
	return _stack == rhs._stack && _index < rhs._index;
}

template <class T, class Allocator>
bool SparseStack<T, Allocator>::Iterator::operator>=(const Iterator& rhs) const
{
	return _stack == rhs._stack && _index >= rhs._index;
}

template <class T, class Allocator>
bool SparseStack<T, Allocator>::Iterator::operator>(const Iterator& rhs) const
{
	return _stack == rhs._stack && _index > rhs._index;
}

template <class T, class Allocator>
const T* SparseStack<T, Allocator>::Iterator::operator->(void) const
{
	return &(**this);
}

template <class T, class Allocator>
T* SparseStack<T, Allocator>::Iterator::operator->(void)
{
	return &(**this);
}

template <class T, class Allocator>
T& SparseStack<T, Allocator>::Iterator::operator*(void) const
{
	return **const_cast<Iterator*>(this);
}

template <class T, class Allocator>
T& SparseStack<T, Allocator>::Iterator::operator*(void)
{
	return (*_stack)[_index];
}



template <class T, class Allocator>
SparseStack<T, Allocator>::SparseStack(size_t initial_capacity, const Allocator& allocator):
	SparseStack(allocator)
{
	_data.set_capacity(initial_capacity);
}

template <class T, class Allocator>
SparseStack<T, Allocator>::SparseStack(const Allocator& allocator):
	_free_indices(allocator), _data(allocator)
{
}

template <class T, class Allocator>
template <class... Args>
int32_t SparseStack<T, Allocator>::emplace(Args&&... args)
{
	T value(std::forward<Args>(args)...);
	return push(std::move(value));
}

template <class T, class Allocator>
int32_t SparseStack<T, Allocator>::push(const T& value)
{
	const int32_t index = allocateIndex();
	_data[index] = value;
	return index;
}

template <class T, class Allocator>
int32_t SparseStack<T, Allocator>::push(T&& value)
{
	const int32_t index = allocateIndex();
	_data[index] = std::move(value);
	return index;
}

template <class T, class Allocator>
void SparseStack<T, Allocator>::remove(const T& value)
{
	const int32_t index = Find(value);

	if (index > -1) {
		_free_indices.emplace_back(index);
		_data[index] = T();
	}
}

template <class T, class Allocator>
void SparseStack<T, Allocator>::remove(int32_t index)
{
	GAFF_ASSERT(validIndex(index));

	_free_indices.emplace_back(index);
	_data[index] = T();
}

template <class T, class Allocator>
bool SparseStack<T, Allocator>::validIndex(int32_t index) const
{
	return index > -1 && index < static_cast<int32_t>(_data.size()) && Find(_free_indices, index) == _free_indices.end();
}

template <class T, class Allocator>
bool SparseStack<T, Allocator>::empty(void) const
{
	return _free_indices.size() == _data.size() || _data.empty();
}

template <class T, class Allocator>
void SparseStack<T, Allocator>::pop(void)
{
	if (_free_indices.empty()) {
		_data.pop_back();

	} else {
		int32_t last_index = static_cast<int32_t>(_data.size()) - 1;

		auto it = Find(_free_indices, last_index);

		while (it != _free_indices.end()) {
			--last_index;

			it = Find(_free_indices, last_index);
		}

		remove(last_index);
	}
}

template <class T, class Allocator>
int32_t SparseStack<T, Allocator>::find(const T& value) const
{
	const int32_t size = static_cast<int32_t>(_data.size());

	for (int32_t i = 0; i < size; ++i) {
		if (Find(_free_indices, i) != _free_indices.end()) {
			continue;
		}

		if (_data[i] == value) {
			return i;
		}
	}

	return -1;
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::ConstIterator SparseStack<T, Allocator>::begin(void) const
{
	return ConstIterator(const_cast<SparseStack*>(this)->begin());
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::ConstIterator SparseStack<T, Allocator>::end(void) const
{
	return ConstIterator(const_cast<SparseStack*>(this)->end());
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::Iterator SparseStack<T, Allocator>::begin(void)
{
	if (empty()) {
		return end();
	}

	int32_t index = 0;

	while (Find(_free_indices, index) != _free_indices.end()) {
		++index;
	}

	if (index >= static_cast<int32_t>(_data.size())) {
		index = -1;
	}

	return Iterator(this, index);
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::Iterator SparseStack<T, Allocator>::end(void)
{
	return Iterator(this, static_cast<int32_t>(_data.size()));
}

template <class T, class Allocator>
const T& SparseStack<T, Allocator>::operator[](int32_t index) const
{
	return (*const_cast<SparseStack*>(this))[index];
}

template <class T, class Allocator>
T& SparseStack<T, Allocator>::operator[](int32_t index)
{
	GAFF_ASSERT(validIndex(index));
	return _data[static_cast<size_t>(index)];
}

template <class T, class Allocator>
int32_t SparseStack<T, Allocator>::getPreviousIndex(int32_t index) const
{
	--index;

	// Find the next used slot.
	while (Find(_free_indices, index) != _free_indices.end()) {
		--index;
	}

	// If we went past the end, invalidate the iterator.
	if (index < 0) {
		index = static_cast<int32_t>(_data.size());
	}

	return index;
}

template <class T, class Allocator>
int32_t SparseStack<T, Allocator>::getNextIndex(int32_t index) const
{
	++index;

	// Find the next used slot.
	while (Find(_free_indices, index) != _free_indices.end()) {
		++index;
	}

	index = Gaff::Min(index, static_cast<int32_t>(_data.size()));

	return index;
}

template <class T, class Allocator>
int32_t SparseStack<T, Allocator>::allocateIndex(void)
{
	int32_t index = -1;

	if (_free_indices.empty()) {
		index = static_cast<int32_t>(_data.size());
		_data.emplace_back();

	} else {
		index = _free_indices.back();
		_free_indices.pop_back();
	}

	return index;
}
