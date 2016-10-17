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

#pragma once

#include "Gaff_String.h"
#include "Gaff_Math.h"

NS_GAFF

template <class T, class Allocator = DefaultAllocator, class HashType = uint32_t>
class HashString
{
public:
	using HashFunc = HashType (*)(const char*, size_t);

	HashString(const HashString<T, Allocator, HashType>& string, HashFunc hash = nullptr);
	HashString(const String<T, Allocator>& string, HashFunc hash = nullptr);
	HashString(const T* string, HashFunc hash = nullptr, const Allocator& allocator = Allocator());
	HashString(HashFunc hash = nullptr, const Allocator& allocator = Allocator());
	HashString(HashString<T, Allocator, HashType>&& rhs);
	~HashString(void);

	const HashString<T, Allocator, HashType>& operator=(const HashString<T, Allocator, HashType>& rhs);
	const HashString<T, Allocator, HashType>& operator=(const String<T, Allocator>& rhs);
	const HashString<T, Allocator, HashType>& operator=(HashString<T, Allocator, HashType>&& rhs);
	const HashString<T, Allocator, HashType>& operator=(String<T, Allocator>&& rhs);
	const HashString<T, Allocator, HashType>& operator=(const T* rhs);

	bool operator==(const HashString<T, Allocator, HashType>& rhs) const;
	bool operator!=(const HashString<T, Allocator, HashType>& rhs) const;
	bool operator<(const HashString<T, Allocator, HashType>& rhs) const;
	bool operator>(const HashString<T, Allocator, HashType>& rhs) const;

	const HashString<T, Allocator, HashType>& operator+=(const HashString<T, Allocator, HashType>& rhs);
	const HashString<T, Allocator, HashType>& operator+=(const String<T, Allocator>& rhs);
	const HashString<T, Allocator, HashType>& operator+=(const T* rhs);

	HashString<T, Allocator, HashType> operator+(const HashString<T, Allocator, HashType>& rhs) const;
	HashString<T, Allocator, HashType> operator+(const String<T, Allocator>& rhs);
	HashString<T, Allocator, HashType> operator+(const T* rhs) const;

	// WARNING: This function takes ownership of the string instead of copying
	void set(T* string);
	void clear(void);
	size_t size(void) const;

	const String<T, Allocator>& getString(void) const;
	const T* getBuffer(void) const;
	HashType getHash(void) const;

private:
	String<T, Allocator> _string;
	HashType _hash_value;
	HashFunc _hash_func;
};

#include "Gaff_HashString.inl"

template <class Allocator = DefaultAllocator> using HashString32 = HashString<char, Allocator>;
template <class Allocator = DefaultAllocator> using HashString64 = HashString<char, Allocator, uint64_t>;

NS_END
