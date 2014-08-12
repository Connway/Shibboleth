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

#pragma once

#include "Gaff_Array.h"
#include "Gaff_Pair.h"

NS_GAFF

template <class Key, class Value, class Allocator = DefaultAllocator>
class Map
{
public:
	typedef typename Array<Pair<Key, Value>, Allocator>::Iterator Iterator;

	Map(const Allocator& allocator = Allocator());
	Map(const Map<Key, Value, Allocator>& rhs);
	Map(Map<Key, Value, Allocator>&& rhs);
	~Map(void);

	const Map<Key, Value, Allocator>& operator=(const Map<Key, Value, Allocator>& rhs);
	const Map<Key, Value, Allocator>& operator=(Map<Key, Value, Allocator>&& rhs);

	bool operator==(const Map<Key, Value, Allocator>& rhs) const;
	bool operator!=(const Map<Key, Value, Allocator>& rhs) const;

	// Dangerous function! Data potentially doesn't exist!
	const Value& operator[](const Key& key) const;
	Value& operator[](const Key& key);

	void erase(const Iterator& it);
	void erase(const Key& key);

	void moveMoveInsert(Key&& key, Value&& value);
	void moveInsert(const Key& key, Value&& value);
	void insert(const Key& key, const Value& value);

	void clear(void);
	void reserve(unsigned int new_size);

	unsigned int capacity(void) const;
	unsigned int size(void) const;
	bool empty(void) const;

	bool hasElementWithValue(const Value& value) const;
	bool hasElementWithKey(const Key& key) const;

	void setAllocator(const Allocator& allocator);

	Iterator begin(void) const;
	Iterator end(void) const;
	Iterator rbegin(void) const;
	Iterator rend(void) const;

	Iterator findElementWithValue(const Value& value) const;

private:
	class KeySearchPredicate
	{
	public:
		bool operator()(const Pair<Key, Value>& lhs, const Key& rhs) const
		{
			return lhs.first < rhs;
		}
	};

	class ValueSearchPredicate
	{
	public:
		bool operator()(const Pair<Key, Value>& lhs, const Value& rhs) const
		{
			return lhs.second == rhs;
		}
	};

	Array<Pair<Key, Value>, Allocator> _array;
	Allocator _allocator;
};

#include "Gaff_Map.inl"

NS_END
