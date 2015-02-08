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

/*! \file */

#pragma once

#include "Gaff_String.h"
#include "Gaff_Math.h"

NS_GAFF

/*!
	\brief Similar to String, but stores the hash result of the passed in string.
*/
template <class T, class Allocator = DefaultAllocator>
class HashString
{
public:
	HashString(const HashString<T, Allocator>& string, HashFunc32 hash = FNV1Hash32);
	HashString(const String<T, Allocator>& string, HashFunc32 hash = FNV1Hash32);
	HashString(const T* string, HashFunc32 hash = FNV1Hash32, const Allocator& allocator = Allocator());
	HashString(HashFunc32 hash = FNV1Hash32, const Allocator& allocator = Allocator());
	HashString(HashString<T, Allocator>&& rhs);

	const HashString<T, Allocator>& operator=(const HashString<T, Allocator>& rhs);
	const HashString<T, Allocator>& operator=(const String<T, Allocator>& rhs);
	const HashString<T, Allocator>& operator=(HashString<T, Allocator>&& rhs);
	const HashString<T, Allocator>& operator=(String<T, Allocator>&& rhs);
	const HashString<T, Allocator>& operator=(const T* rhs);

	bool operator==(const HashString<T, Allocator>& rhs) const;
	bool operator!=(const HashString<T, Allocator>& rhs) const;

	char operator[](size_t index) const;

	// required reference for operations like string[i] = 'a';
	// char& operator[](size_t index);

	const HashString<T, Allocator>& operator+=(const HashString<T, Allocator>& rhs);
	const HashString<T, Allocator>& operator+=(const String<T, Allocator>& rhs);
	const HashString<T, Allocator>& operator+=(const T* rhs);

	HashString<T, Allocator> operator+(const HashString<T, Allocator>& rhs) const;
	HashString<T, Allocator> operator+(const String<T, Allocator>& rhs);
	HashString<T, Allocator> operator+(const T* rhs) const;

	// WARNING: This function takes ownership of the string instead of copying
	void set(T* string);
	void clear(void);
	size_t size(void) const;

	const String<T, Allocator>& getString(void) const;
	const T* getBuffer(void) const;
	unsigned int getHash(void) const;

private:
	String<T, Allocator> _string;
	unsigned int _hash_value;
	HashFunc32 _hash_func;
};

#include "Gaff_HashString.inl"

template <class Allocator = DefaultAllocator> using AHashString = HashString<char, Allocator>;
template <class Allocator = DefaultAllocator> using WHashString = HashString<wchar_t, Allocator>;
template <class Allocator = DefaultAllocator> using GHashString = HashString<GChar, Allocator>;

NS_END
