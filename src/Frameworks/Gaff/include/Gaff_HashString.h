/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

template <class HashType>
using HashFunc = HashType (*)(const char*, size_t);

template <class HashType>
struct DefaultHashFuncHelper;

template <>
struct DefaultHashFuncHelper<Hash32> final
{
	static constexpr HashFunc<Hash32> value = FNV1aHash32;
};

template <>
struct DefaultHashFuncHelper<Hash64> final
{
	static constexpr HashFunc<Hash64> value = FNV1aHash64;
};

template <class HashType>
constexpr HashFunc<HashType> DefaultHashFunc = DefaultHashFuncHelper<HashType>::value;

template <class T, class HashType, HashFunc<HashType> HashingFunc = DefaultHashFunc<HashType>, class Allocator = DefaultAllocator, bool contains_string = true>
class HashString;

template < class T, class HashType, HashFunc<HashType> HashingFunc = DefaultHashFunc<HashType> >
class HashStringView
{
public:
	template <class Allocator>
	explicit HashStringView(const HashString<T, HashType, HashingFunc, Allocator, true>& hash_string);

	template <class Allocator>
	explicit HashStringView(const String<T, Allocator>& string);

	template <size_t size>
	explicit HashStringView(const T (&string)[size]);

	HashStringView(const T* string, size_t size);
	explicit HashStringView(const T* string);

	HashStringView(const HashStringView<T, HashType, HashingFunc>& string) = default;
	HashStringView(HashStringView<T, HashType, HashingFunc>&& string) = default;

	HashStringView(void) = default;
	~HashStringView(void) = default;

	HashStringView<T, HashType, HashingFunc>& operator=(const HashStringView<T, HashType, HashingFunc>& string) = default;
	HashStringView<T, HashType, HashingFunc>& operator=(HashStringView<T, HashType, HashingFunc>&& string) = default;

	template <class Allocator>
	bool operator==(const HashString<T, HashType, HashingFunc, Allocator, true>& rhs) const;
	template <class Allocator>
	bool operator!=(const HashString<T, HashType, HashingFunc, Allocator, true>& rhs) const;
	template <class Allocator>
	bool operator<(const HashString<T, HashType, HashingFunc, Allocator, true>& rhs) const;
	template <class Allocator>
	bool operator>(const HashString<T, HashType, HashingFunc, Allocator, true>& rhs) const;

	bool operator==(const HashStringView<T, HashType, HashingFunc>& rhs) const;
	bool operator!=(const HashStringView<T, HashType, HashingFunc>& rhs) const;
	bool operator<(const HashStringView<T, HashType, HashingFunc>& rhs) const;
	bool operator>(const HashStringView<T, HashType, HashingFunc>& rhs) const;

	bool operator==(HashType rhs) const;
	bool operator!=(HashType rhs) const;
	bool operator<(HashType rhs) const;
	bool operator>(HashType rhs) const;

	const T* getBuffer(void) const;
	HashType getHash(void) const;

	operator HashType(void) const;

private:
	const T* _string = nullptr;
	HashType _hash_value = HashType(0);
};

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
class HashString<T, HashType, HashingFunc, Allocator, true> final
{
public:
	explicit HashString(const String<T, Allocator>& string);
	HashString(const T* string, size_t size, const Allocator& allocator = Allocator());
	explicit HashString(const HashStringView<T, HashType, HashingFunc>& string, const Allocator& allocator = Allocator());
	explicit HashString(const T* string, const Allocator& allocator = Allocator());
	HashString(const T* string, size_t size, HashType hash, const Allocator& allocator = Allocator());
	HashString(const T* string, HashType hash, const Allocator& allocator = Allocator());
	HashString(HashType hash, const Allocator& allocator = Allocator());
	HashString(const Allocator& allocator = Allocator());

	HashString(const HashString<T, HashType, HashingFunc, Allocator, true>& rhs) = default;
	HashString(HashString<T, HashType, HashingFunc, Allocator, true>&& rhs) = default;

	~HashString(void) = default;

	HashString<T, HashType, HashingFunc, Allocator, true>& operator=(const HashString<T, HashType, HashingFunc, Allocator, true>& rhs) = default;
	HashString<T, HashType, HashingFunc, Allocator, true>& operator=(HashString<T, HashType, HashingFunc, Allocator, true>&& rhs) = default;
	HashString<T, HashType, HashingFunc, Allocator, true>& operator=(const HashStringView<T, HashType, HashingFunc>& rhs);
	HashString<T, HashType, HashingFunc, Allocator, true>& operator=(const String<T, Allocator>& rhs);
	HashString<T, HashType, HashingFunc, Allocator, true>& operator=(String<T, Allocator>&& rhs);
	HashString<T, HashType, HashingFunc, Allocator, true>& operator=(const T* rhs);

	template <bool HasString>
	bool operator==(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const;

	template <bool HasString>
	bool operator!=(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const;

	template <bool HasString>
	bool operator<(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const;

	template <bool HasString>
	bool operator>(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const;

	bool operator==(const HashStringView<T, HashType, HashingFunc>& rhs) const;
	bool operator!=(const HashStringView<T, HashType, HashingFunc>& rhs) const;
	bool operator<(const HashStringView<T, HashType, HashingFunc>& rhs) const;
	bool operator>(const HashStringView<T, HashType, HashingFunc>& rhs) const;

	bool operator==(HashType rhs) const;
	bool operator!=(HashType rhs) const;
	bool operator<(HashType rhs) const;
	bool operator>(HashType rhs) const;

	operator HashStringView<T, HashType, HashingFunc>(void) const;


	// WARNING: This function takes ownership of the string instead of copying
	void set(const T* string);
	void clear(void);

	size_t size(void) const;
	bool empty(void) const;

	const String<T, Allocator>& getString(void) const;
	const T* getBuffer(void) const;
	HashType getHash(void) const;

private:
	String<T, Allocator> _string;
	HashType _hash_value;
};

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
class HashString<T, HashType, HashingFunc, Allocator, false> final
{
public:
	explicit HashString(const String<T, Allocator>& string);
	HashString(const T* string, size_t size, const Allocator& allocator = Allocator());
	explicit HashString(const HashStringView<T, HashType, HashingFunc>& string, const Allocator& allocator = Allocator());
	explicit HashString(const T* string, const Allocator& allocator = Allocator());
	HashString(const T* string, size_t size, HashType hash, const Allocator& allocator = Allocator());
	// HashString64 gets build errors from this constructor, as Hash64 is the same data type as size_t.
	//HashString(const T* string, HashType hash, const Allocator& allocator = Allocator());
	HashString(HashType hash, const Allocator& allocator = Allocator());
	HashString(const Allocator& allocator = Allocator());

	template <bool HasString>
	HashString(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs);

	~HashString(void) = default;

	template <bool HasString>
	HashString<T, HashType, HashingFunc, Allocator, false>& operator=(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs);

	HashString<T, HashType, HashingFunc, Allocator, false>& operator=(HashString<T, HashType, HashingFunc, Allocator, false>&& rhs) = default;
	HashString<T, HashType, HashingFunc, Allocator, false>& operator=(const HashStringView<T, HashType, HashingFunc>& rhs);
	HashString<T, HashType, HashingFunc, Allocator, false>& operator=(const String<T, Allocator>& rhs);
	HashString<T, HashType, HashingFunc, Allocator, false>& operator=(const T* rhs);

	template <bool HasString>
	bool operator==(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const;

	template <bool HasString>
	bool operator!=(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const;

	template <bool HasString>
	bool operator<(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const;

	template <bool HasString>
	bool operator>(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const;

	bool operator==(const HashStringView<T, HashType, HashingFunc>& rhs) const;
	bool operator!=(const HashStringView<T, HashType, HashingFunc>& rhs) const;
	bool operator<(const HashStringView<T, HashType, HashingFunc>& rhs) const;
	bool operator>(const HashStringView<T, HashType, HashingFunc>& rhs) const;

	bool operator==(HashType rhs) const;
	bool operator!=(HashType rhs) const;
	bool operator<(HashType rhs) const;
	bool operator>(HashType rhs) const;

	HashType getHash(void) const;

private:
	HashType _hash_value;
};

#include "Gaff_HashString.inl"

template < class Allocator = DefaultAllocator, HashFunc<Hash32> HashingFunc = DefaultHashFunc<Hash32> >
using HashString32 = HashString<char8_t, Hash32, HashingFunc, Allocator, true>;

template < class Allocator = DefaultAllocator, HashFunc<Hash64> HashingFunc = DefaultHashFunc<Hash64> >
using HashString64 = HashString<char8_t, Hash64, HashingFunc, Allocator, true>;

template < HashFunc<Hash32> HashingFunc = DefaultHashFunc<Hash32> >
using HashStringView32 = HashStringView<char8_t, Hash32, HashingFunc>;

template < HashFunc<Hash64> HashingFunc = DefaultHashFunc<Hash64> >
using HashStringView64 = HashStringView<char8_t, Hash64, HashingFunc>;

template <class Allocator = DefaultAllocator, HashFunc<Hash32> HashingFunc = DefaultHashFunc<Hash32>>
using HashStringNoString32 = HashString<char8_t, Hash32, HashingFunc, Allocator, false>;

template <class Allocator = DefaultAllocator, HashFunc<Hash64> HashingFunc = DefaultHashFunc<Hash64>>
using HashStringNoString64 = HashString<char8_t, Hash64, HashingFunc, Allocator, false>;

#ifdef _DEBUG
	template <class Allocator = DefaultAllocator, HashFunc<Hash32> HashingFunc = DefaultHashFunc<Hash32>>
	using OptimizedHashString32 = HashString<char8_t, Hash32, HashingFunc, Allocator, true>;

	template <class Allocator = DefaultAllocator, HashFunc<Hash64> HashingFunc = DefaultHashFunc<Hash64>>
	using OptimizedHashString64 = HashString<char8_t, Hash64, HashingFunc, Allocator, true>;
#else
	template <class Allocator = DefaultAllocator, HashFunc<Hash32> HashingFunc = DefaultHashFunc<Hash32>>
	using OptimizedHashString32 = HashString<char8_t, Hash32, HashingFunc, Allocator, false>;

	template <class Allocator = DefaultAllocator, HashFunc<Hash64> HashingFunc = DefaultHashFunc<Hash64>>
	using OptimizedHashString64 = HashString<char8_t, Hash64, HashingFunc, Allocator, false>;
#endif


template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc, class Allocator, bool contains_string>
bool operator==(HashTypeA lhs, const HashString<T, HashTypeB, HashingFunc, Allocator, contains_string>& rhs);

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc, class Allocator, bool contains_string>
bool operator!=(HashTypeA lhs, const HashString<T, HashTypeB, HashingFunc, Allocator, contains_string>& rhs);

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc, class Allocator, bool contains_string>
bool operator<(HashTypeA lhs, const HashString<T, HashTypeB, HashingFunc, Allocator, contains_string>& rhs);

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc, class Allocator, bool contains_string>
bool operator>(HashTypeA lhs, const HashString<T, HashTypeB, HashingFunc, Allocator, contains_string>& rhs);


template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc>
bool operator==(HashTypeA lhs, const HashStringView<T, HashTypeB, HashingFunc>& rhs);

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc>
bool operator!=(HashTypeA lhs, const HashStringView<T, HashTypeB, HashingFunc>& rhs);

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc>
bool operator<(HashTypeA lhs, const HashStringView<T, HashTypeB, HashingFunc>& rhs);

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc>
bool operator>(HashTypeA lhs, const HashStringView<T, HashTypeB, HashingFunc>& rhs);

NS_END
