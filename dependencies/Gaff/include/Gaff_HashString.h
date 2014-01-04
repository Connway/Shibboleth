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

#pragma once

#include "Gaff_String.h"
#include "Gaff_Math.h"

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class HashString
{
public:
	HashString(const HashString<T, Allocator>& string, HashFunc hash = FNVHash):
		_string(string._string), _hash_value(hash((const char*)string.getBuffer(), string.size() * sizeof(T))), _hash_func(hash)
	{
	}

	HashString(const String<T, Allocator>& string, HashFunc hash = FNVHash):
		_string(string), _hash_value(hash((const char*)string.getBuffer(), string.size() * sizeof(T))), _hash_func(hash)
	{
	}

	HashString(const T* string, HashFunc hash = FNVHash, const Allocator& allocator = Allocator()):
		_string(string, allocator), _hash_func(hash)
	{
		_hash_value = hash((const char*)string, _string.size() * sizeof(T));
	}

	HashString(HashFunc hash = FNVHash, const Allocator& allocator = Allocator()) :
		_string(allocator), _hash_value(0), _hash_func(hash)
	{
	}

	const HashString<T, Allocator>& operator=(const HashString<T, Allocator>& rhs)
	{
		if (this == &rhs) {
			return *this;
		}

		_string = rhs._string;
		_hash_value = rhs._hash_value;
		_hash_func = rhs._hash_func;

		return *this;
	}

	const HashString<T, Allocator>& operator=(const String<T, Allocator>& rhs)
	{
		_string = rhs;
		_hash_value = _hash_func((const char*)_string.getBuffer(), _string.size() * sizeof(T));
		return *this;
	}

	const HashString<T, Allocator>& operator=(const T* rhs)
	{
		_string = rhs;
		_hash_value = _hash_func((const char*)rhs, _string.size() * sizeof(T));
		return *this;
	}

	bool operator==(const HashString<T, Allocator>& rhs) const
	{
		return _hash_value == rhs._hash_value;
	}

	bool operator!=(const HashString<T, Allocator>& rhs) const
	{
		return !(*this == rhs);
	}

	char operator[](unsigned int index) const
	{
		assert(index < _size);
		return _string[index];
	}

	// required reference for operations like string[i] = 'a';
	// char& operator[](unsigned int index)
	// {
	// 	assert(index < _size);
	// 	return _string[index];
	// }

	const HashString<T, Allocator>& operator+=(const HashString<T, Allocator>& rhs)
	{
		return (*this = _string + rhs._string);
	}

	const HashString<T, Allocator>& operator+=(const String<T, Allocator>& rhs)
	{
		return (*this = _string + rhs);
	}

	const HashString<T, Allocator>& operator+=(const T* rhs)
	{
		return (*this = _string + rhs);
	}

	HashString<T, Allocator> operator+(const HashString<T, Allocator>& rhs) const
	{
		return HashString(_string + rhs._string, _hash_func);
	}

	const HashString<T, Allocator>& operator+(const String<T, Allocator>& rhs)
	{
		return HashString(_string + rhs);
	}

	HashString<T, Allocator> operator+(const T* rhs) const
	{
		return HashString(_string + rhs, _hash_func);
	}

	// WARNING: This function takes ownership of the string instead of copying
	void set(T* string)
	{
		_string.set(string);
		_hash_value = _hash_func((const char*)string, _string.size() * sizeof(T));
	}

	void clear(void)
	{
		_string.clear();
		_hash_value = 0;
	}

	unsigned int size(void) const
	{
		return _string.size();
	}

	const T* getBuffer(void) const
	{
		return _string.getBuffer();
	}

	T* getBuffer(void)
	{
		return _string.getBuffer();
	}

	unsigned int getHash(void) const
	{
		return _hash_value;
	}

	HashString(HashString<T, Allocator>&& rhs):
		_string((String<T, Allocator>&&)rhs._string),
		_hash_value(rhs._hash_value), _hash_func(rhs._hash_func)
	{
	}

	const HashString<T, Allocator>& operator=(HashString<T, Allocator>&& rhs)
	{
		_string = (String<T, Allocator>&&)rhs._string;
		_hash_func = rhs._hash_func;
		return *this;
	}

private:
	String<T, Allocator> _string;
	unsigned int _hash_value;
	HashFunc _hash_func;
};

template <class Allocator = DefaultAllocator> using AHashString = HashString<char, Allocator>;
template <class Allocator = DefaultAllocator> using WHashString = HashString<wchar_t, Allocator>;
template <class Allocator = DefaultAllocator> using GHashString = HashString<GChar, Allocator>;

NS_END
