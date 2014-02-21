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

#define LIST_ITERATOR List<T, Allocator>::Iterator

// Iterator
template <class T, class Allocator>
List<T, Allocator>::Iterator::Iterator(const typename LIST_ITERATOR& it):
	_element(it._element)
{
}

template <class T, class Allocator>
List<T, Allocator>::Iterator::Iterator(void):
	_element(nullptr)
{
}

template <class T, class Allocator>
const typename LIST_ITERATOR& List<T, Allocator>::Iterator::backward(void) const
{
	_element = _element->prev;
	return *this;
}

template <class T, class Allocator>
const typename LIST_ITERATOR& List<T, Allocator>::Iterator::forward(void) const
{
	_element = _element->next;
	return *this;
}

template <class T, class Allocator>
const T& List<T, Allocator>::Iterator::operator*(void) const
{
	return _element->data;
}

template <class T, class Allocator>
T& List<T, Allocator>::Iterator::operator*(void)
{
	return _element->data;
}

template <class T, class Allocator>
const T* List<T, Allocator>::Iterator::operator->(void) const
{
	return &_element->data;
}

template <class T, class Allocator>
T* List<T, Allocator>::Iterator::operator->(void)
{
	return &_element->data;
}

template <class T, class Allocator>
bool List<T, Allocator>::Iterator::operator==(const Iterator& rhs) const
{
	return _element == rhs._element;
}

template <class T, class Allocator>
bool List<T, Allocator>::Iterator::operator!=(const Iterator& rhs) const
{
	return _element != rhs._element;
}

template <class T, class Allocator>
const typename LIST_ITERATOR& List<T, Allocator>::Iterator::operator=(const Iterator& rhs) const
{
	_element = rhs._element;
	return *this;
}

template <class T, class Allocator>
const typename LIST_ITERATOR& List<T, Allocator>::Iterator::operator++(void) const
{
	return forward();
}

template <class T, class Allocator>
const typename LIST_ITERATOR& List<T, Allocator>::Iterator::operator++(int) const
{
	const Iterator it = *this;
	forward();
	return it;
}

template <class T, class Allocator>
const typename LIST_ITERATOR& List<T, Allocator>::Iterator::operator--(void) const
{
	backward();
	return *this;
}

template <class T, class Allocator>
const typename LIST_ITERATOR& List<T, Allocator>::Iterator::operator--(int) const
{
	const Iterator it = *this;
	backward();
	return it;
}

template <class T, class Allocator>
List<T, Allocator>::Iterator::Iterator(Node* element):
	_element(element)
{
}

// List
template <class T, class Allocator>
List<T, Allocator>::List(const Allocator& allocator):
	_allocator(allocator), _size(0), _head(0), _tail(0)
{
}

template <class T, class Allocator>
List<T, Allocator>::List(const List<T, Allocator>& rhs):
	_allocator(rhs._allocator), _size(0), _head(0), _tail(0)
{
	*this = rhs;
}

template <class T, class Allocator>
List<T, Allocator>::List(List<T, Allocator>&& rhs):
	_allocator(rhs._allocator), _size(rhs._size),
	_head(rhs._head), _tail(rhs._tail)
{
	rhs._head = rhs._tail = nullptr;
	rhs._size = 0;
}

template <class T, class Allocator>
List<T, Allocator>::~List(void)
{
	clear();
}

template <class T, class Allocator>
const List<T, Allocator>& List<T, Allocator>::operator=(const List<T, Allocator>& rhs)
{
	clear();

	Iterator beg = rhs.begin();
	Iterator end = rhs.end();

	while (beg != end) {
		pushBack(*beg);
		beg.forward();
	}

	return *this;
}

template <class T, class Allocator>
const List<T, Allocator>& List<T, Allocator>::operator=(List<T, Allocator>&& rhs)
{
	clear();

	_size = rhs._size;
	_head = rhs._head;
	_tail = rhs._tail;

	rhs._head = rhs._tail = nullptr;
	rhs._size = 0;

	return *this;
}

template <class T, class Allocator>
void List<T, Allocator>::clear(void)
{
	Node* node = _head;
	Node* next = (_head) ? _head->next : 0;
	while (node) {
		deconstruct(node);
		_allocator.free(node);

		node = next;

		if (node) {
			next = node->next;
		}
	}

	_head = _tail = nullptr;
	_size = 0;
}

template <class T, class Allocator>
unsigned int List<T, Allocator>::size(void) const
{
	return _size;
}

template <class T, class Allocator>
bool List<T, Allocator>::empty(void) const
{
	return _size == 0;
}

template <class T, class Allocator>
const T& List<T, Allocator>::first(void) const
{
	assert(_size > 0);
	return _head->data;
}

template <class T, class Allocator>
T& List<T, Allocator>::first(void)
{
	assert(_size > 0);
	return _head->data;
}

template <class T, class Allocator>
const T& List<T, Allocator>::last(void) const
{
	assert(_size > 0);
	return _tail->data;
}

template <class T, class Allocator>
T& List<T, Allocator>::last(void)
{
	assert(_size > 0);
	return _tail->data;
}

template <class T, class Allocator>
typename LIST_ITERATOR List<T, Allocator>::begin(void) const
{
	assert(_size > 0);
	return Iterator(_head);
}

template <class T, class Allocator>
typename LIST_ITERATOR List<T, Allocator>::end(void) const
{
	return Iterator();
}

template <class T, class Allocator>
typename LIST_ITERATOR List<T, Allocator>::rbegin(void) const
{
	assert(_size > 0);
	return Iterator(_tail);
}

template <class T, class Allocator>
typename LIST_ITERATOR List<T, Allocator>::rend(void) const
{
	return Iterator();
}

template <class T, class Allocator>
void List<T, Allocator>::pushBack(const T& val)
{
	Node* node = (Node*)_allocator.alloc(sizeof(Node));
	node->data = val;
	node->next = nullptr;
	node->prev = _tail;

	if (_tail) {
		_tail->next = node;
	} else {
		_head = node;
	}

	_tail = node;
	++_size;
}

template <class T, class Allocator>
void List<T, Allocator>::pushFront(const T& val)
{
	Node* node = (Node*)_allocator.alloc(sizeof(Node));
	node->data = val;
	node->next = _head;
	node->prev = nullptr;

	if (_head) {
		_head->prev = node;
	} else {
		_tail = node;
	}

	_head = node;
	++_size;
}

template <class T, class Allocator>
void List<T, Allocator>::popBack(void)
{
	assert(_size > 0);

	Node* node = _tail;
	_tail = _tail->prev;

	if (_tail) {
		_tail->next = nullptr;
	} else {
		_head = nullptr;
	}

	deconstruct(node);
	_allocator.free(node);
	--_size;
}

template <class T, class Allocator>
void List<T, Allocator>::popFront(void)
{
	assert(_size > 0);

	Node* node = _head;
	_head = _head->next;

	if (_head) {
		_head->prev = nullptr;
	} else {
		_tail = nullptr;
	}

	deconstruct(node);
	_allocator.free(node);
	--_size;
}

// A lot of if-checks when using this one, but this is inherintly not optimal anyways,
// so I think it can warrant the extra if-checks
template <class T, class Allocator>
void List<T, Allocator>::erase(unsigned int position)
{
	assert(_size > 0 && position < _size);

	if (position == _size - 1) {
		popBack();
	} else if (position == 0) {
		popFront();
	} else {
		Iterator it = begin();

		for (unsigned int i = 0; i < position; ++i) {
			it.forward();
		}

		erase(it);
	}
}

template <class T, class Allocator>
typename LIST_ITERATOR List<T, Allocator>::erase(const Iterator& it)
{
	assert(_size > 0 && it != Iterator());

	Iterator ret = Iterator();

	if (it._element == _head) {
		popFront();
		ret = Iterator(_head);

	} else if (it._element == _tail) {
		popBack();
		ret = Iterator(_tail);

	} else {
		ret = Iterator(it._element->next);

		it._element->prev->next = it._element->next;
		it._element->next->prev = it._element->prev;

		deconstruct(it._element);
		--_size;
	}

	return ret;
}

// A lot of if-checks when using this one, but this is inherintly not optimal anyways,
// so I think it can warrant the extra if-checks
template <class T, class Allocator>
void List<T, Allocator>::insert(unsigned int position, const T& val)
{
	assert(position <= _size);

	if (position == _size) {
		pushBack(val);
	} else if (position == 0) {
		pushFront(val);
	} else {
		Iterator it = begin();
		for (unsigned int i = 0; i < position - 1; ++i) {
			it.forward();
		}

		insert(it, val);
	}
}

template <class T, class Allocator>
void List<T, Allocator>::insert(const Iterator& it, const T& val)
{
	if (it._element == _head) {
		pushFront(val);
	} else if (it == Iterator()) {
		pushBack(val);
	} else {
		Node* node = (Node*)_allocator.alloc(sizeof(Node));
		node->data = val;
		node->next = it._element->next;
		node->prev = it._element;

		it._element->next = node;
		node->next->prev = node;
		++_size;
	}
}
