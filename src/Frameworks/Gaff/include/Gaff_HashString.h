/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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
#include "Gaff_Hash.h"

NS_GAFF

template <class T, class HashType, class Allocator>
class HashString;

template <class T, class HashType>
class HashStringTemp
{
public:
	using HashFunc = HashType(*)(const char*, size_t);

	template <class Allocator>
	HashStringTemp(const String<T, Allocator>& string, HashFunc hash_func = nullptr);

	template <size_t size>
	HashStringTemp(const T (&string)[size], HashFunc hash_func = nullptr);

	HashStringTemp(const T* string, size_t size, HashFunc hash_func = nullptr);

	HashStringTemp(const HashStringTemp<T, HashType>& string) = default;
	HashStringTemp(HashStringTemp<T, HashType>&& string) = default;

	~HashStringTemp(void) = default;

	HashStringTemp<T, HashType>& operator=(const HashStringTemp<T, HashType>& string) = default;
	HashStringTemp<T, HashType>& operator=(HashStringTemp<T, HashType>&& string) = default;

	template <class Allocator>
	bool operator==(const HashString<T, HashType, Allocator>& rhs) const;
	template <class Allocator>
	bool operator!=(const HashString<T, HashType, Allocator>& rhs) const;
	template <class Allocator>
	bool operator<(const HashString<T, HashType, Allocator>& rhs) const;
	template <class Allocator>
	bool operator>(const HashString<T, HashType, Allocator>& rhs) const;

	bool operator==(const HashStringTemp<T, HashType>& rhs) const;
	bool operator!=(const HashStringTemp<T, HashType>& rhs) const;
	bool operator<(const HashStringTemp<T, HashType>& rhs) const;
	bool operator>(const HashStringTemp<T, HashType>& rhs) const;

	bool operator==(HashType rhs) const;
	bool operator!=(HashType rhs) const;
	bool operator<(HashType rhs) const;
	bool operator>(HashType rhs) const;

	const T* getBuffer(void) const;
	HashType getHash(void) const;

private:
	const T* const _string;
	HashType _hash_value;
};

template <class T, class HashType, class Allocator = DefaultAllocator>
class HashString
{
public:
	using HashFunc = HashType (*)(const char*, size_t);

	explicit HashString(const String<T, Allocator>& string, HashFunc hash_func = nullptr);
	HashString(const T* string, size_t size, HashFunc hash_func = nullptr, const Allocator& allocator = Allocator());
	explicit HashString(const HashStringTemp<T, HashType>& string, HashFunc hash_func = nullptr, const Allocator& allocator = Allocator());
	explicit HashString(const T* string, HashFunc hash_func = nullptr, const Allocator& allocator = Allocator());
	HashString(const T* string, size_t size, HashType hash, HashFunc hash_func = nullptr, const Allocator& allocator = Allocator());
	// HashString64 gets build errors from this constructor, as Hash64 is the same data type as size_t.
	//HashString(const T* string, HashType hash, HashFunc hash_func = nullptr, const Allocator& allocator = Allocator());
	HashString(HashType hash, HashFunc hash_func = nullptr, const Allocator& allocator = Allocator());
	HashString(HashFunc hash_func = nullptr, const Allocator& allocator = Allocator());

	HashString(const HashString<T, HashType, Allocator>& rhs) = default;
	HashString(HashString<T, HashType, Allocator>&& rhs) = default;

	~HashString(void) = default;

	HashString<T, HashType, Allocator>& operator=(const HashString<T, HashType, Allocator>& rhs) = default;
	HashString<T, HashType, Allocator>& operator=(HashString<T, HashType, Allocator>&& rhs) = default;
	HashString<T, HashType, Allocator>& operator=(const String<T, Allocator>& rhs);
	HashString<T, HashType, Allocator>& operator=(String<T, Allocator>&& rhs);
	HashString<T, HashType, Allocator>& operator=(const T* rhs);

	bool operator==(const HashString<T, HashType, Allocator>& rhs) const;
	bool operator!=(const HashString<T, HashType, Allocator>& rhs) const;
	bool operator<(const HashString<T, HashType, Allocator>& rhs) const;
	bool operator>(const HashString<T, HashType, Allocator>& rhs) const;

	bool operator==(const HashStringTemp<T, HashType>& rhs) const;
	bool operator!=(const HashStringTemp<T, HashType>& rhs) const;
	bool operator<(const HashStringTemp<T, HashType>& rhs) const;
	bool operator>(const HashStringTemp<T, HashType>& rhs) const;

	bool operator==(HashType rhs) const;
	bool operator!=(HashType rhs) const;
	bool operator<(HashType rhs) const;
	bool operator>(HashType rhs) const;

	// WARNING: This function takes ownership of the string instead of copying
	void set(const T* string);
	void clear(void);
	size_t size(void) const;

	const String<T, Allocator>& getString(void) const;
	const T* getBuffer(void) const;
	HashType getHash(void) const;

private:
	String<T, Allocator> _string;
	HashType _hash_value;
	HashFunc _hash_func = nullptr;
};

#include "Gaff_HashString.inl"

template <class Allocator = DefaultAllocator> using HashString32 = HashString<char, Hash32, Allocator>;
template <class Allocator = DefaultAllocator> using HashString64 = HashString<char, Hash64, Allocator>;
using HashStringTemp32 = HashStringTemp<char, Hash32>;
using HashStringTemp64 = HashStringTemp<char, Hash64>;


template <class HashTypeA, class T, class HashTypeB, class Allocator>
bool operator==(HashTypeA lhs, const HashString<T, HashTypeB, Allocator>& rhs);

template <class HashTypeA, class T, class HashTypeB, class Allocator>
bool operator!=(HashTypeA lhs, const HashString<T, HashTypeB, Allocator>& rhs);

template <class HashTypeA, class T, class HashTypeB, class Allocator>
bool operator<(HashTypeA lhs, const HashString<T, HashTypeB, Allocator>& rhs);

template <class HashTypeA, class T, class HashTypeB, class Allocator>
bool operator>(HashTypeA lhs, const HashString<T, HashTypeB, Allocator>& rhs);


template <class HashTypeA, class T, class HashTypeB>
bool operator==(HashTypeA lhs, const HashStringTemp<T, HashTypeB>& rhs);

template <class HashTypeA, class T, class HashTypeB>
bool operator!=(HashTypeA lhs, const HashStringTemp<T, HashTypeB>& rhs);

template <class HashTypeA, class T, class HashTypeB>
bool operator<(HashTypeA lhs, const HashStringTemp<T, HashTypeB>& rhs);

template <class HashTypeA, class T, class HashTypeB>
bool operator>(HashTypeA lhs, const HashStringTemp<T, HashTypeB>& rhs);

NS_END
