/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

// A hash map implementation using open addressing.
// Defaults to Fowler/Noll/Vo Hash

#pragma once

#include "Gaff_HashString.h"
#include "Gaff_Math.h"
#include <cstring>

NS_GAFF

template <class Key, class Value, class Allocator = DefaultAllocator>
class HashMap
{
public:
	HashMap(HashFunc hash = FNVHash, const Allocator& allocator = Allocator()) :
		_allocator(allocator), _size(0), _used(0), _hash(hash), _slots(nullptr)
	{
	}

	HashMap(const HashMap<Key, Value, Allocator>& rhs):
		_allocator(rhs._allocator), _size(0), _used(0), _hash(rhs._hash), _slots(nullptr)
	{
		*this = rhs;
	}

	~HashMap(void)
	{
		clear();
	}

	const HashMap<Key, Value, Allocator>& operator=(const HashMap<Key, Value, Allocator>& rhs)
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
			}
		}

		_used = rhs._used;
		_size = rhs._size;

		return *this;
	}

	bool operator==(const HashMap<Key, Value, Allocator>& rhs)
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

	bool operator!=(const HashMap<Key, Value, Allocator>& rhs)
	{
		return !(*this == rhs);
	}

	// Dangerous function! Data potentially doesn't exist!
	const Value& operator[](const Key& key) const
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

	Value& operator[](const Key& key)
	{
		if (_size == 0) {
			reserve(1);
		}

		unsigned int index = _hash((const char*)&key, sizeof(Key)) % _size;
		unsigned int i = 0;

		while (_slots[index].occupied && _slots[index].key != key && i < _size) {
			index = (index + 1) % _size;
			++i;
		}

		if (!_slots[index].occupied) {
			construct(&_slots[index].key, key);
			construct(&_slots[index].value);
			_slots[index].occupied = true;
		}

		if (i == _size) {
			reserve(_size * 2);

			// try again
			return operator[](key);
		}

		return _slots[index].value;
	}

	// dangerous functions
	// slots are potentially unoccupied
	const Value& valueAt(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].value;
	}

	Value& valueAt(unsigned int index)
	{
		assert(index < _size);
		return _slots[index].value;
	}

	const Key& keyAt(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].key;
	}

	bool isOccupied(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].occupied;
	}

	void erase(unsigned int index)
	{
		assert(index < _size);
		deconstruct(&_slots[index].key);
		deconstruct(&_slots[index].value);
		_slots[index].occupied = false;
	}

	void erase(const Key& key)
	{
		int index = indexOf(key);
		assert(index > -1);
		erase(index);
	}

	void insert(const Key& key, const Value& value)
	{
		operator[](key) = value;
	}

	int indexOf(const Key& key) const
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

	void clear(void)
	{
		if (_slots) {
			for (unsigned int i = 0; i < _used; ++i) {
				deconstruct(&_slots[i].key);
				deconstruct(&_slots[i].value);
			}

			_allocator.free(_slots);
			_used = _size = 0;
			_slots = nullptr;
		}
	}

	void reserve(unsigned int new_size)
	{
		if (new_size <= _size) {
			return;
		}

		Slot* old_data = _slots;
		unsigned int old_size = _size;

		_slots = (Slot*)_allocator.alloc(sizeof(Slot) * new_size);
		_size = new_size;

		memset(_slots, 0, sizeof(Slot) * new_size);

		if (old_data) {
			for (unsigned int i = 0; i < old_size; ++i) {
				if (old_data[i].occupied) {
					insert(old_data[i].key, old_data[i].value);
				}
			}

			_allocator.free(old_data);
		}
	}

	unsigned int capacity(void) const
	{
		return _size;
	}

	unsigned int size(void) const
	{
		return _used;
	}

	HashMap(HashMap<Key, Value, Allocator>&& rhs):
		_allocator(rhs._allocator), _used(rhs._used),
		_size(rhs._size), _hash(rhs._hash), _slots(rhs._slots)
	{
		rhs._used = rhs._size = 0;
		rhs._slots = nullptr;
	}

	const HashMap<Key, Value, Allocator>& operator=(HashMap<Key, Value, Allocator>&& rhs)
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

private:
	struct Slot
	{
		Key key;
		Value value;
		bool occupied;
	};

	Allocator _allocator;
	unsigned int _size;
	unsigned int _used;

	HashFunc _hash;
	Slot* _slots;
};

// String specialized classes
template <class Value, class Allocator>
class HashMap<AString<Allocator>, Value, Allocator>
{
public:
	HashMap(HashFunc hash = FNVHash, const Allocator& allocator = Allocator()) :
		_allocator(allocator), _size(0), _used(0), _hash(hash), _slots(nullptr)
	{
	}

	HashMap(const HashMap<AString<Allocator>, Value, Allocator>& rhs):
		_allocator(rhs._allocator), _size(0), _used(0), _hash(rhs._hash), _slots(nullptr)
	{
		*this = rhs;
	}

	~HashMap(void)
	{
		clear();
	}

	const HashMap<AString<Allocator>, Value, Allocator>& operator=(const HashMap<AString<Allocator>, Value, Allocator>& rhs)
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
			}
		}

		_used = rhs._used;
		_size = rhs._size;

		return *this;
	}

	bool operator==(const HashMap<AString<Allocator>, Value, Allocator>& rhs)
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

	bool operator!=(const HashMap<AString<Allocator>, Value, Allocator>& rhs)
	{
		return !(*this == rhs);
	}

	// Dangerous function! Data potentially doesn't exist!
	const Value& operator[](const AString<Allocator>& key) const
	{
		assert(_size);

		//if (_size == 0) {
		//	return Value();
		//}

		unsigned int index = _hash(key.getBuffer(), key.size() * sizeof(char)) % _size;
		unsigned int i = 0;

		while (_slots[index].occupied && _slots[index].key != key && i < _size) {
			index = (index + 1) % _size;
			++i;
		}

		assert(i != _size);
		return _slots[index].value;

		//return (i == _size) ? Value() : _slots[index].value;
	}

	Value& operator[](const AString<Allocator>& key)
	{
		if (_size == 0) {
			reserve(1);
		}

		unsigned int index = _hash(key.getBuffer(), key.size() * sizeof(char)) % _size;
		unsigned int i = 0;

		while (_slots[index].occupied && _slots[index].key != key && i < _size) {
			index = (index + 1) % _size;
			++i;
		}

		if (!_slots[index].occupied) {
			construct(&_slots[index].key, key);
			construct(&_slots[index].value);
			_slots[index].occupied = true;
		}

		if (i == _size) {
			reserve(_size * 2);

			// try again
			return operator[](key);
		}

		return _slots[index].value;
	}

	// dangerous functions
	// slots are potentially unoccupied
	const Value& valueAt(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].value;
	}

	Value& valueAt(unsigned int index)
	{
		assert(index < _size);
		return _slots[index].value;
	}

	const AString<Allocator>& keyAt(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].key;
	}

	bool isOccupied(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].occupied;
	}

	void erase(unsigned int index)
	{
		assert(index < _size);
		deconstruct(&_slots[index].key);
		deconstruct(&_slots[index].value);
		_slots[index].occupied = false;
	}

	void erase(const AString<Allocator>& key)
	{
		int index = indexOf(key);
		assert(index > -1);
		erase(index);
	}

	void insert(const AString<Allocator>& key, const Value& value)
	{
		operator[](key) = value;
	}

	int indexOf(const AString<Allocator>& key) const
	{
		if (!_size) {
			return -1;
		}

		unsigned int index = _hash(key.getBuffer(), key.size() * sizeof(char)) % _size;
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

	void clear(void)
	{
		if (_slots) {
			for (unsigned int i = 0; i < _used; ++i) {
				deconstruct(&_slots[i].key);
				deconstruct(&_slots[i].value);
			}

			_allocator.free(_slots);
			_used = _size = 0;
			_slots = nullptr;
		}
	}

	void reserve(unsigned int new_size)
	{
		if (new_size <= _size) {
			return;
		}

		Slot* old_data = _slots;
		unsigned int old_size = _size;

		_slots = (Slot*)_allocator.alloc(sizeof(Slot) * new_size);
		_size = new_size;

		memset(_slots, 0, sizeof(Slot) * new_size);

		if (old_data) {
			for (unsigned int i = 0; i < old_size; ++i) {
				if (old_data[i].occupied) {
					insert(old_data[i].key, old_data[i].value);
				}
			}

			_allocator.free(old_data);
		}
	}

	unsigned int capacity(void) const
	{
		return _size;
	}

	unsigned int size(void) const
	{
		return _used;
	}

	HashMap(HashMap<AString<Allocator>, Value, Allocator>&& rhs):
		_allocator(rhs._allocator), _used(rhs._used),
		_size(rhs._size), _hash(rhs._hash), _slots(rhs._slots)
	{
		rhs._used = rhs._size = 0;
		rhs._slots = nullptr;
	}

	const HashMap<AString<Allocator>, Value, Allocator>& operator=(HashMap<AString<Allocator>, Value, Allocator>&& rhs)
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

private:
	struct Slot
	{
		AString<Allocator> key;
		Value value;
		bool occupied;
	};

	Allocator _allocator;
	unsigned int _size;
	unsigned int _used;

	HashFunc _hash;
	Slot* _slots;
};

template <class Value, class Allocator>
class HashMap<WString<Allocator>, Value, Allocator>
{
public:
	HashMap(HashFunc hash = FNVHash, const Allocator& allocator = Allocator()) :
		_allocator(allocator), _size(0), _used(0), _hash(hash), _slots(nullptr)
	{
	}

	HashMap(const HashMap<WString<Allocator>, Value, Allocator>& rhs):
		_allocator(rhs._allocator), _size(0), _used(0), _hash(rhs._hash), _slots(nullptr)
	{
		*this = rhs;
	}

	~HashMap(void)
	{
		clear();
	}

	const HashMap<WString<Allocator>, Value, Allocator>& operator=(const HashMap<WString<Allocator>, Value, Allocator>& rhs)
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
			}
		}

		_used = rhs._used;
		_size = rhs._size;

		return *this;
	}

	bool operator==(const HashMap<WString<Allocator>, Value, Allocator>& rhs)
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

	bool operator!=(const HashMap<WString<Allocator>, Value, Allocator>& rhs)
	{
		return !(*this == rhs);
	}

	// Dangerous function! Data potentially doesn't exist!
	const Value& operator[](const WString<Allocator>& key) const
	{
		assert(_size);

		//if (_size == 0) {
		//	return Value();
		//}

		unsigned int index = _hash((const char*)key.getBuffer(), key.size() * sizeof(wchar_t)) % _size;
		unsigned int i = 0;

		while (_slots[index].occupied && _slots[index].key != key && i < _size) {
			index = (index + 1) % _size;
			++i;
		}

		assert(i != _size);
		return _slots[index].value;

		//return (i == _size) ? Value() : _slots[index].value;
	}

	Value& operator[](const WString<Allocator>& key)
	{
		if (_size == 0) {
			reserve(1);
		}

		unsigned int index = _hash((const char*)key.getBuffer(), key.size() * sizeof(wchar_t)) % _size;
		unsigned int i = 0;

		while (_slots[index].occupied && _slots[index].key != key && i < _size) {
			index = (index + 1) % _size;
			++i;
		}

		if (!_slots[index].occupied) {
			construct(&_slots[index].key, key);
			construct(&_slots[index].value);
			_slots[index].occupied = true;
		}

		if (i == _size) {
			reserve(_size * 2);

			// try again
			return operator[](key);
		}

		return _slots[index].value;
	}

	// dangerous functions
	// slots are potentially unoccupied
	const Value& valueAt(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].value;
	}

	Value& valueAt(unsigned int index)
	{
		assert(index < _size);
		return _slots[index].value;
	}

	const WString<Allocator>& keyAt(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].key;
	}

	bool isOccupied(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].occupied;
	}

	void erase(unsigned int index)
	{
		assert(index < _size);
		deconstruct(&_slots[index].key);
		deconstruct(&_slots[index].value);
		_slots[index].occupied = false;
	}

	void erase(const WString<Allocator>& key)
	{
		int index = indexOf(key);
		assert(index > -1);
		erase(index);
	}

	void insert(const WString<Allocator>& key, const Value& value)
	{
		operator[](key) = value;
	}

	int indexOf(const WString<Allocator>& key) const
	{
		if (!_size) {
			return -1;
		}

		unsigned int index = _hash((const char*)key.getBuffer(), key.size() * sizeof(wchar_t)) % _size;
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

	void clear(void)
	{
		if (_slots) {
			for (unsigned int i = 0; i < _used; ++i) {
				deconstruct(&_slots[i].key);
				deconstruct(&_slots[i].value);
			}

			_allocator.free(_slots);
			_used = _size = 0;
			_slots = nullptr;
		}
	}

	void reserve(unsigned int new_size)
	{
		if (new_size <= _size) {
			return;
		}

		Slot* old_data = _slots;
		unsigned int old_size = _size;

		_slots = (Slot*)_allocator.alloc(sizeof(Slot) * new_size);
		_size = new_size;

		memset(_slots, 0, sizeof(Slot) * new_size);

		if (old_data) {
			for (unsigned int i = 0; i < old_size; ++i) {
				if (old_data[i].occupied) {
					insert(old_data[i].key, old_data[i].value);
				}
			}

			_allocator.free(old_data);
		}
	}

	unsigned int capacity(void) const
	{
		return _size;
	}

	unsigned int size(void) const
	{
		return _used;
	}

	HashMap(HashMap<WString<Allocator>, Value, Allocator>&& rhs):
		_allocator(rhs._allocator), _used(rhs._used),
		_size(rhs._size), _hash(rhs._hash), _slots(rhs._slots)
	{
		rhs._used = rhs._size = 0;
		rhs._slots = nullptr;
	}

	const HashMap<WString<Allocator>, Value, Allocator>& operator=(HashMap<WString<Allocator>, Value, Allocator>&& rhs)
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

private:
	struct Slot
	{
		WString<Allocator> key;
		Value value;
		bool occupied;
	};

	Allocator _allocator;
	unsigned int _size;
	unsigned int _used;

	HashFunc _hash;
	Slot* _slots;
};


template <class Value, class Allocator, class T>
class HashMap<HashString<T, Allocator>, Value, Allocator>
{
public:
	HashMap(const Allocator& allocator = Allocator()) :
		_allocator(allocator), _size(0), _used(0), _slots(nullptr)
	{
	}

	HashMap(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs) :
		_allocator(rhs._allocator), _size(0), _used(0), _slots(nullptr)
	{
		*this = rhs;
	}

	~HashMap(void)
	{
		clear();
	}

	const HashMap<HashString<T, Allocator>, Value, Allocator>& operator=(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs)
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

		_used = rhs._used;
		_size = rhs._size;

		return *this;
	}

	bool operator==(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs)
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

	bool operator!=(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs)
	{
		return !(*this == rhs);
	}

	// Dangerous function! Data potentially doesn't exist!
	const Value& operator[](const HashString<T, Allocator>& key) const
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

	Value& operator[](const HashString<T, Allocator>& key)
	{
		if (_size == 0) {
			reserve(1);
		}

		unsigned int index = key.getHash() % _size;
		unsigned int i = 0;

		while (_slots[index].occupied && _slots[index].key != key && i < _size) {
			index = (index + 1) % _size;
			++i;
		}

		if (!_slots[index].occupied) {
			construct(&_slots[index].key, key);
			construct(&_slots[index].value);
			_slots[index].occupied = true;
		}

		if (i == _size) {
			reserve(_size * 2);

			// try again
			return operator[](key);
		}

		return _slots[index].value;
	}

	// dangerous functions
	// slots are potentially unoccupied
	const Value& valueAt(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].value;
	}

	Value& valueAt(unsigned int index)
	{
		assert(index < _size);
		return _slots[index].value;
	}

	const HashString<T, Allocator>& keyAt(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].key;
	}

	bool isOccupied(unsigned int index) const
	{
		assert(index < _size);
		return _slots[index].occupied;
	}

	void erase(unsigned int index)
	{
		assert(index < _size);
		deconstruct(&_slots[index].key);
		deconstruct(&_slots[index].value);
		_slots[index].occupied = false;
	}

	void erase(const HashString<T, Allocator>& key)
	{
		int index = indexOf(key);
		assert(index > -1);
		erase(index);
	}

	void insert(const HashString<T, Allocator>& key, const Value& value)
	{
		operator[](key) = value;
	}

	int indexOf(const HashString<T, Allocator>& key) const
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

	void clear(void)
	{
		if (_slots) {
			for (unsigned int i = 0; i < _used; ++i) {
				deconstruct(&_slots[i].key);
				deconstruct(&_slots[i].value);
			}

			_allocator.free(_slots);
			_used = _size = 0;
			_slots = nullptr;
		}
	}

	void reserve(unsigned int new_size)
	{
		if (new_size <= _size) {
			return;
		}

		Slot* old_data = _slots;
		unsigned int old_size = _size;

		_slots = (Slot*)_allocator.alloc(sizeof(Slot)* new_size);
		_size = new_size;

		memset(_slots, 0, sizeof(Slot)* new_size);

		if (old_data) {
			for (unsigned int i = 0; i < old_size; ++i) {
				if (old_data[i].occupied) {
					insert(old_data[i].key, old_data[i].value);
				}
			}

			_allocator.free(old_data);
		}
	}

	unsigned int capacity(void) const
	{
		return _size;
	}

	unsigned int size(void) const
	{
		return _used;
	}

	HashMap(HashMap<HashString<T, Allocator>, Value, Allocator>&& rhs) :
		_allocator(rhs._allocator), _used(rhs._used),
		_size(rhs._size), _slots(rhs._slots)
	{
		rhs._used = rhs._size = 0;
		rhs._slots = nullptr;
	}

	const HashMap<HashString<T, Allocator>, Value, Allocator>& operator=(HashMap<HashString<T, Allocator>, Value, Allocator>&& rhs)
	{
		clear();

		_used = rhs._used;
		_size = rhs._size;
		_slots = rhs._slots;

		rhs._used = rhs._size = 0;
		rhs._slots = nullptr;

		return *this;
	}

private:
	struct Slot
	{
		HashString<T, Allocator> key;
		Value value;
		bool occupied;
	};

	Allocator _allocator;
	unsigned int _size;
	unsigned int _used;

	Slot* _slots;
};
NS_END
