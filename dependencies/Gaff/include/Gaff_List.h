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

#include "Gaff_DefaultAllocator.h"
#include "Gaff_IncludeAssert.h"

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class List
{
private:
	struct Node
	{
		Node* prev;
		Node* next;
		T data;
	};

public:
	class Iterator
	{
	public:
		Iterator(const Iterator& it);
		Iterator(void);

		const Iterator& backward(void) const;
		const Iterator& forward(void) const;

		const T& operator*(void) const;
		T& operator*(void);

		const T* operator->(void) const;
		T* operator->(void);

		bool operator==(const Iterator& rhs) const;
		bool operator!=(const Iterator& rhs) const;

		const Iterator& operator=(const Iterator& rhs) const;

		const Iterator& operator++(void) const;
		const Iterator& operator++(int) const;
		const Iterator& operator--(void) const;
		const Iterator& operator--(int) const;

	private:
		// Is this wise? I don't really want to make a separate iterator class for const.
		mutable Node* _element;

		Iterator(Node* element);
		friend class List<T, Allocator>;
	};

	List(const Allocator& allocator = Allocator());
	List(const List<T, Allocator>& rhs);
	List(List<T, Allocator>&& rhs);
	~List(void);

	const List<T, Allocator>& operator=(const List<T, Allocator>& rhs);
	const List<T, Allocator>& operator=(List<T, Allocator>&& rhs);

	void clear(void);

	unsigned int size(void) const;
	bool empty(void) const;

	const T& first(void) const;
	T& first(void);
	const T& last(void) const;
	T& last(void);

	Iterator begin(void) const;
	Iterator end(void) const;
	Iterator rbegin(void) const;
	Iterator rend(void) const;

	void pushBack(const T& val);
	void pushFront(const T& val);
	void popBack(void);
	void popFront(void);

	// A lot of if-checks when using this one, but this is inherintly not optimal anyways,
	// so I think it can warrant the extra if-checks
	void erase(unsigned int position);
	Iterator erase(const Iterator& it);

	// A lot of if-checks when using this one, but this is inherintly not optimal anyways,
	// so I think it can warrant the extra if-checks
	void insert(unsigned int position, const T& val);
	void insert(const Iterator& it, const T& val);

private:
	Allocator _allocator;
	unsigned int _size;
	Node* _head;
	Node* _tail;
};

#include "Gaff_List.inl"

NS_END
