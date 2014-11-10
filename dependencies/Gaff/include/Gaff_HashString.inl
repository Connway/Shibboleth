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

template <class T, class Allocator>
HashString<T, Allocator>::HashString(const HashString<T, Allocator>& string, HashFunc hash):
	_string(string._string), _hash_value(hash((const char*)string.getBuffer(), string.size() * sizeof(T))), _hash_func(hash)
{
}

template <class T, class Allocator>
HashString<T, Allocator>::HashString(const String<T, Allocator>& string, HashFunc hash):
	_string(string), _hash_value(hash((const char*)string.getBuffer(), string.size() * sizeof(T))), _hash_func(hash)
{
}

template <class T, class Allocator>
HashString<T, Allocator>::HashString(const T* string, HashFunc hash, const Allocator& allocator):
	_string(string, allocator), _hash_func(hash)
{
	_hash_value = hash((const char*)string, _string.size() * sizeof(T));
}

template <class T, class Allocator>
HashString<T, Allocator>::HashString(HashFunc hash, const Allocator& allocator):
	_string(allocator), _hash_value(0), _hash_func(hash)
{
}

template <class T, class Allocator>
HashString<T, Allocator>::HashString(HashString<T, Allocator>&& rhs):
	_string(Move(rhs._string)), _hash_value(rhs._hash_value), _hash_func(rhs._hash_func)
{
}

template <class T, class Allocator>
const HashString<T, Allocator>& HashString<T, Allocator>::operator=(const HashString<T, Allocator>& rhs)
{
	if (this == &rhs) {
		return *this;
	}

	_string = rhs._string;
	_hash_value = rhs._hash_value;
	_hash_func = rhs._hash_func;

	return *this;
}

template <class T, class Allocator>
const HashString<T, Allocator>& HashString<T, Allocator>::operator=(const String<T, Allocator>& rhs)
{
	_string = rhs;
	_hash_value = _hash_func((const char*)_string.getBuffer(), _string.size() * sizeof(T));
	return *this;
}

template <class T, class Allocator>
const HashString<T, Allocator>& HashString<T, Allocator>::operator=(HashString<T, Allocator>&& rhs)
{
	_string = Move(rhs._string);
	_hash_value = rhs._hash_value;
	_hash_func = rhs._hash_func;
	return *this;
}

template <class T, class Allocator>
const HashString<T, Allocator>& HashString<T, Allocator>::operator=(String<T, Allocator>&& rhs)
{
	_string = Move(rhs);
	_hash_value = _hash_func((const char*)_string.getBuffer(), _string.size() * sizeof(T));
	return *this;
}

template <class T, class Allocator>
const HashString<T, Allocator>& HashString<T, Allocator>::operator=(const T* rhs)
{
	_string = rhs;
	_hash_value = _hash_func((const char*)rhs, _string.size() * sizeof(T));
	return *this;
}

template <class T, class Allocator>
bool HashString<T, Allocator>::operator==(const HashString<T, Allocator>& rhs) const
{
	return _hash_value == rhs._hash_value;
}

template <class T, class Allocator>
bool HashString<T, Allocator>::operator!=(const HashString<T, Allocator>& rhs) const
{
	return _hash_value != rhs._hash_value;
}

template <class T, class Allocator>
char HashString<T, Allocator>::operator[](unsigned int index) const
{
	assert(index < _string.size());
	return _string[index];
}

// required reference for operations like string[i] = 'a';
//template <class T, class Allocator>
//char& HashString<T, Allocator>::operator[](unsigned int index)
//{
//	assert(index < _size);
//	return _string[index];
//}

template <class T, class Allocator>
const HashString<T, Allocator>& HashString<T, Allocator>::operator+=(const HashString<T, Allocator>& rhs)
{
	return (*this = _string + rhs._string);
}

template <class T, class Allocator>
const HashString<T, Allocator>& HashString<T, Allocator>::operator+=(const String<T, Allocator>& rhs)
{
	return (*this = _string + rhs);
}

template <class T, class Allocator>
const HashString<T, Allocator>& HashString<T, Allocator>::operator+=(const T* rhs)
{
	return (*this = _string + rhs);
}

template <class T, class Allocator>
HashString<T, Allocator> HashString<T, Allocator>::operator+(const HashString<T, Allocator>& rhs) const
{
	return HashString(_string + rhs._string, _hash_func);
}

template <class T, class Allocator>
HashString<T, Allocator> HashString<T, Allocator>::operator+(const String<T, Allocator>& rhs)
{
	return HashString(_string + rhs);
}

template <class T, class Allocator>
HashString<T, Allocator> HashString<T, Allocator>::operator+(const T* rhs) const
{
	return HashString(_string + rhs, _hash_func);
}

// WARNING: This function takes ownership of the string instead of copying
/*!
	\brief Sets the raw string to be used internally. Does not allocate to make a copy.
	\param string The raw string to use.
	\note Using this function means you are passing ownership of \a string to HashString.
*/
template <class T, class Allocator>
void HashString<T, Allocator>::set(T* string)
{
	_string.set(string);
	_hash_value = _hash_func((const char*)string, _string.size() * sizeof(T));
}

template <class T, class Allocator>
void HashString<T, Allocator>::clear(void)
{
	_string.clear();
	_hash_value = 0;
}

template <class T, class Allocator>
unsigned int HashString<T, Allocator>::size(void) const
{
	return _string.size();
}

template <class T, class Allocator>
const String<T, Allocator>& HashString<T, Allocator>::getString(void) const
{
	return _string;
}

template <class T, class Allocator>
const T* HashString<T, Allocator>::getBuffer(void) const
{
	return _string.getBuffer();
}

template <class T, class Allocator>
unsigned int HashString<T, Allocator>::getHash(void) const
{
	return _hash_value;
}
