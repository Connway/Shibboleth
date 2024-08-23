/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
bool SparseStack<T, Allocator>::Iterator::valid(void) const
{
	return _stack && _stack->validIndex(_index);
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::Iterator SparseStack<T, Allocator>::Iterator::operator+(int32_t offset) const
{
	Iterator temp = *this;
	temp += offset;
	return temp;
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::Iterator& SparseStack<T, Allocator>::Iterator::operator+=(int32_t offset)
{
	if (offset < 0) {
		(*this) -= abs(offset);
		return *this;
	}

	for (int32_t i = 0; i < offset; ++i) {
		++(*this);
	}

	return *this;
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::Iterator SparseStack<T, Allocator>::Iterator::operator-(int32_t offset) const
{
	Iterator temp = *this;
	temp -= offset;
	return temp;
}

template <class T, class Allocator>
typename SparseStack<T, Allocator>::Iterator& SparseStack<T, Allocator>::Iterator::operator-=(int32_t offset)
{
	if (offset < 0) {
		(*this) -= abs(offset);
		return *this;
	}

	for (int32_t i = 0; i < offset; ++i) {
		--(*this);
	}

	return *this;
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
	const int32_t index = find(value);

	if (index > -1) {
		remove(index);
	}
}

template <class T, class Allocator>
void SparseStack<T, Allocator>::removeAt(int32_t index)
{
	GAFF_ASSERT(validIndex(index));

	++_free_index_count;

	// All indices are free. Just clear all the arrays.
	if (_free_index_count == static_cast<int32_t>(_data.size())) {
		_free_indices.clear();
		_data.clear();

		_first_free_index = -1;
		_free_index_count = 0;

		return;
	}

	// Removing last element, just pop off the back.
	if (index == static_cast<int32_t>(_data.size()) - 1) {
		_free_indices.pop_back();
		_data.pop_back();

		if (_first_free_index == -1) {
			_first_free_index = index;
		}

	} else {
		_free_indices.set(static_cast<size_t>(index), true);
		_data[index] = T();

		if (_first_free_index == -1) {
			_first_free_index = index;
		} else {
			_first_free_index = Min(_first_free_index, index);
		}

		// If all indices above us are free, just remove all elements at index and above.
		for (int32_t i = index; i < static_cast<int32_t>(_free_indices.size()); ++i) {
			if (!_free_indices[i]) {
				return;
			}
		}

		_free_indices.erase(_free_indices.begin() + index, _free_indices.end());
		_data.erase(_data.begin() + index, _data.end());

		_free_index_count -= static_cast<int32_t>(_data.size()) - index;

		if (!_free_index_count) {
			_first_free_index = -1;
		}
	}
}

template <class T, class Allocator>
bool SparseStack<T, Allocator>::validIndex(int32_t index) const
{
	return index >= 0 && index < static_cast<int32_t>(_data.size()) && !_free_indices[static_cast<size_t>(index)];
}

template <class T, class Allocator>
bool SparseStack<T, Allocator>::empty(void) const
{
	return _data.empty();
}

template <class T, class Allocator>
void SparseStack<T, Allocator>::pop(void)
{
	GAFF_ASSERT(!_data.empty());

	const int32_t last_index = getPreviousIndex(static_cast<int32_t>(_data.size()) - 1);

	GAFF_ASSERT(last_index >= 0);
	remove(last_index);
}

template <class T, class Allocator>
template <class U, class Predicate>
int32_t SparseStack<T, Allocator>::find(const U& value, Predicate predicate) const
{
	const int32_t size = static_cast<int32_t>(_data.size());

	for (int32_t i = 0; i < size; i = getNextIndex(i)) {
		if (predicate(_data[i], value)) {
			return i;
		}
	}

	return -1;
}

template <class T, class Allocator>
template <class U>
int32_t SparseStack<T, Allocator>::find(const U& value) const
{
	const int32_t size = static_cast<int32_t>(_data.size());

	for (int32_t i = 0; i < size; i = getNextIndex(i)) {
		if (_data[i] == value) {
			return i;
		}
	}

	return -1;
}

template <class T, class Allocator>
int32_t SparseStack<T, Allocator>::getValidSize() const
{
	return static_cast<int32_t>(_data.size()) - _free_index_count;
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

	return ++Iterator(this, -1);
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
	while (index >= 0 && _free_indices[static_cast<size_t>(index)]) {
		--index;
	}

	return index;
}

template <class T, class Allocator>
int32_t SparseStack<T, Allocator>::getNextIndex(int32_t index) const
{
	++index;

	// Find the next used slot.
	while (index < static_cast<int32_t>(_data.size()) && _free_indices[static_cast<size_t>(index)]) {
		++index;
	}

	return index;
}

template <class T, class Allocator>
int32_t SparseStack<T, Allocator>::allocateIndex(void)
{
	int32_t index = -1;

	if (_first_free_index >= 0) {
		index = _first_free_index;

		_free_indices.set(static_cast<size_t>(index), false);
		_first_free_index = getNextIndex(_first_free_index);

		if (_first_free_index == static_cast<int32_t>(_data.size())) {
			_first_free_index = -1;
		}

	} else {
		index = static_cast<int32_t>(_data.size());
		_free_indices.push_back(false);
		_data.emplace_back();
	}

	return index;
}
