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

template <class Key, class Value, class Allocator>
Map<Key, Value, Allocator>::Map(const Allocator& allocator):
	_array(allocator), _allocator(allocator)
{
}

template <class Key, class Value, class Allocator>
Map<Key, Value, Allocator>::Map(const Map<Key, Value, Allocator>& rhs):
	_array(rhs._array), _allocator(rhs._allocator)
{
}

template <class Key, class Value, class Allocator>
Map<Key, Value, Allocator>::Map(Map<Key, Value, Allocator>&& rhs):
	_array(Move(rhs._array)), _allocator(rhs._allocator)
{
}

template <class Key, class Value, class Allocator>
Map<Key, Value, Allocator>::~Map(void)
{
}

template <class Key, class Value, class Allocator>
const Map<Key, Value, Allocator>& Map<Key, Value, Allocator>::operator=(const Map<Key, Value, Allocator>& rhs)
{
	_array = rhs._array;
	return *this;
}

template <class Key, class Value, class Allocator>
const Map<Key, Value, Allocator>& Map<Key, Value, Allocator>::operator=(Map<Key, Value, Allocator>&& rhs)
{
	_array = Move(rhs._array);
	return *this;
}

template <class Key, class Value, class Allocator>
bool Map<Key, Value, Allocator>::operator==(const Map<Key, Value, Allocator>& rhs) const
{
	return _array == rhs._array;
}

template <class Key, class Value, class Allocator>
bool Map<Key, Value, Allocator>::operator!=(const Map<Key, Value, Allocator>& rhs) const
{
	return _array != rhs._array;
}

// Dangerous function! Data potentially doesn't exist!
template <class Key, class Value, class Allocator>
const Value& Map<Key, Value, Allocator>::operator[](const Key& key) const
{
	assert(hasElementWithKey(key));
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, KeySearchPredicate());

	return it->second;
}

template <class Key, class Value, class Allocator>
Value& Map<Key, Value, Allocator>::operator[](const Key& key)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, KeySearchPredicate());

	if (it == _array.end() || it->first != key) {
		it = _array.emplaceInsert(it, key, Value());
	}

	return it->second;
}

template <class Key, class Value, class Allocator>
void Map<Key, Value, Allocator>::erase(const Iterator& it)
{
	assert(it != _array.end());
	_array.erase(it);
}

template <class Key, class Value, class Allocator>
void Map<Key, Value, Allocator>::erase(const Key& key)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, KeySearchPredicate());
	assert(it != _array.end() && it->first == key);
	_array.erase(it);
}

template <class Key, class Value, class Allocator>
template <class... Args>
void Map<Key, Value, Allocator>::emplaceInsert(const Key& key, Args&&... args)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, KeySearchPredicate());
	assert(it == _array.end() || it->first != key);
	_array.emplaceMoveInsert(it, key, Value(args...));
}

template <class Key, class Value, class Allocator>
template <class... Args>
void Map<Key, Value, Allocator>::emplaceMoveInsert(const Key& key, Args&&... args)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, KeySearchPredicate());
	assert(it == _array.end() || it->first != key);
	Value temp(Move(args)...);
	_array.emplaceMoveInsert(it, key, Move(temp));
}

template <class Key, class Value, class Allocator>
template <class... Args>
void Map<Key, Value, Allocator>::emplaceMoveMoveInsert(Key&& key, Args&&... args)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, KeySearchPredicate());
	assert(it == _array.end() || it->first != key);
	Value temp(Move(args)...);
	_array.emplaceMoveInsert(it, Move(key), temp);
}

template <class Key, class Value, class Allocator>
void Map<Key, Value, Allocator>::moveMoveInsert(Key&& key, Value&& value)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, KeySearchPredicate());
	assert(it == _array.end() || it->first != key);
	_array.emplaceMoveInsert(it, Move(key), Move(value));
}

template <class Key, class Value, class Allocator>
void Map<Key, Value, Allocator>::moveInsert(const Key& key, Value&& value)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, KeySearchPredicate());
	assert(it == _array.end() || it->first != key);

	Pair<Key, Value> pair;
	pair.first = key;
	pair.second = Move(value);
	_array.emplaceMoveInsert(it, Move(pair));
}

template <class Key, class Value, class Allocator>
void Map<Key, Value, Allocator>::insert(const Key& key, const Value& value)
{
	Iterator it = _array.binarySearch(_array.begin(), _array.end(), key, KeySearchPredicate());
	assert(it == _array.end() || it->first != key);
	_array.emplaceInsert(it, key, value);
}

template <class Key, class Value, class Allocator>
void Map<Key, Value, Allocator>::clearNoFree(void)
{
	_array.clearNoFree();
}

template <class Key, class Value, class Allocator>
void Map<Key, Value, Allocator>::clear(void)
{
	_array.clear();
}

template <class Key, class Value, class Allocator>
void Map<Key, Value, Allocator>::reserve(size_t new_size)
{
	_array.reserve(new_size);
}

template <class Key, class Value, class Allocator>
size_t Map<Key, Value, Allocator>::capacity(void) const
{
	return _array.capacity();
}

template <class Key, class Value, class Allocator>
size_t Map<Key, Value, Allocator>::size(void) const
{
	return _array.size();
}

template <class Key, class Value, class Allocator>
bool Map<Key, Value, Allocator>::empty(void) const
{
	return _array.empty();
}

template <class Key, class Value, class Allocator>
bool Map<Key, Value, Allocator>::hasElementWithValue(const Value& value) const
{
	Iterator it = _array.linearSearch(value, ValueSearchPredicate());
	return it != _array.end();
}

template <class Key, class Value, class Allocator>
bool Map<Key, Value, Allocator>::hasElementWithKey(const Key& key) const
{
	Iterator it = _array.binarySearch(key, KeySearchPredicate());
	return it != _array.end() && it->first == key;
}

template <class Key, class Value, class Allocator>
void Map<Key, Value, Allocator>::setAllocator(const Allocator& allocator)
{
	_array.setAllocator(allocator);
	_allocator = allocator;
}

template <class Key, class Value, class Allocator>
typename Map<Key, Value, Allocator>::Iterator Map<Key, Value, Allocator>::begin(void) const
{
	return _array.begin();
}

template <class Key, class Value, class Allocator>
typename Map<Key, Value, Allocator>::Iterator Map<Key, Value, Allocator>::end(void) const
{
	return _array.end();
}

template <class Key, class Value, class Allocator>
typename Map<Key, Value, Allocator>::Iterator Map<Key, Value, Allocator>::rbegin(void) const
{
	return _array.rbegin();
}

template <class Key, class Value, class Allocator>
typename Map<Key, Value, Allocator>::Iterator Map<Key, Value, Allocator>::rend(void) const
{
	return _array.rend();
}

/*!
	\brief Returns an iterator to the FIRST element that contains \a value.
*/
template <class Key, class Value, class Allocator>
typename Map<Key, Value, Allocator>::Iterator Map<Key, Value, Allocator>::findElementWithValue(const Value& value) const
{
	return _array.linearSearch(value, ValueSearchPredicate());
}

/*!
	\brief Returns an iterator to the element that contains \a key.
*/
template <class Key, class Value, class Allocator>
typename Map<Key, Value, Allocator>::Iterator Map<Key, Value, Allocator>::findElementWithKey(const Key& key) const
{
	Iterator it = _array.binarySearch(key, KeySearchPredicate());
	return (it != _array.end() && it->first == key) ? it : end();
}
