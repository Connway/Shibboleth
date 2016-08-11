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

#define HASHMAP_ITERATOR(TYPE) HashMap<TYPE, Value, Allocator>::Iterator

template <class Key, class HashType>
HashType HashKey(const Key& key, HashType (*hash_func)(const char*, size_t))
{
	if (hash_func) {
		return hash_func(reinterpret_cast<const char*>(&key), sizeof(Key));
	}

	ContainerHash hash = CONT_HASH(reinterpret_cast<const char*>(&key), sizeof(Key));
	return static_cast<HashType>(hash);
}

template <class T, class Allocator, class HashType>
HashType HashKey(const String<T, Allocator>& key, HashType (*hash_func)(const char*, size_t))
{
	if (hash_func) {
		return hash_func(key.getBuffer(), key.size() * sizeof(T));
	}

	ContainerHash hash = CONT_HASH(key.getBuffer(), key.size() * sizeof(T));
	return static_cast<HashType>(hash);
}

template <class T, class Allocator, class HashType>
HashType HashKey(const HashString<T, Allocator, HashType>& key, HashType(*)(const char*, size_t))
{
	return key.getHash();
}

////////////////////
//    Iterator    //
////////////////////
template <class Key, class Value, class Allocator, class HashType>
HashMap<Key, Value, Allocator, HashType>::Iterator::Iterator(const Iterator& it):
	_slot(it._slot), _rend(it._rend), _end(it._end)
{
}

template <class Key, class Value, class Allocator, class HashType>
HashMap<Key, Value, Allocator, HashType>::Iterator::Iterator(void):
	_slot(nullptr), _rend(nullptr), _end(nullptr)
{
}

template <class Key, class Value, class Allocator, class HashType>
const typename HashMap<Key, Value, Allocator, HashType>::Iterator& HashMap<Key, Value, Allocator, HashType>::Iterator::backward(void) const
{
	while (_slot != _rend) {
		--_slot;

		if (_slot == _rend || _slot->occupied) {
			break;
		}
	}

	return *this;
}

template <class Key, class Value, class Allocator, class HashType>
const typename HashMap<Key, Value, Allocator, HashType>::Iterator& HashMap<Key, Value, Allocator, HashType>::Iterator::forward(void) const
{
	while (_slot != _end) {
		++_slot;

		if (_slot == _end || _slot->occupied) {
			break;
		}
	}

	return *this;
}

template <class Key, class Value, class Allocator, class HashType>
const typename HashMap<Key, Value, Allocator, HashType>::Iterator& HashMap<Key, Value, Allocator, HashType>::Iterator::operator++(void) const
{
	return forward();
}

template <class Key, class Value, class Allocator, class HashType>
const typename HashMap<Key, Value, Allocator, HashType>::Iterator& HashMap<Key, Value, Allocator, HashType>::Iterator::operator--(void) const
{
	return backward();
}

template <class Key, class Value, class Allocator, class HashType>
typename HashMap<Key, Value, Allocator, HashType>::Iterator HashMap<Key, Value, Allocator, HashType>::Iterator::operator++(int) const
{
	const Iterator it = *this;
	forward();
	return it;
}

template <class Key, class Value, class Allocator, class HashType>
typename HashMap<Key, Value, Allocator, HashType>::Iterator HashMap<Key, Value, Allocator, HashType>::Iterator::operator--(int) const
{
	const Iterator it = *this;
	backward();
	return it;
}

template <class Key, class Value, class Allocator, class HashType>
bool HashMap<Key, Value, Allocator, HashType>::Iterator::operator==(const Iterator& rhs) const
{
	// Redundant to check if rend/end is the same, but for thoroughness, checking anyways
	return _slot == rhs._slot && _rend == rhs._rend && _end == rhs._end;
}

template <class Key, class Value, class Allocator, class HashType>
bool HashMap<Key, Value, Allocator, HashType>::Iterator::operator!=(const Iterator& rhs) const
{
	return _slot != rhs._slot || _rend != rhs._rend || _end != rhs._end;
}

template <class Key, class Value, class Allocator, class HashType>
const typename HashMap<Key, Value, Allocator, HashType>::Iterator& HashMap<Key, Value, Allocator, HashType>::Iterator::operator=(const Iterator& rhs) const
{
	_slot = rhs._slot;
	_rend = rhs._rend;
	_end = rhs._end;
	return *this;
}

template <class Key, class Value, class Allocator, class HashType>
const Value& HashMap<Key, Value, Allocator, HashType>::Iterator::operator*(void) const
{
	return _slot->value;
}

template <class Key, class Value, class Allocator, class HashType>
Value& HashMap<Key, Value, Allocator, HashType>::Iterator::operator*(void)
{
	return _slot->value;
}

template <class Key, class Value, class Allocator, class HashType>
const Value* HashMap<Key, Value, Allocator, HashType>::Iterator::operator->(void) const
{
	return &_slot->value;
}

template <class Key, class Value, class Allocator, class HashType>
Value* HashMap<Key, Value, Allocator, HashType>::Iterator::operator->(void)
{
	return &_slot->value;
}

template <class Key, class Value, class Allocator, class HashType>
const Key& HashMap<Key, Value, Allocator, HashType>::Iterator::getKey(void) const
{
	return _slot->key;
}

template <class Key, class Value, class Allocator, class HashType>
const Value& HashMap<Key, Value, Allocator, HashType>::Iterator::getValue(void) const
{
	return _slot->value;
}

template <class Key, class Value, class Allocator, class HashType>
Value& HashMap<Key, Value, Allocator, HashType>::Iterator::getValue(void)
{
	return _slot->value;
}

template <class Key, class Value, class Allocator, class HashType>
HashMap<Key, Value, Allocator, HashType>::Iterator::Iterator(Slot* slot, Slot* end, Slot* rend):
	_slot(slot), _rend(rend), _end(end)
{
}


////////////////////
//    Hash Map    //
////////////////////
template <class Key, class Value, class Allocator, class HashType>
HashMap<Key, Value, Allocator, HashType>::HashMap(HashFunc hash, const Allocator& allocator):
	_allocator(allocator), _size(0), _used(0), _hash(hash), _slots(nullptr)
{
}

template <class Key, class Value, class Allocator, class HashType>
HashMap<Key, Value, Allocator, HashType>::HashMap(const Allocator& allocator):
	_allocator(allocator), _size(0), _used(0), _hash(nullptr), _slots(nullptr)
{
}

template <class Key, class Value, class Allocator, class HashType>
HashMap<Key, Value, Allocator, HashType>::HashMap(const HashMap<Key, Value, Allocator>& rhs):
	_allocator(rhs._allocator), _size(0), _used(0), _hash(rhs._hash), _slots(nullptr)
{
	*this = rhs;
}

template <class Key, class Value, class Allocator, class HashType>
HashMap<Key, Value, Allocator, HashType>::HashMap(HashMap<Key, Value, Allocator>&& rhs):
	_allocator(rhs._allocator), _used(rhs._used),
	_size(rhs._size), _hash(rhs._hash), _slots(rhs._slots)
{
	rhs._used = rhs._size = 0;
	rhs._slots = nullptr;
}

template <class Key, class Value, class Allocator, class HashType>
HashMap<Key, Value, Allocator, HashType>::~HashMap(void)
{
	clear();
}

template <class Key, class Value, class Allocator, class HashType>
const HashMap<Key, Value, Allocator>& HashMap<Key, Value, Allocator, HashType>::operator=(const HashMap<Key, Value, Allocator>& rhs)
{
	if (this == &rhs) {
		return *this;
	}

	clear();

	if (rhs._size == 0) {
		return *this;
	}

	if (rhs._size > 0) {
		_slots = GAFF_ALLOC_CAST(Slot*, sizeof(Slot) * rhs._size, _allocator);

		for (size_t i = 0; i < _used; ++i) {
			Construct(&_slots[i].key, rhs._slots[i].key);
			Construct(&_slots[i].value, rhs._slots[i].value);
			_slots[i].occupied, rhs._slots[i].occupied;
		}
	}

	_hash = rhs._hash;
	_used = rhs._used;
	_size = rhs._size;

	return *this;
}

template <class Key, class Value, class Allocator, class HashType>
const HashMap<Key, Value, Allocator>& HashMap<Key, Value, Allocator, HashType>::operator=(HashMap<Key, Value, Allocator>&& rhs)
{
	clear();

	_used = rhs._used;
	_size = rhs._size;
	_hash = rhs._hash;
	_slots = rhs._slots;

	rhs._used = rhs._size = 0;
	rhs._slots = nullptr;

	return *this;
}

template <class Key, class Value, class Allocator, class HashType>
bool HashMap<Key, Value, Allocator, HashType>::operator==(const HashMap<Key, Value, Allocator>& rhs) const
{
	if (_used != rhs._used) {
		return false;
	}

	for (size_t i = 0; i < _used; ++i) {
		if (_slots[i].occupied != rhs._slots[i].occupied ||
			_slots[i].key != rhs._slots[i].key ||
			_slots[i].value != rhs._slots[i].value) {

			return false;
		}
	}

	return true;
}

template <class Key, class Value, class Allocator, class HashType>
bool HashMap<Key, Value, Allocator, HashType>::operator!=(const HashMap<Key, Value, Allocator>& rhs) const
{
	return !(*this == rhs);
}

// Dangerous function! Data potentially doesn't exist!
template <class Key, class Value, class Allocator, class HashType>
const Value& HashMap<Key, Value, Allocator, HashType>::operator[](const Key& key) const
{
	GAFF_ASSERT(_size);

	//if (_size == 0) {
	//	return Value();
	//}

	size_t index = HashKey(key, _hash) % _size;
	size_t i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	GAFF_ASSERT(i != _size);
	return _slots[index].value;

	//return (i == _size) ? Value() : _slots[index].value;
}

template <class Key, class Value, class Allocator, class HashType>
Value& HashMap<Key, Value, Allocator, HashType>::operator[](const Key& key)
{
	if (_size == 0) {
		reserve(1);
	}

	size_t index = HashKey(key, _hash) % _size;
	size_t i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied) {
		Construct(&_slots[index].key, key);
		Construct(&_slots[index].value);
		_slots[index].occupied = true;
		++_used;
	}

	if (i == _size) {
		reserve(_size * 2);

		// try again
		return operator[](key);
	}

	return _slots[index].value;
}

template <class Key, class Value, class Allocator, class HashType>
Value& HashMap<Key, Value, Allocator, HashType>::operator[](Key&& key)
{
	if (_size == 0) {
		reserve(1);
	}

	size_t index = HashKey(key, _hash) % _size;
	size_t i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied) {
		Construct(&_slots[index].key, std::move(key));
		Construct(&_slots[index].value);
		_slots[index].occupied = true;
		++_used;
	}

	if (i == _size) {
		reserve(_size * 2);

		// try again
		return operator[](std::move(key));
	}

	return _slots[index].value;
}

// dangerous functions
// slots are potentially unoccupied
template <class Key, class Value, class Allocator, class HashType>
const Value& HashMap<Key, Value, Allocator, HashType>::valueAt(size_t index) const
{
	GAFF_ASSERT(index < _size);
	return _slots[index].value;
}

template <class Key, class Value, class Allocator, class HashType>
Value& HashMap<Key, Value, Allocator, HashType>::valueAt(size_t index)
{
	GAFF_ASSERT(index < _size);
	return _slots[index].value;
}

template <class Key, class Value, class Allocator, class HashType>
const Key& HashMap<Key, Value, Allocator, HashType>::keyAt(size_t index) const
{
	GAFF_ASSERT(index < _size);
	return _slots[index].key;
}

template <class Key, class Value, class Allocator, class HashType>
bool HashMap<Key, Value, Allocator, HashType>::isOccupied(size_t index) const
{
	GAFF_ASSERT(index < _size);
	return _slots[index].occupied;
}

template <class Key, class Value, class Allocator, class HashType>
void HashMap<Key, Value, Allocator, HashType>::erase(const Iterator& it)
{
	GAFF_ASSERT(it._slot >= _slots && it._slot < _slots + _size);
	erase(static_cast<size_t>(it._slot - _slots));
}

template <class Key, class Value, class Allocator, class HashType>
void HashMap<Key, Value, Allocator, HashType>::erase(size_t index)
{
	GAFF_ASSERT(index < _size);
	Deconstruct(&_slots[index].key);
	Deconstruct(&_slots[index].value);
	_slots[index].occupied = false;
	--_used;

	rebuildMap();
}

template <class Key, class Value, class Allocator, class HashType>
void HashMap<Key, Value, Allocator, HashType>::erase(const Key& key)
{
	size_t index = indexOf(key);
	GAFF_ASSERT(index != SIZE_T_FAIL);
	erase(index);
}

template <class Key, class Value, class Allocator, class HashType>
void HashMap<Key, Value, Allocator, HashType>::insert(Key&& key, Value&& value)
{
	operator[](std::move(key)) = std::move(value);
}

template <class Key, class Value, class Allocator, class HashType>
void HashMap<Key, Value, Allocator, HashType>::insert(const Key& key, Value&& value)
{
	operator[](key) = std::move(value);
}

template <class Key, class Value, class Allocator, class HashType>
void HashMap<Key, Value, Allocator, HashType>::insert(const Key& key, const Value& value)
{
	operator[](key) = value;
}

template <class Key, class Value, class Allocator, class HashType>
bool HashMap<Key, Value, Allocator, HashType>::hasElementWithValue(const Value& value) const
{
	for (auto it = begin(); it != end(); ++it) {
		if (*it == value) {
			return true;
		}
	}

	return false;
}

template <class Key, class Value, class Allocator, class HashType>
bool HashMap<Key, Value, Allocator, HashType>::hasElementWithKey(const Key& key) const
{
	return indexOf(key) != SIZE_T_FAIL;
}

template <class Key, class Value, class Allocator, class HashType>
size_t HashMap<Key, Value, Allocator, HashType>::indexOf(const Key& key) const
{
	if (!_size) {
		return SIZE_T_FAIL;
	}

	size_t index = HashKey(key, _hash) % _size;
	size_t i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied || i == _size) {
		return SIZE_T_FAIL;
	}

	return index;
}

template <class Key, class Value, class Allocator, class HashType>
void HashMap<Key, Value, Allocator, HashType>::clear(void)
{
	if (_slots) {
		for (size_t i = 0; i < _size; ++i) {
			if (_slots[i].occupied) {
				Deconstruct(&_slots[i].key);
				Deconstruct(&_slots[i].value);
			}

			_slots[i].occupied = false;
		}

		GAFF_FREE(_slots, _allocator);
		_used = _size = 0;
		_slots = nullptr;
	}
}

template <class Key, class Value, class Allocator, class HashType>
void HashMap<Key, Value, Allocator, HashType>::reserve(size_t new_size)
{
	if (new_size <= _size) {
		return;
	}

	Slot* old_data = _slots;
	size_t old_size = _size;

	_slots = GAFF_ALLOC_CAST(Slot*, sizeof(Slot) * new_size, _allocator);
	_size = new_size;
	_used = 0;

	memset(_slots, 0, sizeof(Slot) * new_size);

	if (old_data) {
		for (size_t i = 0; i < old_size; ++i) {
			if (old_data[i].occupied) {
				insert(std::move(old_data[i].key), std::move(old_data[i].value));

				// If move semantics are not supported, then we still need to call destructor
				Deconstruct(&old_data[i].key);
				Deconstruct(&old_data[i].value);
			}
		}

		GAFF_FREE(old_data, _allocator);
	}
}

template <class Key, class Value, class Allocator, class HashType>
size_t HashMap<Key, Value, Allocator, HashType>::capacity(void) const
{
	return _size;
}

template <class Key, class Value, class Allocator, class HashType>
size_t HashMap<Key, Value, Allocator, HashType>::size(void) const
{
	return _used;
}

template <class Key, class Value, class Allocator, class HashType>
bool HashMap<Key, Value, Allocator, HashType>::empty(void) const
{
	return _used == 0;
}

template <class Key, class Value, class Allocator, class HashType>
void HashMap<Key, Value, Allocator, HashType>::setAllocator(const Allocator& allocator)
{
	_allocator = allocator;
}

template <class Key, class Value, class Allocator, class HashType>
typename HashMap<Key, Value, Allocator, HashType>::Iterator HashMap<Key, Value, Allocator, HashType>::findElementWithValue(const Value& value) const
{
	auto it = begin();

	for (; it != end(); ++it) {
		if (it.getValue() == value) {
			break;
		}
	}

	return it;
}

template <class Key, class Value, class Allocator, class HashType>
typename HashMap<Key, Value, Allocator, HashType>::Iterator HashMap<Key, Value, Allocator, HashType>::findElementWithKey(const Key& key) const
{
	auto it = begin();

	for (; it != end(); ++it) {
		if (it.getKey() == key) {
			break;
		}
	}

	return it;
}

template <class Key, class Value, class Allocator, class HashType>
typename HashMap<Key, Value, Allocator, HashType>::Iterator HashMap<Key, Value, Allocator, HashType>::begin(void) const
{
	Iterator it(_slots, _slots + _size, _slots - 1);

	if (_slots && !_slots->occupied) {
		++it;
	}

	return it;
}

template <class Key, class Value, class Allocator, class HashType>
typename HashMap<Key, Value, Allocator, HashType>::Iterator HashMap<Key, Value, Allocator, HashType>::end(void) const
{
	return Iterator(_slots + _size, _slots + _size, _slots - 1);
}

template <class Key, class Value, class Allocator, class HashType>
typename HashMap<Key, Value, Allocator, HashType>::Iterator HashMap<Key, Value, Allocator, HashType>::rbegin(void) const
{
	Iterator it(_slots + _size - 1, _slots + _size, _slots - 1);

	if (((_slots + _size - 1)) && !(_slots + _size - 1)->occupied) {
		--it;
	}

	return it;
}

template <class Key, class Value, class Allocator, class HashType>
typename HashMap<Key, Value, Allocator, HashType>::Iterator HashMap<Key, Value, Allocator, HashType>::rend(void) const
{
	return Iterator(_slots - 1, _slots + _size, _slots - 1);
}

template <class Key, class Value, class Allocator, class HashType>
void HashMap<Key, Value, Allocator, HashType>::rebuildMap(void)
{
	HashMap<Key, Value, Allocator> new_map(_allocator);

	for (size_t i = 0; i < _size; ++i) {
		if (_slots[i].occupied) {
			new_map.insert(std::move(_slots[i].key), std::move(_slots[i].value));
			Deconstruct(&_slots[i].key);
			Deconstruct(&_slots[i].value);
			_slots[i].occupied = false;
		}
	}

	*this = std::move(new_map);
}
