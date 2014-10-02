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

#define HASHMAP_ITERATOR(TYPE) HashMap<TYPE, Value, Allocator>::Iterator

/////////////////////
//    Iterators    //
/////////////////////
template <class Key, class Value, class Allocator>
HashMap<Key, Value, Allocator>::Iterator::Iterator(const Iterator& it):
	_slot(it._slot), _rend(it._rend), _end(it._end)
{
}

template <class Key, class Value, class Allocator>
HashMap<Key, Value, Allocator>::Iterator::Iterator(void):
	_slot(nullptr), _rend(nullptr), _end(nullptr)
{
}

template <class Key, class Value, class Allocator>
const typename HashMap<Key, Value, Allocator>::Iterator& HashMap<Key, Value, Allocator>::Iterator::backward(void) const
{
	while (_slot != _rend) {
		--_slot;

		if (_slot->occupied) {
			break;
		}
	}

	return *this;
}

template <class Key, class Value, class Allocator>
const typename HashMap<Key, Value, Allocator>::Iterator& HashMap<Key, Value, Allocator>::Iterator::forward(void) const
{
	while (_slot != _end) {
		++_slot;

		if (_slot->occupied) {
			break;
		}
	}

	return *this;
}

template <class Key, class Value, class Allocator>
const typename HashMap<Key, Value, Allocator>::Iterator& HashMap<Key, Value, Allocator>::Iterator::operator++(void) const
{
	return forward();
}

template <class Key, class Value, class Allocator>
const typename HashMap<Key, Value, Allocator>::Iterator& HashMap<Key, Value, Allocator>::Iterator::operator--(void) const
{
	return backward();
}

template <class Key, class Value, class Allocator>
typename HashMap<Key, Value, Allocator>::Iterator HashMap<Key, Value, Allocator>::Iterator::operator++(int) const
{
	const Iterator it = *this;
	forward();
	return it;
}

template <class Key, class Value, class Allocator>
typename HashMap<Key, Value, Allocator>::Iterator HashMap<Key, Value, Allocator>::Iterator::operator--(int) const
{
	const Iterator it = *this;
	backward();
	return it;
}

template <class Key, class Value, class Allocator>
bool HashMap<Key, Value, Allocator>::Iterator::operator==(const Iterator& rhs) const
{
	// Redundant to check if rend/end is the same, but for thoroughness, checking anyways
	return _slot == rhs._slot && _rend == rhs._rend && _end == rhs._end;
}

template <class Key, class Value, class Allocator>
bool HashMap<Key, Value, Allocator>::Iterator::operator!=(const Iterator& rhs) const
{
	return _slot != rhs._slot || _rend != rhs._rend || _end != rhs._end;
}

template <class Key, class Value, class Allocator>
const typename HashMap<Key, Value, Allocator>::Iterator& HashMap<Key, Value, Allocator>::Iterator::operator=(const Iterator& rhs) const
{
	_slot = rhs._slot;
	_rend = rhs._rend;
	_end = rhs._end;
	return *this;
}

template <class Key, class Value, class Allocator>
const Value& HashMap<Key, Value, Allocator>::Iterator::operator*(void) const
{
	return _slot->value;
}

template <class Key, class Value, class Allocator>
Value& HashMap<Key, Value, Allocator>::Iterator::operator*(void)
{
	return _slot->value;
}

template <class Key, class Value, class Allocator>
const Value* HashMap<Key, Value, Allocator>::Iterator::operator->(void) const
{
	return &_slot->value;
}

template <class Key, class Value, class Allocator>
Value* HashMap<Key, Value, Allocator>::Iterator::operator->(void)
{
	return &_slot->value;
}

template <class Key, class Value, class Allocator>
const Key& HashMap<Key, Value, Allocator>::Iterator::getKey(void) const
{
	return _slot->key;
}

template <class Key, class Value, class Allocator>
const Value& HashMap<Key, Value, Allocator>::Iterator::getValue(void) const
{
	return _slot->value;
}

template <class Key, class Value, class Allocator>
Value& HashMap<Key, Value, Allocator>::Iterator::getValue(void)
{
	return _slot->value;
}

template <class Key, class Value, class Allocator>
HashMap<Key, Value, Allocator>::Iterator::Iterator(Slot* slot, Slot* end, Slot* rend):
	_slot(slot), _rend(rend), _end(end)
{
}



template <class Value, class Allocator, class T>
HashMap<String<T, Allocator>, Value, Allocator>::Iterator::Iterator(const Iterator& it):
	_slot(it._slot), _rend(it._rend), _end(it._end)
{
}

template <class Value, class Allocator, class T>
HashMap<String<T, Allocator>, Value, Allocator>::Iterator::Iterator(void):
	_slot(nullptr), _rend(nullptr), _end(nullptr)
{
}

template <class Value, class Allocator, class T>
const typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator& HashMap<String<T, Allocator>, Value, Allocator>::Iterator::backward(void) const
{
	while (_slot != _rend) {
		--_slot;

		if (_slot->occupied) {
			break;
		}
	}

	return *this;
}

template <class Value, class Allocator, class T>
const typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator& HashMap<String<T, Allocator>, Value, Allocator>::Iterator::forward(void) const
{
	while (_slot != _end) {
		++_slot;

		if (_slot->occupied) {
			break;
		}
	}

	return *this;
}

template <class Value, class Allocator, class T>
const typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator& HashMap<String<T, Allocator>, Value, Allocator>::Iterator::operator++(void) const
{
	return forward();
}

template <class Value, class Allocator, class T>
const typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator& HashMap<String<T, Allocator>, Value, Allocator>::Iterator::operator--(void) const
{
	return backward();
}

template <class Value, class Allocator, class T>
typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator HashMap<String<T, Allocator>, Value, Allocator>::Iterator::operator++(int) const
{
	const Iterator it = *this;
	forward();
	return it;
}

template <class Value, class Allocator, class T>
typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator HashMap<String<T, Allocator>, Value, Allocator>::Iterator::operator--(int) const
{
	const Iterator it = *this;
	backward();
	return it;
}

template <class Value, class Allocator, class T>
bool HashMap<String<T, Allocator>, Value, Allocator>::Iterator::operator==(const Iterator& rhs) const
{
	// Redundant to check if rend/end is the same, but for thoroughness, checking anyways
	return _slot == rhs._slot && _rend == rhs._rend && _end == rhs._end;
}

template <class Value, class Allocator, class T>
bool HashMap<String<T, Allocator>, Value, Allocator>::Iterator::operator!=(const Iterator& rhs) const
{
	return _slot != rhs._slot || _rend != rhs._rend || _end != rhs._end;
}

template <class Value, class Allocator, class T>
const typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator& HashMap<String<T, Allocator>, Value, Allocator>::Iterator::operator=(const Iterator& rhs) const
{
	_slot = rhs._slot;
	_rend = rhs._rend;
	_end = rhs._end;
	return *this;
}

template <class Value, class Allocator, class T>
const Value& HashMap<String<T, Allocator>, Value, Allocator>::Iterator::operator*(void) const
{
	return _slot->value;
}

template <class Value, class Allocator, class T>
Value& HashMap<String<T, Allocator>, Value, Allocator>::Iterator::operator*(void)
{
	return _slot->value;
}

template <class Value, class Allocator, class T>
const Value* HashMap<String<T, Allocator>, Value, Allocator>::Iterator::operator->(void) const
{
	return &_slot->value;
}

template <class Value, class Allocator, class T>
Value* HashMap<String<T, Allocator>, Value, Allocator>::Iterator::operator->(void)
{
	return &_slot->value;
}

template <class Value, class Allocator, class T>
const String<T, Allocator>& HashMap<String<T, Allocator>, Value, Allocator>::Iterator::getKey(void) const
{
	return _slot->key;
}

template <class Value, class Allocator, class T>
const Value& HashMap<String<T, Allocator>, Value, Allocator>::Iterator::getValue(void) const
{
	return _slot->value;
}

template <class Value, class Allocator, class T>
Value& HashMap<String<T, Allocator>, Value, Allocator>::Iterator::getValue(void)
{
	return _slot->value;
}

template <class Value, class Allocator, class T>
HashMap<String<T, Allocator>, Value, Allocator>::Iterator::Iterator(Slot* slot, Slot* end, Slot* rend):
	_slot(slot), _rend(rend), _end(end)
{
}



template <class Value, class Allocator, class T>
HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::Iterator(const Iterator& it):
	_slot(it._slot), _rend(it._rend), _end(it._end)
{
}

template <class Value, class Allocator, class T>
HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::Iterator(void):
	_slot(nullptr), _rend(nullptr), _end(nullptr)
{
}

template <class Value, class Allocator, class T>
const typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator& HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::backward(void) const
{
	while (_slot != _rend) {
		--_slot;

		if (_slot->occupied) {
			break;
		}
	}

	return *this;
}

template <class Value, class Allocator, class T>
const typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator& HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::forward(void) const
{
	while (_slot != _end) {
		++_slot;

		if (_slot->occupied) {
			break;
		}
	}

	return *this;
}

template <class Value, class Allocator, class T>
const typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator& HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::operator++(void) const
{
	return forward();
}

template <class Value, class Allocator, class T>
const typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator& HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::operator--(void) const
{
	return backward();
}

template <class Value, class Allocator, class T>
typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::operator++(int) const
{
	const Iterator it = *this;
	forward();
	return it;
}

template <class Value, class Allocator, class T>
typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::operator--(int) const
{
	const Iterator it = *this;
	backward();
	return it;
}

template <class Value, class Allocator, class T>
bool HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::operator==(const Iterator& rhs) const
{
	// Redundant to check if rend/end is the same, but for thoroughness, checking anyways
	return _slot == rhs._slot && _rend == rhs._rend && _end == rhs._end;
}

template <class Value, class Allocator, class T>
bool HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::operator!=(const Iterator& rhs) const
{
	return _slot != rhs._slot || _rend != rhs._rend || _end != rhs._end;
}

template <class Value, class Allocator, class T>
const typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator& HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::operator=(const Iterator& rhs) const
{
	_slot = rhs._slot;
	_rend = rhs._rend;
	_end = rhs._end;
	return *this;
}

template <class Value, class Allocator, class T>
const Value& HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::operator*(void) const
{
	return _slot->value;
}

template <class Value, class Allocator, class T>
Value& HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::operator*(void)
{
	return _slot->value;
}

template <class Value, class Allocator, class T>
const Value* HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::operator->(void) const
{
	return &_slot->value;
}

template <class Value, class Allocator, class T>
Value* HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::operator->(void)
{
	return &_slot->value;
}

template <class Value, class Allocator, class T>
const HashString<T, Allocator>& HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::getKey(void) const
{
	return _slot->key;
}

template <class Value, class Allocator, class T>
const Value& HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::getValue(void) const
{
	return _slot->value;
}

template <class Value, class Allocator, class T>
Value& HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::getValue(void)
{
	return _slot->value;
}

template <class Value, class Allocator, class T>
HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator::Iterator(Slot* slot, Slot* end, Slot* rend):
	_slot(slot), _rend(rend), _end(end)
{
}

// Normal
template <class Key, class Value, class Allocator>
HashMap<Key, Value, Allocator>::HashMap(HashFunc hash, const Allocator& allocator):
	_allocator(allocator), _size(0), _used(0), _hash(hash), _slots(nullptr)
{
}

template <class Key, class Value, class Allocator>
HashMap<Key, Value, Allocator>::HashMap(const HashMap<Key, Value, Allocator>& rhs):
	_allocator(rhs._allocator), _size(0), _used(0), _hash(rhs._hash), _slots(nullptr)
{
	*this = rhs;
}

template <class Key, class Value, class Allocator>
HashMap<Key, Value, Allocator>::HashMap(HashMap<Key, Value, Allocator>&& rhs):
	_allocator(rhs._allocator), _used(rhs._used),
	_size(rhs._size), _hash(rhs._hash), _slots(rhs._slots)
{
	rhs._used = rhs._size = 0;
	rhs._slots = nullptr;
}

template <class Key, class Value, class Allocator>
HashMap<Key, Value, Allocator>::~HashMap(void)
{
	clear();
}

template <class Key, class Value, class Allocator>
const HashMap<Key, Value, Allocator>& HashMap<Key, Value, Allocator>::operator=(const HashMap<Key, Value, Allocator>& rhs)
{
	if (this == &rhs) {
		return *this;
	}

	clear();

	if (rhs._size == 0) {
		return *this;
	}

	if (rhs._size > 0) {
		_slots = (Slot*)_allocator.alloc(sizeof(Slot) * rhs._size);

		for (unsigned int i = 0; i < _used; ++i) {
			construct(&_slots[i].key, rhs._slots[i].key);
			construct(&_slots[i].value, rhs._slots[i].value);
			construct(&_slots[i].occupied, rhs._slots[i].occupied);
			_slots[i].initial_hash = rhs._slots[i].initial_hash;
		}
	}

	_hash = rhs._hash;
	_used = rhs._used;
	_size = rhs._size;

	return *this;
}

template <class Key, class Value, class Allocator>
const HashMap<Key, Value, Allocator>& HashMap<Key, Value, Allocator>::operator=(HashMap<Key, Value, Allocator>&& rhs)
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

template <class Key, class Value, class Allocator>
bool HashMap<Key, Value, Allocator>::operator==(const HashMap<Key, Value, Allocator>& rhs) const
{
	if (_used != rhs._used) {
		return false;
	}

	for (unsigned int i = 0; i < _used; ++i) {
		if (_slots[i].occupied != rhs._slots[i].occupied ||
			_slots[i].key != rhs._slots[i].key ||
			_slots[i].value != rhs._slots[i].value) {

			return false;
		}
	}

	return true;
}

template <class Key, class Value, class Allocator>
bool HashMap<Key, Value, Allocator>::operator!=(const HashMap<Key, Value, Allocator>& rhs) const
{
	return !(*this == rhs);
}

// Dangerous function! Data potentially doesn't exist!
template <class Key, class Value, class Allocator>
const Value& HashMap<Key, Value, Allocator>::operator[](const Key& key) const
{
	assert(_size);

	//if (_size == 0) {
	//	return Value();
	//}

	unsigned int index = _hash((const char*)&key, sizeof(Key)) % _size;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	assert(i != _size);
	return _slots[index].value;

	//return (i == _size) ? Value() : _slots[index].value;
}

template <class Key, class Value, class Allocator>
Value& HashMap<Key, Value, Allocator>::operator[](const Key& key)
{
	if (_size == 0) {
		reserve(1);
	}

	unsigned int index = _hash((const char*)&key, sizeof(Key)) % _size;
	unsigned int initial_index = index;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied) {
		construct(&_slots[index].key, key);
		construct(&_slots[index].value);
		_slots[index].initial_index = initial_index;
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

template <class Key, class Value, class Allocator>
Value& HashMap<Key, Value, Allocator>::operator[](Key&& key)
{
	if (_size == 0) {
		reserve(1);
	}

	unsigned int index = _hash((const char*)&key, sizeof(Key)) % _size;
	unsigned int initial_index = index;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied) {
		moveConstruct(&_slots[index].key, Move(key));
		construct(&_slots[index].value);
		_slots[index].initial_index = initial_index;
		_slots[index].occupied = true;
		++_used;
	}

	if (i == _size) {
		reserve(_size * 2);

		// try again
		return operator[](Move(key));
	}

	return _slots[index].value;
}

// dangerous functions
// slots are potentially unoccupied
template <class Key, class Value, class Allocator>
const Value& HashMap<Key, Value, Allocator>::valueAt(unsigned int index) const
{
	assert(index < _size);
	return _slots[index].value;
}

template <class Key, class Value, class Allocator>
Value& HashMap<Key, Value, Allocator>::valueAt(unsigned int index)
{
	assert(index < _size);
	return _slots[index].value;
}

template <class Key, class Value, class Allocator>
const Key& HashMap<Key, Value, Allocator>::keyAt(unsigned int index) const
{
	assert(index < _size);
	return _slots[index].key;
}

template <class Key, class Value, class Allocator>
bool HashMap<Key, Value, Allocator>::isOccupied(unsigned int index) const
{
	assert(index < _size);
	return _slots[index].occupied;
}

template <class Key, class Value, class Allocator>
void HashMap<Key, Value, Allocator>::erase(const Iterator& it)
{
	assert(it._slot >= _slots && it._slot < _slots + _size);
	erase((unsigned int)(it._slot - _slots));
}

template <class Key, class Value, class Allocator>
void HashMap<Key, Value, Allocator>::erase(unsigned int index)
{
	assert(index < _size);
	deconstruct(&_slots[index].key);
	deconstruct(&_slots[index].value);
	_slots[index].occupied = false;

	shiftBuckets(index);
}

template <class Key, class Value, class Allocator>
void HashMap<Key, Value, Allocator>::erase(const Key& key)
{
	int index = indexOf(key);
	assert(index > -1);
	erase(index);
}

template <class Key, class Value, class Allocator>
void HashMap<Key, Value, Allocator>::moveMoveInsert(Key&& key, Value&& value)
{
	operator[](Move(key)) = Move(value);
}

template <class Key, class Value, class Allocator>
void HashMap<Key, Value, Allocator>::moveInsert(const Key& key, Value&& value)
{
	operator[](key) = Move(value);
}

template <class Key, class Value, class Allocator>
void HashMap<Key, Value, Allocator>::insert(const Key& key, const Value& value)
{
	operator[](key) = value;
}

template <class Key, class Value, class Allocator>
bool HashMap<Key, Value, Allocator>::hasElementWithValue(const Value& value) const
{
	for (auto it = begin(); it != end(); ++it) {
		if (*it == value) {
			return true;
		}
	}

	return false;
}

template <class Key, class Value, class Allocator>
bool HashMap<Key, Value, Allocator>::hasElementWithKey(const Key& key) const
{
	return indexOf(key) != -1;
}

template <class Key, class Value, class Allocator>
int HashMap<Key, Value, Allocator>::indexOf(const Key& key) const
{
	if (!_size) {
		return -1;
	}

	unsigned int index = _hash((const char*)&key, sizeof(Key)) % _size;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied || i == _size) {
		return -1;
	}

	return index;
}

template <class Key, class Value, class Allocator>
void HashMap<Key, Value, Allocator>::clear(void)
{
	if (_slots) {
		for (unsigned int i = 0; i < _size; ++i) {
			if (_slots[i].occupied) {
				deconstruct(&_slots[i].key);
				deconstruct(&_slots[i].value);
			}

			_slots[i].occupied = false;
		}

		_allocator.free(_slots);
		_used = _size = 0;
		_slots = nullptr;
	}
}

template <class Key, class Value, class Allocator>
void HashMap<Key, Value, Allocator>::reserve(unsigned int new_size)
{
	if (new_size <= _size) {
		return;
	}

	Slot* old_data = _slots;
	unsigned int old_size = _size;

	_slots = (Slot*)_allocator.alloc(sizeof(Slot) * new_size);
	_size = new_size;
	_used = 0;

	memset(_slots, 0, sizeof(Slot) * new_size);

	if (old_data) {
		for (unsigned int i = 0; i < old_size; ++i) {
			if (old_data[i].occupied) {
				moveMoveInsert(Move(old_data[i].key), Move(old_data[i].value));

				// If Move semantics are not supported, then we still need to call destructor
				deconstruct(&old_data[i].key);
				deconstruct(&old_data[i].value);
			}
		}

		_allocator.free(old_data);
	}
}

template <class Key, class Value, class Allocator>
unsigned int HashMap<Key, Value, Allocator>::capacity(void) const
{
	return _size;
}

template <class Key, class Value, class Allocator>
unsigned int HashMap<Key, Value, Allocator>::size(void) const
{
	return _used;
}

template <class Key, class Value, class Allocator>
bool HashMap<Key, Value, Allocator>::empty(void) const
{
	return _used == 0;
}

template <class Key, class Value, class Allocator>
void HashMap<Key, Value, Allocator>::setAllocator(const Allocator& allocator)
{
	_allocator = allocator;
}

template <class Key, class Value, class Allocator>
typename HashMap<Key, Value, Allocator>::Iterator HashMap<Key, Value, Allocator>::findElementWithValue(const Value& value) const
{
	auto it = begin();

	for (; it != end(); ++it) {
		if (it.getValue() == value) {
			break;
		}
	}

	return it;
}

template <class Key, class Value, class Allocator>
typename HashMap<Key, Value, Allocator>::Iterator HashMap<Key, Value, Allocator>::findElementWithKey(const Key& key) const
{
	auto it = begin();

	for (; it != end(); ++it) {
		if (it.getKey() == key) {
			break;
		}
	}

	return it;
}

template <class Key, class Value, class Allocator>
typename HashMap<Key, Value, Allocator>::Iterator HashMap<Key, Value, Allocator>::begin(void) const
{
	Iterator it(_slots, _slots + _size, _slots - 1);

	if (_slots && !_slots->occupied) {
		++it;
	}

	return it;
}

template <class Key, class Value, class Allocator>
typename HashMap<Key, Value, Allocator>::Iterator HashMap<Key, Value, Allocator>::end(void) const
{
	return Iterator(_slots + _size, _slots + _size, _slots - 1);
}

template <class Key, class Value, class Allocator>
typename HashMap<Key, Value, Allocator>::Iterator HashMap<Key, Value, Allocator>::rbegin(void) const
{
	Iterator it(_slots + _size - 1, _slots + _size, _slots - 1);

	if (((_slots + _size - 1)) && !(_slots + _size - 1)->occupied) {
		--it;
	}

	return it;
}

template <class Key, class Value, class Allocator>
typename HashMap<Key, Value, Allocator>::Iterator HashMap<Key, Value, Allocator>::rend(void) const
{
	return Iterator(_slots - 1, _slots + _size, _slots - 1);
}

template <class Key, class Value, class Allocator>
void HashMap<Key, Value, Allocator>::shiftBuckets(unsigned int index)
{
	unsigned int initial_index = _slots[index].initial_index;
	unsigned int prev_index = index;
	index = (index + 1) % _size;

	// If the slot is occupied and its initial index is less than its current index,
	// then shift it down one.
	while (_slots[index].occupied && _slots[index].initial_index < index) {
		_slots[prev_index].key = Gaff::Move(_slots[index].key);
		_slots[prev_index].value = Gaff::Move(_slots[index].value);
		_slots[prev_index].occupied = true;
		_slots[index].occupied = false;

		prev_index = index;
		index = (index + 1) % _size;
	}
	
	// We have looped to the beginning of the array
	if (index == 0) {
		// If the slot is occupied and its initial index is greater than its current index,
		// then it was looped around because it had to insert past the end of the array.
		// Shift it down one.
		while (_slots[index].occupied && _slots[index].initial_index > index) {
			_slots[prev_index].key = Gaff::Move(_slots[index].key);
			_slots[prev_index].value = Gaff::Move(_slots[index].value);
			_slots[prev_index].occupied = true;
			_slots[index].occupied = false;

			prev_index = index;
			index = (index + 1) % _size;
		}
	}
}


// String Specialization
template <class Value, class Allocator, class T>
HashMap<String<T, Allocator>, Value, Allocator>::HashMap(HashFunc hash, const Allocator& allocator):
	_allocator(allocator), _size(0), _used(0), _hash(hash), _slots(nullptr)
{
}

template <class Value, class Allocator, class T>
HashMap<String<T, Allocator>, Value, Allocator>::HashMap(const HashMap<String<T, Allocator>, Value, Allocator>& rhs):
	_allocator(rhs._allocator), _size(0), _used(0), _hash(rhs._hash), _slots(nullptr)
{
	*this = rhs;
}

template <class Value, class Allocator, class T>
HashMap<String<T, Allocator>, Value, Allocator>::HashMap(HashMap<String<T, Allocator>, Value, Allocator>&& rhs):
	_allocator(rhs._allocator), _used(rhs._used),
	_size(rhs._size), _hash(rhs._hash), _slots(rhs._slots)
{
	rhs._used = rhs._size = 0;
	rhs._slots = nullptr;
}

template <class Value, class Allocator, class T>
HashMap<String<T, Allocator>, Value, Allocator>::~HashMap(void)
{
	clear();
}

template <class Value, class Allocator, class T>
const HashMap<String<T, Allocator>, Value, Allocator>& HashMap<String<T, Allocator>, Value, Allocator>::operator=(const HashMap<String<T, Allocator>, Value, Allocator>& rhs)
{
	if (this == &rhs) {
		return *this;
	}

	clear();

	if (rhs._size == 0) {
		return *this;
	}

	if (rhs._size > 0) {
		_slots = (Slot*)_allocator.alloc(sizeof(Slot) * rhs._size);

		for (unsigned int i = 0; i < _used; ++i) {
			construct(&_slots[i].key, rhs._slots[i].key);
			construct(&_slots[i].value, rhs._slots[i].value);
			construct(&_slots[i].occupied, rhs._slots[i].occupied);
			_slots[i].initial_hash = rhs._slots[i].initial_hash;
		}
	}

	_hash = rhs._hash;
	_used = rhs._used;
	_size = rhs._size;

	return *this;
}

template <class Value, class Allocator, class T>
const HashMap<String<T, Allocator>, Value, Allocator>& HashMap<String<T, Allocator>, Value, Allocator>::operator=(HashMap<String<T, Allocator>, Value, Allocator>&& rhs)
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

template <class Value, class Allocator, class T>
bool HashMap<String<T, Allocator>, Value, Allocator>::operator==(const HashMap<String<T, Allocator>, Value, Allocator>& rhs) const
{
	if (_used != rhs._used) {
		return false;
	}

	for (unsigned int i = 0; i < _used; ++i) {
		if (_slots[i].occupied != rhs._slots[i].occupied ||
			_slots[i].key != rhs._slots[i].key ||
			_slots[i].value != rhs._slots[i].value) {

			return false;
		}
	}

	return true;
}

template <class Value, class Allocator, class T>
bool HashMap<String<T, Allocator>, Value, Allocator>::operator!=(const HashMap<String<T, Allocator>, Value, Allocator>& rhs) const
{
	return !(*this == rhs);
}

// Dangerous function! Data potentially doesn't exist!
template <class Value, class Allocator, class T>
const Value& HashMap<String<T, Allocator>, Value, Allocator>::operator[](const String<T, Allocator>& key) const
{
	assert(_size);

	//if (_size == 0) {
	//	return Value();
	//}

	unsigned int index = _hash((const char*)key.getBuffer(), key.size() * sizeof(T)) % _size;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	assert(i != _size);
	return _slots[index].value;

	//return (i == _size) ? Value() : _slots[index].value;
}

template <class Value, class Allocator, class T>
Value& HashMap<String<T, Allocator>, Value, Allocator>::operator[](const String<T, Allocator>& key)
{
	if (_size == 0) {
		reserve(1);
	}

	unsigned int index = _hash((const char*)key.getBuffer(), key.size() * sizeof(T)) % _size;
	unsigned int initial_index = index;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied) {
		construct(&_slots[index].key, key);
		construct(&_slots[index].value);
		_slots[index].initial_index = initial_index;
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

template <class Value, class Allocator, class T>
Value& HashMap<String<T, Allocator>, Value, Allocator>::operator[](String<T, Allocator>&& key)
{
	if (_size == 0) {
		reserve(1);
	}

	unsigned int index = _hash((const char*)key.getBuffer(), key.size() * sizeof(T)) % _size;
	unsigned int initial_index = index;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied) {
		moveConstruct(&_slots[index].key, Move(key));
		construct(&_slots[index].value);
		_slots[index].initial_index = initial_index;
		_slots[index].occupied = true;
		++_used;
	}

	if (i == _size) {
		reserve(_size * 2);

		// try again
		return operator[](Move(key));
	}

	return _slots[index].value;
}

// dangerous functions
// slots are potentially unoccupied
template <class Value, class Allocator, class T>
const Value& HashMap<String<T, Allocator>, Value, Allocator>::valueAt(unsigned int index) const
{
	assert(index < _size);
	return _slots[index].value;
}

template <class Value, class Allocator, class T>
Value& HashMap<String<T, Allocator>, Value, Allocator>::valueAt(unsigned int index)
{
	assert(index < _size);
	return _slots[index].value;
}

template <class Value, class Allocator, class T>
const String<T, Allocator>& HashMap<String<T, Allocator>, Value, Allocator>::keyAt(unsigned int index) const
{
	assert(index < _size);
	return _slots[index].key;
}

template <class Value, class Allocator, class T>
bool HashMap<String<T, Allocator>, Value, Allocator>::isOccupied(unsigned int index) const
{
	assert(index < _size);
	return _slots[index].occupied;
}

template <class Value, class Allocator, class T>
void HashMap<String<T, Allocator>, Value, Allocator>::erase(const Iterator& it)
{
	assert(it._slot >= _slots && it._slot < _slots + _size);
	erase((unsigned int)(it._slot - _slots));
}

template <class Value, class Allocator, class T>
void HashMap<String<T, Allocator>, Value, Allocator>::erase(unsigned int index)
{
	assert(index < _size);
	deconstruct(&_slots[index].key);
	deconstruct(&_slots[index].value);
	_slots[index].occupied = false;

	shiftBuckets(index);
}

template <class Value, class Allocator, class T>
void HashMap<String<T, Allocator>, Value, Allocator>::erase(const String<T, Allocator>& key)
{
	int index = indexOf(key);
	assert(index > -1);
	erase(index);
}

template <class Value, class Allocator, class T>
void HashMap<String<T, Allocator>, Value, Allocator>::moveMoveInsert(String<T, Allocator>&& key, Value&& value)
{
	operator[](Move(key)) = Move(value);
}

template <class Value, class Allocator, class T>
void HashMap<String<T, Allocator>, Value, Allocator>::moveInsert(const String<T, Allocator>& key, Value&& value)
{
	operator[](key) = Move(value);
}

template <class Value, class Allocator, class T>
void HashMap<String<T, Allocator>, Value, Allocator>::insert(const String<T, Allocator>& key, const Value& value)
{
	operator[](key) = value;
}

template <class Value, class Allocator, class T>
bool HashMap<String<T, Allocator>, Value, Allocator>::hasElementWithValue(const Value& value) const
{
	for (auto it = begin(); it != end(); ++it) {
		if (*it == value) {
			return true;
		}
	}

	return false;
}

template <class Value, class Allocator, class T>
bool HashMap<String<T, Allocator>, Value, Allocator>::hasElementWithKey(const String<T, Allocator>& key) const
{
	return indexOf(key) != -1;
}

template <class Value, class Allocator, class T>
int HashMap<String<T, Allocator>, Value, Allocator>::indexOf(const String<T, Allocator>& key) const
{
	if (!_size) {
		return -1;
	}

	unsigned int index = _hash((const char*)key.getBuffer(), key.size() * sizeof(T)) % _size;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied || i == _size) {
		return -1;
	}

	return index;
}

template <class Value, class Allocator, class T>
void HashMap<String<T, Allocator>, Value, Allocator>::clear(void)
{
	if (_slots) {
		for (unsigned int i = 0; i < _size; ++i) {
			if (_slots[i].occupied) {
				deconstruct(&_slots[i].key);
				deconstruct(&_slots[i].value);
			}

			_slots[i].occupied = false;
		}

		_allocator.free(_slots);
		_used = _size = 0;
		_slots = nullptr;
	}
}

template <class Value, class Allocator, class T>
void HashMap<String<T, Allocator>, Value, Allocator>::reserve(unsigned int new_size)
{
	if (new_size <= _size) {
		return;
	}

	Slot* old_data = _slots;
	unsigned int old_size = _size;

	_slots = (Slot*)_allocator.alloc(sizeof(Slot) * new_size);
	_size = new_size;
	_used = 0;

	memset(_slots, 0, sizeof(Slot) * new_size);

	if (old_data) {
		for (unsigned int i = 0; i < old_size; ++i) {
			if (old_data[i].occupied) {
				moveMoveInsert(Move(old_data[i].key), Move(old_data[i].value));

				// If Move semantics are not supported, then we still need to call destructor
				deconstruct(&old_data[i].key);
				deconstruct(&old_data[i].value);
			}
		}

		_allocator.free(old_data);
	}
}

template <class Value, class Allocator, class T>
unsigned int HashMap<String<T, Allocator>, Value, Allocator>::capacity(void) const
{
	return _size;
}

template <class Value, class Allocator, class T>
unsigned int HashMap<String<T, Allocator>, Value, Allocator>::size(void) const
{
	return _used;
}

template <class Value, class Allocator, class T>
bool HashMap<String<T, Allocator>, Value, Allocator>::empty(void) const
{
	return _used == 0;
}

template <class Value, class Allocator, class T>
void HashMap<String<T, Allocator>, Value, Allocator>::setAllocator(const Allocator& allocator)
{
	_allocator = allocator;
}

template <class Value, class Allocator, class T>
typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator HashMap<String<T, Allocator>, Value, Allocator>::findElementWithValue(const Value& value) const
{
	auto it = begin();

	for (; it != end(); ++it) {
		if (it.getValue() == value) {
			break;
		}
	}

	return it;
}

template <class Value, class Allocator, class T>
typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator HashMap<String<T, Allocator>, Value, Allocator>::findElementWithKey(const String<T, Allocator>& key) const
{
	auto it = begin();

	for (; it != end(); ++it) {
		if (it.getKey() == key) {
			break;
		}
	}

	return it;
}

template <class Value, class Allocator, class T>
typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator HashMap<String<T, Allocator>, Value, Allocator>::begin(void) const
{
	Iterator it(_slots, _slots + _size, _slots - 1);

	if (_slots && !_slots->occupied) {
		++it;
	}

	return it;
}

template <class Value, class Allocator, class T>
typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator HashMap<String<T, Allocator>, Value, Allocator>::end(void) const
{
	return Iterator(_slots + _size, _slots + _size, _slots - 1);
}

template <class Value, class Allocator, class T>
typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator HashMap<String<T, Allocator>, Value, Allocator>::rbegin(void) const
{
	Iterator it(_slots + _size - 1, _slots + _size, _slots - 1);

	if (((_slots + _size - 1)) && !(_slots + _size - 1)->occupied) {
		--it;
	}

	return it;
}

template <class Value, class Allocator, class T>
typename HashMap<String<T, Allocator>, Value, Allocator>::Iterator HashMap<String<T, Allocator>, Value, Allocator>::rend(void) const
{
	return Iterator(_slots - 1, _slots + _size, _slots - 1);
}

template <class Value, class Allocator, class T>
void HashMap<String<T, Allocator>, Value, Allocator>::shiftBuckets(unsigned int index)
{
	unsigned int initial_index = _slots[index].initial_index;
	unsigned int prev_index = index;
	index = (index + 1) % _size;

	// If the slot is occupied and its initial index is less than its current index,
	// then shift it down one.
	while (_slots[index].occupied && _slots[index].initial_index < index) {
		_slots[prev_index].key = Gaff::Move(_slots[index].key);
		_slots[prev_index].value = Gaff::Move(_slots[index].value);
		_slots[prev_index].occupied = true;
		_slots[index].occupied = false;

		prev_index = index;
		index = (index + 1) % _size;
	}

	// We have looped to the beginning of the array
	if (index == 0) {
		// If the slot is occupied and its initial index is greater than its current index,
		// then it was looped around because it had to insert past the end of the array.
		// Shift it down one.
		while (_slots[index].occupied && _slots[index].initial_index > index) {
			_slots[prev_index].key = Gaff::Move(_slots[index].key);
			_slots[prev_index].value = Gaff::Move(_slots[index].value);
			_slots[prev_index].occupied = true;
			_slots[index].occupied = false;

			prev_index = index;
			index = (index + 1) % _size;
		}
	}
}


// HashString Specialization
template <class Value, class Allocator, class T>
HashMap<HashString<T, Allocator>, Value, Allocator>::HashMap(const Allocator& allocator):
	_allocator(allocator), _size(0), _used(0), _slots(nullptr)
{
}

template <class Value, class Allocator, class T>
HashMap<HashString<T, Allocator>, Value, Allocator>::HashMap(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs):
	_allocator(rhs._allocator), _size(0), _used(0), _slots(nullptr)
{
	*this = rhs;
}

template <class Value, class Allocator, class T>
HashMap<HashString<T, Allocator>, Value, Allocator>::HashMap(HashMap<HashString<T, Allocator>, Value, Allocator>&& rhs):
	_allocator(rhs._allocator), _used(rhs._used),
	_size(rhs._size), _slots(rhs._slots)
{
	rhs._used = rhs._size = 0;
	rhs._slots = nullptr;
}

template <class Value, class Allocator, class T>
HashMap<HashString<T, Allocator>, Value, Allocator>::~HashMap(void)
{
	clear();
}

template <class Value, class Allocator, class T>
const HashMap<HashString<T, Allocator>, Value, Allocator>& HashMap<HashString<T, Allocator>, Value, Allocator>::operator=(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs)
{
	if (this == &rhs) {
		return *this;
	}

	clear();

	if (rhs._size == 0) {
		return *this;
	}

	if (rhs._size > 0) {
		_slots = (Slot*)_allocator.alloc(sizeof(Slot)* rhs._size);

		for (unsigned int i = 0; i < _used; ++i) {
			construct(&_slots[i].key, rhs._slots[i].key);
			construct(&_slots[i].value, rhs._slots[i].value);
			construct(&_slots[i].occupied, rhs._slots[i].occupied);
		}
	}

	_hash = rhs._hash;
	_used = rhs._used;
	_size = rhs._size;

	return *this;
}

template <class Value, class Allocator, class T>
const HashMap<HashString<T, Allocator>, Value, Allocator>& HashMap<HashString<T, Allocator>, Value, Allocator>::operator=(HashMap<HashString<T, Allocator>, Value, Allocator>&& rhs)
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

template <class Value, class Allocator, class T>
bool HashMap<HashString<T, Allocator>, Value, Allocator>::operator==(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs) const
{
	if (_used != rhs._used) {
		return false;
	}

	for (unsigned int i = 0; i < _used; ++i) {
		if (_slots[i].occupied != rhs._slots[i].occupied ||
			_slots[i].key != rhs._slots[i].key ||
			_slots[i].value != rhs._slots[i].value) {

			return false;
		}
	}

	return true;
}

template <class Value, class Allocator, class T>
bool HashMap<HashString<T, Allocator>, Value, Allocator>::operator!=(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs) const
{
	return !(*this == rhs);
}

// Dangerous function! Data potentially doesn't exist!
template <class Value, class Allocator, class T>
const Value& HashMap<HashString<T, Allocator>, Value, Allocator>::operator[](const HashString<T, Allocator>& key) const
{
	assert(_size);

	//if (_size == 0) {
	//	return Value();
	//}

	unsigned int index = key.getHash() % _size;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	assert(i != _size);
	return _slots[index].value;

	//return (i == _size) ? Value() : _slots[index].value;
}

template <class Value, class Allocator, class T>
Value& HashMap<HashString<T, Allocator>, Value, Allocator>::operator[](const HashString<T, Allocator>& key)
{
	if (_size == 0) {
		reserve(1);
	}

	unsigned int index = key.getHash() % _size;
	unsigned int initial_index = index;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied) {
		construct(&_slots[index].key, key);
		construct(&_slots[index].value);
		_slots[index].initial_index = initial_index;
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

template <class Value, class Allocator, class T>
Value& HashMap<HashString<T, Allocator>, Value, Allocator>::operator[](HashString<T, Allocator>&& key)
{
	if (_size == 0) {
		reserve(1);
	}

	unsigned int index = key.getHash() % _size;
	unsigned int initial_index = index;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied) {
		moveConstruct(&_slots[index].key, Move(key));
		construct(&_slots[index].value);
		_slots[index].initial_index = initial_index;
		_slots[index].occupied = true;
		++_used;
	}

	if (i == _size) {
		reserve(_size * 2);

		// try again
		return operator[](Move(key));
	}

	return _slots[index].value;
}

// dangerous functions
// slots are potentially unoccupied
template <class Value, class Allocator, class T>
const Value& HashMap<HashString<T, Allocator>, Value, Allocator>::valueAt(unsigned int index) const
{
	assert(index < _size);
	return _slots[index].value;
}

template <class Value, class Allocator, class T>
Value& HashMap<HashString<T, Allocator>, Value, Allocator>::valueAt(unsigned int index)
{
	assert(index < _size);
	return _slots[index].value;
}

template <class Value, class Allocator, class T>
const HashString<T, Allocator>& HashMap<HashString<T, Allocator>, Value, Allocator>::keyAt(unsigned int index) const
{
	assert(index < _size);
	return _slots[index].key;
}

template <class Value, class Allocator, class T>
bool HashMap<HashString<T, Allocator>, Value, Allocator>::isOccupied(unsigned int index) const
{
	assert(index < _size);
	return _slots[index].occupied;
}

template <class Value, class Allocator, class T>
void HashMap<HashString<T, Allocator>, Value, Allocator>::erase(const Iterator& it)
{
	assert(it._slot >= _slots && it._slot < _slots + _size);
	erase((unsigned int)(it._slot - _slots));
}

template <class Value, class Allocator, class T>
void HashMap<HashString<T, Allocator>, Value, Allocator>::erase(unsigned int index)
{
	assert(index < _size);
	deconstruct(&_slots[index].key);
	deconstruct(&_slots[index].value);
	_slots[index].occupied = false;

	shiftBuckets(index);
}

template <class Value, class Allocator, class T>
void HashMap<HashString<T, Allocator>, Value, Allocator>::erase(const HashString<T, Allocator>& key)
{
	int index = indexOf(key);
	assert(index > -1);
	erase(index);
}

template <class Value, class Allocator, class T>
void HashMap<HashString<T, Allocator>, Value, Allocator>::moveMoveInsert(HashString<T, Allocator>&& key, Value&& value)
{
	operator[](Move(key)) = Move(value);
}

template <class Value, class Allocator, class T>
void HashMap<HashString<T, Allocator>, Value, Allocator>::moveInsert(const HashString<T, Allocator>& key, Value&& value)
{
	operator[](key) = Move(value);
}

template <class Value, class Allocator, class T>
void HashMap<HashString<T, Allocator>, Value, Allocator>::insert(const HashString<T, Allocator>& key, const Value& value)
{
	operator[](key) = value;
}

template <class Value, class Allocator, class T>
bool HashMap<HashString<T, Allocator>, Value, Allocator>::hasElementWithValue(const Value& value) const
{
	for (auto it = begin(); it != end(); ++it) {
		if (*it == value) {
			return true;
		}
	}

	return false;
}

template <class Value, class Allocator, class T>
bool HashMap<HashString<T, Allocator>, Value, Allocator>::hasElementWithKey(const HashString<T, Allocator>& key) const
{
	return indexOf(key) != -1;
}

template <class Value, class Allocator, class T>
int HashMap<HashString<T, Allocator>, Value, Allocator>::indexOf(const HashString<T, Allocator>& key) const
{
	if (!_size) {
		return -1;
	}

	unsigned int index = key.getHash() % _size;
	unsigned int i = 0;

	while (_slots[index].occupied && _slots[index].key != key && i < _size) {
		index = (index + 1) % _size;
		++i;
	}

	if (!_slots[index].occupied || i == _size) {
		return -1;
	}

	return index;
}

template <class Value, class Allocator, class T>
void HashMap<HashString<T, Allocator>, Value, Allocator>::clear(void)
{
	if (_slots) {
		for (unsigned int i = 0; i < _size; ++i) {
			if (_slots[i].occupied) {
				deconstruct(&_slots[i].key);
				deconstruct(&_slots[i].value);
			}

			_slots[i].occupied = false;
		}

		_allocator.free(_slots);
		_used = _size = 0;
		_slots = nullptr;
	}
}

template <class Value, class Allocator, class T>
void HashMap<HashString<T, Allocator>, Value, Allocator>::reserve(unsigned int new_size)
{
	if (new_size <= _size) {
		return;
	}

	Slot* old_data = _slots;
	unsigned int old_size = _size;

	_slots = (Slot*)_allocator.alloc(sizeof(Slot) * new_size);
	_size = new_size;
	_used = 0;

	memset(_slots, 0, sizeof(Slot) * new_size);

	if (old_data) {
		for (unsigned int i = 0; i < old_size; ++i) {
			if (old_data[i].occupied) {
				moveMoveInsert(Move(old_data[i].key), Move(old_data[i].value));

				// If Move semantics are not supported, then we still need to call destructor
				deconstruct(&old_data[i].key);
				deconstruct(&old_data[i].value);
			}
		}

		_allocator.free(old_data);
	}
}

template <class Value, class Allocator, class T>
unsigned int HashMap<HashString<T, Allocator>, Value, Allocator>::capacity(void) const
{
	return _size;
}

template <class Value, class Allocator, class T>
unsigned int HashMap<HashString<T, Allocator>, Value, Allocator>::size(void) const
{
	return _used;
}

template <class Value, class Allocator, class T>
bool HashMap<HashString<T, Allocator>, Value, Allocator>::empty(void) const
{
	return _used == 0;
}

template <class Value, class Allocator, class T>
void HashMap<HashString<T, Allocator>, Value, Allocator>::setAllocator(const Allocator& allocator)
{
	_allocator = allocator;
}

template <class Value, class Allocator, class T>
typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator HashMap<HashString<T, Allocator>, Value, Allocator>::begin(void) const
{
	Iterator it(_slots, _slots + _size, _slots - 1);

	if (_slots && !_slots->occupied) {
		++it;
	}

	return it;
}

template <class Value, class Allocator, class T>
typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator HashMap<HashString<T, Allocator>, Value, Allocator>::findElementWithValue(const Value& value) const
{
	auto it = begin();

	for (; it != end(); ++it) {
		if (it.getValue() == value) {
			break;
		}
	}

	return it;
}

template <class Value, class Allocator, class T>
typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator HashMap<HashString<T, Allocator>, Value, Allocator>::findElementWithKey(const HashString<T, Allocator>& key) const
{
	auto it = begin();

	for (; it != end(); ++it) {
		if (it.getKey() == key) {
			break;
		}
	}

	return it;
}

template <class Value, class Allocator, class T>
typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator HashMap<HashString<T, Allocator>, Value, Allocator>::end(void) const
{
	return Iterator(_slots + _size, _slots + _size, _slots - 1);
}

template <class Value, class Allocator, class T>
typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator HashMap<HashString<T, Allocator>, Value, Allocator>::rbegin(void) const
{
	Iterator it(_slots + _size - 1, _slots + _size, _slots - 1);

	if (((_slots + _size - 1)) && !(_slots + _size - 1)->occupied) {
		--it;
	}

	return it;
}

template <class Value, class Allocator, class T>
typename HashMap<HashString<T, Allocator>, Value, Allocator>::Iterator HashMap<HashString<T, Allocator>, Value, Allocator>::rend(void) const
{
	return Iterator(_slots - 1, _slots + _size, _slots - 1);
}

template <class Value, class Allocator, class T>
void HashMap<HashString<T, Allocator>, Value, Allocator>::shiftBuckets(unsigned int index)
{
	unsigned int initial_index = _slots[index].initial_index;
	unsigned int prev_index = index;
	index = (index + 1) % _size;

	// If the slot is occupied and its initial index is less than its current index,
	// then shift it down one.
	while (_slots[index].occupied && _slots[index].initial_index < index) {
		_slots[prev_index].key = Gaff::Move(_slots[index].key);
		_slots[prev_index].value = Gaff::Move(_slots[index].value);
		_slots[prev_index].occupied = true;
		_slots[index].occupied = false;

		prev_index = index;
		index = (index + 1) % _size;
	}

	// We have looped to the beginning of the array
	if (index == 0) {
		// If the slot is occupied and its initial index is greater than its current index,
		// then it was looped around because it had to insert past the end of the array.
		// Shift it down one.
		while (_slots[index].occupied && _slots[index].initial_index > index) {
			_slots[prev_index].key = Gaff::Move(_slots[index].key);
			_slots[prev_index].value = Gaff::Move(_slots[index].value);
			_slots[prev_index].occupied = true;
			_slots[index].occupied = false;

			prev_index = index;
			index = (index + 1) % _size;
		}
	}
}
