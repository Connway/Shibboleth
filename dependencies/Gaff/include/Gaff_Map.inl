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

template <class Key, class Value, class Allocator, class Predicate>
Map<Key, Value, Allocator, Predicate>::Map(const Allocator& allocator):
	_array(allocator), _allocator(allocator)
{
}

template <class Key, class Value, class Allocator, class Predicate>
Map<Key, Value, Allocator, Predicate>::Map(const Map<Key, Value, Allocator>& rhs):
	_array(rhs._array), _allocator(rhs._allocator)
{
}

template <class Key, class Value, class Allocator, class Predicate>
Map<Key, Value, Allocator, Predicate>::Map(Map<Key, Value, Allocator>&& rhs):
	_array(std::move(rhs._array)), _allocator(rhs._allocator)
{
}

template <class Key, class Value, class Allocator, class Predicate>
Map<Key, Value, Allocator, Predicate>::~Map(void)
{
}

template <class Key, class Value, class Allocator, class Predicate>
const Map<Key, Value, Allocator>& Map<Key, Value, Allocator, Predicate>::operator=(const Map<Key, Value, Allocator>& rhs)
{
	_array = rhs._array;
	return *this;
}

template <class Key, class Value, class Allocator, class Predicate>
const Map<Key, Value, Allocator>& Map<Key, Value, Allocator, Predicate>::operator=(Map<Key, Value, Allocator>&& rhs)
{
	_array = std::move(rhs._array);
	return *this;
}

template <class Key, class Value, class Allocator, class Predicate>
bool Map<Key, Value, Allocator, Predicate>::operator==(const Map<Key, Value, Allocator>& rhs) const
{
	return _array == rhs._array;
}

template <class Key, class Value, class Allocator, class Predicate>
bool Map<Key, Value, Allocator, Predicate>::operator!=(const Map<Key, Value, Allocator>& rhs) const
{
	return _array != rhs._array;
}

// Dangerous function! Data potentially doesn't exist!
template <class Key, class Value, class Allocator, class Predicate>
const Value& Map<Key, Value, Allocator, Predicate>::operator[](const Key& key) const
{
	GAFF_ASSERT(hasElementWithKey(key));
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, Predicate());

	return it->second;
}

template <class Key, class Value, class Allocator, class Predicate>
Value& Map<Key, Value, Allocator, Predicate>::operator[](const Key& key)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, Predicate());

	if (it == _array.end() || it->first != key) {
		it = _array.emplace(it, key, Value());
	}

	return it->second;
}

template <class Key, class Value, class Allocator, class Predicate>
void Map<Key, Value, Allocator, Predicate>::erase(const Iterator& it)
{
	GAFF_ASSERT(it != _array.end());
	_array.erase(it);
}

template <class Key, class Value, class Allocator, class Predicate>
void Map<Key, Value, Allocator, Predicate>::erase(const Key& key)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, Predicate());
	GAFF_ASSERT(it != _array.end() && it->first == key);
	_array.erase(it);
}

template <class Key, class Value, class Allocator, class Predicate>
template <class... Args>
typename Map<Key, Value, Allocator, Predicate>::Iterator Map<Key, Value, Allocator, Predicate>::emplace(const Key& key, Args&&... args)
{
	Value temp(std::forward<Args>(args)...);
	return insert(key, std::move(temp));
}

template <class Key, class Value, class Allocator, class Predicate>
template <class... Args>
typename Map<Key, Value, Allocator, Predicate>::Iterator Map<Key, Value, Allocator, Predicate>::emplace(Key&& key, Args&&... args)
{
	Value temp(std::forward<Args>(args)...);
	return insert(std::move(key), std::move(temp));
}

template <class Key, class Value, class Allocator, class Predicate>
typename Map<Key, Value, Allocator, Predicate>::Iterator Map<Key, Value, Allocator, Predicate>::insert(const Key& key, const Value& value)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, Predicate());
	GAFF_ASSERT(it == _array.end() || it->first != key);

	size_t index = it - _array.begin();
	_array.emplace(it, key, value);

	return _array.begin() + index;
}

template <class Key, class Value, class Allocator, class Predicate>
typename Map<Key, Value, Allocator, Predicate>::Iterator Map<Key, Value, Allocator, Predicate>::insert(const Key& key, Value&& value)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, Predicate());
	GAFF_ASSERT(it == _array.end() || it->first != key);

	size_t index = it - _array.begin();
	_array.emplace(it, key, std::move(value));

	return _array.begin() + index;
}

template <class Key, class Value, class Allocator, class Predicate>
typename Map<Key, Value, Allocator, Predicate>::Iterator Map<Key, Value, Allocator, Predicate>::insert(Key&& key, Value&& value)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, Predicate());
	GAFF_ASSERT(it == _array.end() || it->first != key);

	size_t index = it - _array.begin();
	_array.emplace(it, std::move(key), std::move(value));

	return _array.begin() + index;
}

template <class Key, class Value, class Allocator, class Predicate>
void Map<Key, Value, Allocator, Predicate>::clearNoFree(void)
{
	_array.clearNoFree();
}

template <class Key, class Value, class Allocator, class Predicate>
void Map<Key, Value, Allocator, Predicate>::clear(void)
{
	_array.clear();
}

template <class Key, class Value, class Allocator, class Predicate>
void Map<Key, Value, Allocator, Predicate>::reserve(size_t new_size)
{
	_array.reserve(new_size);
}

template <class Key, class Value, class Allocator, class Predicate>
size_t Map<Key, Value, Allocator, Predicate>::capacity(void) const
{
	return _array.capacity();
}

template <class Key, class Value, class Allocator, class Predicate>
size_t Map<Key, Value, Allocator, Predicate>::size(void) const
{
	return _array.size();
}

template <class Key, class Value, class Allocator, class Predicate>
bool Map<Key, Value, Allocator, Predicate>::empty(void) const
{
	return _array.empty();
}

template <class Key, class Value, class Allocator, class Predicate>
bool Map<Key, Value, Allocator, Predicate>::hasElementWithValue(const Value& value) const
{
	Iterator it = _array.linearSearch(value, ValueSearchPredicate());
	return it != _array.end();
}

template <class Key, class Value, class Allocator, class Predicate>
bool Map<Key, Value, Allocator, Predicate>::hasElementWithKey(const Key& key) const
{
	Iterator it = _array.binarySearch(key, Predicate());
	return it != _array.end() && it->first == key;
}

template <class Key, class Value, class Allocator, class Predicate>
void Map<Key, Value, Allocator, Predicate>::setAllocator(const Allocator& allocator)
{
	_array.setAllocator(allocator);
	_allocator = allocator;
}

template <class Key, class Value, class Allocator, class Predicate>
typename Map<Key, Value, Allocator, Predicate>::Iterator Map<Key, Value, Allocator, Predicate>::begin(void) const
{
	return _array.begin();
}

template <class Key, class Value, class Allocator, class Predicate>
typename Map<Key, Value, Allocator, Predicate>::Iterator Map<Key, Value, Allocator, Predicate>::end(void) const
{
	return _array.end();
}

template <class Key, class Value, class Allocator, class Predicate>
typename Map<Key, Value, Allocator, Predicate>::Iterator Map<Key, Value, Allocator, Predicate>::rbegin(void) const
{
	return _array.rbegin();
}

template <class Key, class Value, class Allocator, class Predicate>
typename Map<Key, Value, Allocator, Predicate>::Iterator Map<Key, Value, Allocator, Predicate>::rend(void) const
{
	return _array.rend();
}

template <class Key, class Value, class Allocator, class Predicate>
typename Map<Key, Value, Allocator, Predicate>::Iterator Map<Key, Value, Allocator, Predicate>::findElementWithValue(const Value& value) const
{
	return _array.linearSearch(value, ValueSearchPredicate());
}

template <class Key, class Value, class Allocator, class Predicate>
typename Map<Key, Value, Allocator, Predicate>::Iterator Map<Key, Value, Allocator, Predicate>::findElementWithKey(const Key& key) const
{
	Iterator it = _array.binarySearch(key, Predicate());
	return (it != _array.end() && it->first == key) ? it : end();
}
