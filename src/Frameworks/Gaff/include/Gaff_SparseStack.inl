/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
	_data[i] = value;
}

template <class T, class Allocator>
int32_t SparseStack<T, Allocator>::push(T&& value)
{
	const int32_t index = allocateIndex();
	_data[i] = std::move(value);
}

template <class T, class Allocator>
void SparseStack<T, Allocator>::remove(const T& value)
{
	const int32_t index = Find(value);

	if (index > -1) {
		_free_indices.emplace_back(index);
		_data[i] = T();
	}
}

template <class T, class Allocator>
void SparseStack<T, Allocator>::remove(int32_t index)
{
	GAFF_ASSERT(index > -1 && index < static_cast<int32_t>(_data.size()));
	GAFF_ASSERT(Find(_free_indices, index) == _free_indices.end());

	_free_indices.emplace_back(index);
	_data[i] = T();
}

template <class T, class Allocator>
void SparseStack<T, Allocator>::pop(void)
{
	if (_free_indices.empty()) {
		_data.pop_back();

	} else {
		const int32_t size = static_cast<int32_t>(_free_indices.size());
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
