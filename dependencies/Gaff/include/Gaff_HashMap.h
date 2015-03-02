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

/* \file */

// A hash map implementation using open addressing.
// Defaults to Fowler/Noll/Vo Hash

#pragma once

#include "Gaff_HashString.h"
#include "Gaff_Math.h"
#include <cstring>

NS_GAFF

// Normal
/*!
	\brief Open addressed, contiguous hash map. Has specialized implementations for String and HashString.

	\tparam Key The type used as the key.
	\tparam Value The type used as the value.
	\tparam Allocator The allocator used to allocate memory.
*/
template <class Key, class Value, class Allocator = DefaultAllocator>
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

	HashMap(HashFunc32 hash, const Allocator& allocator = Allocator());
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

	// dangerous functions
	// slots are potentially unoccupied
	const Value& valueAt(size_t index) const;
	Value& valueAt(size_t index);
	const Key& keyAt(size_t index) const;
	bool isOccupied(size_t index) const;

	void erase(const Iterator& it);
	void erase(size_t index);
	void erase(const Key& key);

	void moveMoveInsert(Key&& key, Value&& value);
	void moveInsert(const Key& key, Value&& value);
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

	HashFunc32 _hash;
	Slot* _slots;

	void rebuildMap(void);
};

#ifndef DOXY_SKIP
// String Specialization
template <class Value, class Allocator, class T>
class HashMap<String<T, Allocator>, Value, Allocator>
{
private:
	struct Slot
	{
		String<T, Allocator> key;
		Value value;
		size_t initial_index;
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

		const String<T, Allocator>& getKey(void) const;
		const Value& getValue(void) const;
		Value& getValue(void);

	private:
		Iterator(Slot* slot, Slot* end, Slot* rend);

		mutable Slot* _slot;
		mutable Slot* _rend;
		mutable Slot* _end;
		friend class HashMap<String<T, Allocator>, Value, Allocator>;
	};

	HashMap(HashFunc32 hash, const Allocator& allocator = Allocator());
	HashMap(const Allocator& allocator = Allocator());
	HashMap(const HashMap<String<T, Allocator>, Value, Allocator>& rhs);
	HashMap(HashMap<String<T, Allocator>, Value, Allocator>&& rhs);
	~HashMap(void);

	const HashMap<String<T, Allocator>, Value, Allocator>& operator=(const HashMap<String<T, Allocator>, Value, Allocator>& rhs);
	const HashMap<String<T, Allocator>, Value, Allocator>& operator=(HashMap<String<T, Allocator>, Value, Allocator>&& rhs);

	bool operator==(const HashMap<String<T, Allocator>, Value, Allocator>& rhs) const;
	bool operator!=(const HashMap<String<T, Allocator>, Value, Allocator>& rhs) const;

	// Dangerous function! Data potentially doesn't exist!
	const Value& operator[](const String<T, Allocator>& key) const;
	Value& operator[](const String<T, Allocator>& key);
	Value& operator[](String<T, Allocator>&& key);

	// dangerous functions
	// slots are potentially unoccupied
	const Value& valueAt(size_t index) const;
	Value& valueAt(size_t index);
	const String<T, Allocator>& keyAt(size_t index) const;

	bool isOccupied(size_t index) const;

	void erase(const Iterator& it);
	void erase(size_t index);
	void erase(const String<T, Allocator>& key);

	void moveMoveInsert(String<T, Allocator>&& key, Value&& value);
	void moveInsert(const String<T, Allocator>& key, Value&& value);
	void insert(const String<T, Allocator>& key, const Value& value);

	bool hasElementWithValue(const Value& value) const;
	bool hasElementWithKey(const String<T, Allocator>& key) const;
	size_t indexOf(const String<T, Allocator>& key) const;

	void clear(void);
	void reserve(size_t new_size);

	size_t capacity(void) const;
	size_t size(void) const;
	bool empty(void) const;

	void setAllocator(const Allocator& allocator);

	Iterator findElementWithValue(const Value& value) const;
	Iterator findElementWithKey(const String<T, Allocator>& value) const;

	Iterator begin(void) const;
	Iterator end(void) const;
	Iterator rbegin(void) const;
	Iterator rend(void) const;

private:
	Allocator _allocator;
	size_t _size;
	size_t _used;

	HashFunc32 _hash;
	Slot* _slots;

	void rebuildMap(void);
};

// HashString Specialization
template <class Value, class Allocator, class T>
class HashMap<HashString<T, Allocator>, Value, Allocator>
{
private:
	struct Slot
	{
		HashString<T, Allocator> key;
		Value value;
		size_t initial_index;
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

		const HashString<T, Allocator>& getKey(void) const;
		const Value& getValue(void) const;
		Value& getValue(void);

	private:
		Iterator(Slot* slot, Slot* end, Slot* rend);

		mutable Slot* _slot;
		mutable Slot* _rend;
		mutable Slot* _end;
		friend class HashMap<HashString<T, Allocator>, Value, Allocator>;
	};

	HashMap(HashFunc32 hash, const Allocator& allocator = Allocator());
	HashMap(const Allocator& allocator = Allocator());
	HashMap(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs);
	HashMap(HashMap<HashString<T, Allocator>, Value, Allocator>&& rhs);
	~HashMap(void);

	const HashMap<HashString<T, Allocator>, Value, Allocator>& operator=(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs);
	const HashMap<HashString<T, Allocator>, Value, Allocator>& operator=(HashMap<HashString<T, Allocator>, Value, Allocator>&& rhs);

	bool operator==(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs) const;
	bool operator!=(const HashMap<HashString<T, Allocator>, Value, Allocator>& rhs) const;

	// Dangerous function! Data potentially doesn't exist!
	const Value& operator[](const HashString<T, Allocator>& key) const;
	Value& operator[](const HashString<T, Allocator>& key);
	Value& operator[](HashString<T, Allocator>&& key);

	// dangerous functions
	// slots are potentially unoccupied
	const Value& valueAt(size_t index) const;
	Value& valueAt(size_t index);
	const HashString<T, Allocator>& keyAt(size_t index) const;

	bool isOccupied(size_t index) const;

	void erase(const Iterator& it);
	void erase(size_t index);
	void erase(const HashString<T, Allocator>& key);

	void moveMoveInsert(HashString<T, Allocator>&& key, Value&& value);
	void moveInsert(const HashString<T, Allocator>& key, Value&& value);
	void insert(const HashString<T, Allocator>& key, const Value& value);

	bool hasElementWithValue(const Value& value) const;
	bool hasElementWithKey(const HashString<T, Allocator>& key) const;
	size_t indexOf(const HashString<T, Allocator>& key) const;

	void clear(void);
	void reserve(size_t new_size);

	size_t capacity(void) const;
	size_t size(void) const;
	bool empty(void) const;

	void setAllocator(const Allocator& allocator);

	Iterator findElementWithValue(const Value& value) const;
	Iterator findElementWithKey(const HashString<T, Allocator>& value) const;

	Iterator begin(void) const;
	Iterator end(void) const;
	Iterator rbegin(void) const;
	Iterator rend(void) const;

private:
	Allocator _allocator;
	size_t _size;
	size_t _used;

	HashFunc32 _hash;
	Slot* _slots;

	void rebuildMap(void);
};
#endif

#include "Gaff_HashMap.inl"

NS_END
