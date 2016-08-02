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

// A hash map implementation using open addressing.
// Defaults to Fowler/Noll/Vo Hash

#pragma once

#include "Gaff_HashString.h"
#include <cstring>

NS_GAFF

template <class Key, class Value, class Allocator = DefaultAllocator, class HashType = ContainerHash>
class HashMap
{
private:
	struct Slot
	{
		Key key;
		Value value;
		bool occupied;
	};

public:
	class Iterator
	{
	public:
		Iterator(const Iterator& it);
		Iterator(void);

		const Iterator& backward(void) const;
		const Iterator& forward(void) const;

		const Iterator& operator++(void) const;
		const Iterator& operator--(void) const;
		Iterator operator++(int) const;
		Iterator operator--(int) const;

		bool operator==(const Iterator& rhs) const;
		bool operator!=(const Iterator& rhs) const;

		const Iterator& operator=(const Iterator& rhs) const;

		const Value& operator*(void) const;
		Value& operator*(void);

		const Value* operator->(void) const;
		Value* operator->(void);

		const Key& getKey(void) const;
		const Value& getValue(void) const;
		Value& getValue(void);

	private:
		Iterator(Slot* slot, Slot* end, Slot* rend);

		mutable Slot* _slot;
		mutable Slot* _rend;
		mutable Slot* _end;
		friend class HashMap<Key, Value, Allocator>;
	};

	using HashFunc = HashType (*)(const char*, size_t);

	HashMap(HashFunc hash, const Allocator& allocator = Allocator());
	HashMap(const Allocator& allocator = Allocator());
	HashMap(const HashMap<Key, Value, Allocator>& rhs);
	HashMap(HashMap<Key, Value, Allocator>&& rhs);
	~HashMap(void);

	const HashMap<Key, Value, Allocator>& operator=(const HashMap<Key, Value, Allocator>& rhs);
	const HashMap<Key, Value, Allocator>& operator=(HashMap<Key, Value, Allocator>&& rhs);

	bool operator==(const HashMap<Key, Value, Allocator>& rhs) const;
	bool operator!=(const HashMap<Key, Value, Allocator>& rhs) const;

	// Dangerous function! Data potentially doesn't exist!
	const Value& operator[](const Key& key) const;
	Value& operator[](const Key& key);
	Value& operator[](Key&& key);

	// Dangerous functions! Slots are potentially unoccupied
	const Value& valueAt(size_t index) const;
	Value& valueAt(size_t index);
	const Key& keyAt(size_t index) const;
	bool isOccupied(size_t index) const;

	void erase(const Iterator& it);
	void erase(size_t index);
	void erase(const Key& key);

	void insert(Key&& key, Value&& value);
	void insert(const Key& key, Value&& value);
	void insert(const Key& key, const Value& value);

	bool hasElementWithValue(const Value& value) const;
	bool hasElementWithKey(const Key& key) const;
	size_t indexOf(const Key& key) const;

	void clear(void);
	void reserve(size_t new_size);

	size_t capacity(void) const;
	size_t size(void) const;
	bool empty(void) const;

	void setAllocator(const Allocator& allocator);

	Iterator findElementWithValue(const Value& value) const;
	Iterator findElementWithKey(const Key& value) const;

	Iterator begin(void) const;
	Iterator end(void) const;
	Iterator rbegin(void) const;
	Iterator rend(void) const;

private:
	Allocator _allocator;
	size_t _size;
	size_t _used;

	HashFunc _hash;
	Slot* _slots;

	void rebuildMap(void);
};

#include "Gaff_HashMap.inl"

NS_END
