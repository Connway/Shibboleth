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

#include "Gaff_String.h"
#include "Gaff_Math.h"

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class HashString
{
public:
	HashString(const HashString<T, Allocator>& string, HashFunc hash = FNVHash);
	HashString(const String<T, Allocator>& string, HashFunc hash = FNVHash);
	HashString(const T* string, HashFunc hash = FNVHash, const Allocator& allocator = Allocator());
	HashString(HashFunc hash = FNVHash, const Allocator& allocator = Allocator());
	HashString(HashString<T, Allocator>&& rhs);

	const HashString<T, Allocator>& operator=(const HashString<T, Allocator>& rhs);
	const HashString<T, Allocator>& operator=(const String<T, Allocator>& rhs);
	const HashString<T, Allocator>& operator=(HashString<T, Allocator>&& rhs);
	const HashString<T, Allocator>& operator=(const T* rhs);

	bool operator==(const HashString<T, Allocator>& rhs) const;
	bool operator!=(const HashString<T, Allocator>& rhs) const;

	char operator[](unsigned int index) const;

	// required reference for operations like string[i] = 'a';
	// char& operator[](unsigned int index);

	const HashString<T, Allocator>& operator+=(const HashString<T, Allocator>& rhs);
	const HashString<T, Allocator>& operator+=(const String<T, Allocator>& rhs);
	const HashString<T, Allocator>& operator+=(const T* rhs);

	HashString<T, Allocator> operator+(const HashString<T, Allocator>& rhs) const;
	HashString<T, Allocator> operator+(const String<T, Allocator>& rhs);
	HashString<T, Allocator> operator+(const T* rhs) const;

	// WARNING: This function takes ownership of the string instead of copying
	void set(T* string);
	void clear(void);
	unsigned int size(void) const;

	const T* getBuffer(void) const;
	T* getBuffer(void);
	unsigned int getHash(void) const;

private:
	String<T, Allocator> _string;
	unsigned int _hash_value;
	HashFunc _hash_func;
};

#include "Gaff_HashString.inl"

template <class Allocator = DefaultAllocator> using AHashString = HashString<char, Allocator>;
template <class Allocator = DefaultAllocator> using WHashString = HashString<wchar_t, Allocator>;
template <class Allocator = DefaultAllocator> using GHashString = HashString<GChar, Allocator>;

NS_END
