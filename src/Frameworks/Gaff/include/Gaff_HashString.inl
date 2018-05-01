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

template <class T, class HashType>
HashType CalculateHash(const T* value, size_t size, HashType (*hash_func)(const char*, size_t))
{
	if (hash_func) {
		return hash_func(reinterpret_cast<const char*>(value), size * sizeof(T));
	}

	Hash32 hash_value = FNV1aHash32(reinterpret_cast<const char*>(value), size * sizeof(T));
	return static_cast<HashType>(hash_value);
}

template <class T>
Hash32 CalculateHash(const T* value, size_t size, Hash32 (*hash_func)(const char*, size_t))
{
	if (hash_func) {
		return hash_func(reinterpret_cast<const char*>(value), size * sizeof(T));
	}

	return FNV1aHash32(reinterpret_cast<const char*>(value), size * sizeof(T));
}

template <class T>
Hash64 CalculateHash(const T* value, size_t size, Hash64 (*hash_func)(const char*, size_t))
{
	if (hash_func) {
		return hash_func(reinterpret_cast<const char*>(value), size * sizeof(T));
	}

	return FNV1aHash64(reinterpret_cast<const char*>(value), size * sizeof(T));
}

template <class T, class HashType, class Allocator>
HashType CalculateHash(const String<T, Allocator>& value, HashType(*hash_func)(const char*, size_t))
{
	return CalculateHash(value.data(), value.size(), hash_func);
}



// HashStringTemp
template <class T, class HashType>
template <class Allocator>
HashStringTemp<T, HashType>::HashStringTemp(const String<T, Allocator>& string, HashFunc hash):
	_string(string.data()), _hash_value(CalculateHash(string, hash))
{
}

template <class T, class HashType>
template <size_t size>
HashStringTemp<T, HashType>::HashStringTemp(const T (&string)[size], HashFunc hash):
	_string(string), _hash_value(CalculateHash(string, size - 1, hash))
{
}

template <class T, class HashType>
HashStringTemp<T, HashType>::HashStringTemp(const T* string, size_t size, HashFunc hash):
	_string(string), _hash_value(CalculateHash(string, size, hash))
{
}

template <class T, class HashType>
template <class Allocator>
bool HashStringTemp<T, HashType>::operator==(const HashString<T, HashType, Allocator>& rhs) const
{
	return _hash_value == rhs.getHash();
}

template <class T, class HashType>
template <class Allocator>
bool HashStringTemp<T, HashType>::operator!=(const HashString<T, HashType, Allocator>& rhs) const
{
	return _hash_value != rhs.getHash();
}

template <class T, class HashType>
template <class Allocator>
bool HashStringTemp<T, HashType>::operator<(const HashString<T, HashType, Allocator>& rhs) const
{
	return _hash_value < rhs.getHash();
}

template <class T, class HashType>
template <class Allocator>
bool HashStringTemp<T, HashType>::operator>(const HashString<T, HashType, Allocator>& rhs) const
{
	return _hash_value > rhs.getHash();
}

template <class T, class HashType>
bool HashStringTemp<T, HashType>::operator==(const HashStringTemp<T, HashType>& rhs) const
{
	return _hash_value == rhs._hash_value;
}

template <class T, class HashType>
bool HashStringTemp<T, HashType>::operator!=(const HashStringTemp<T, HashType>& rhs) const
{
	return _hash_value != rhs._hash_value;
}

template <class T, class HashType>
bool HashStringTemp<T, HashType>::operator<(const HashStringTemp<T, HashType>& rhs) const
{
	return _hash_value < rhs._hash_value;
}

template <class T, class HashType>
bool HashStringTemp<T, HashType>::operator>(const HashStringTemp<T, HashType>& rhs) const
{
	return _hash_value > rhs._hash_value;
}

template <class T, class HashType>
bool HashStringTemp<T, HashType>::operator==(HashType rhs) const
{
	return _hash_value == rhs;
}

template <class T, class HashType>
bool HashStringTemp<T, HashType>::operator!=(HashType rhs) const
{
	return _hash_value != rhs;
}

template <class T, class HashType>
bool HashStringTemp<T, HashType>::operator<(HashType rhs) const
{
	return _hash_value < rhs;
}

template <class T, class HashType>
bool HashStringTemp<T, HashType>::operator>(HashType rhs) const
{
	return _hash_value > rhs;
}

template <class T, class HashType>
const T* HashStringTemp<T, HashType>::getBuffer(void) const
{
	return _string;
}

template <class T, class HashType>
HashType HashStringTemp<T, HashType>::getHash(void) const
{
	return _hash_value;
}



// HashString
template <class T, class HashType, class Allocator>
HashString<T, HashType, Allocator>::HashString(const String<T, Allocator>& string, HashFunc hash_func):
	_string(string), _hash_value(CalculateHash(string, hash)), _hash_func(hash_func)
{
}

template <class T, class HashType, class Allocator>
HashString<T, HashType, Allocator>::HashString(const HashStringTemp<T, HashType>& string, HashFunc hash_func, const Allocator& allocator):
	_string(string.getBuffer(), allocator), _hash_value(string.getHash()), _hash_func(hash_func)
{
}

template <class T, class HashType, class Allocator>
HashString<T, HashType, Allocator>::HashString(const T* string, size_t size, HashFunc hash_func, const Allocator& allocator):
	_string(string, size, allocator), _hash_value(0), _hash_func(hash_func)
{
	_hash_value = CalculateHash(_string, hash_func);
}

template <class T, class HashType, class Allocator>
HashString<T, HashType, Allocator>::HashString(const T* string, HashFunc hash_func, const Allocator& allocator):
	_string(string, allocator), _hash_value(0), _hash_func(hash_func)
{
	_hash_value = CalculateHash(_string, hash_func);
}

template <class T, class HashType, class Allocator>
HashString<T, HashType, Allocator>::HashString(const T* string, size_t size, HashType hash, HashFunc hash_func, const Allocator& allocator):
	_string(string, size, allocator), _hash_value(hash), _hash_func(hash_func)
{
}

//template <class T, class HashType, class Allocator>
//HashString<T, HashType, Allocator>::HashString(const T* string, HashType hash, HashFunc hash_func, const Allocator& allocator):
//	_string(string, allocator), _hash_value(hash), _hash_func(hash_func)
//{
//}

template <class T, class HashType, class Allocator>
HashString<T, HashType, Allocator>::HashString(HashType hash, HashFunc hash_func, const Allocator& allocator):
	_string(allocator), _hash_value(hash), _hash_func(hash_func)
{
}

template <class T, class HashType, class Allocator>
HashString<T, HashType, Allocator>::HashString(HashFunc hash_func, const Allocator& allocator):
	_string(allocator), _hash_value(0), _hash_func(hash_func)
{
}

template <class T, class HashType, class Allocator>
HashString<T, HashType, Allocator>& HashString<T, HashType, Allocator>::operator=(const String<T, Allocator>& rhs)
{
	_string = rhs;
	 _hash_value = CalculateHash(_string, _hash_func);
	return *this;
}

template <class T, class HashType, class Allocator>
HashString<T, HashType, Allocator>& HashString<T, HashType, Allocator>::operator=(String<T, Allocator>&& rhs)
{
	_string = std::move(rhs);
	 _hash_value = CalculateHash(_string, _hash_func);
	return *this;
}

template <class T, class HashType, class Allocator>
HashString<T, HashType, Allocator>& HashString<T, HashType, Allocator>::operator=(const T* rhs)
{
	_string = rhs;
	 _hash_value = CalculateHash(_string, _hash_func);
	return *this;
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator==(const HashString<T, HashType, Allocator>& rhs) const
{
	return _hash_value == rhs._hash_value;
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator!=(const HashString<T, HashType, Allocator>& rhs) const
{
	return _hash_value != rhs._hash_value;
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator<(const HashString<T, HashType, Allocator>& rhs) const
{
	return _hash_value < rhs._hash_value;
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator>(const HashString<T, HashType, Allocator>& rhs) const
{
	return _hash_value > rhs._hash_value;
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator==(const HashStringTemp<T, HashType>& rhs) const
{
	return _hash_value == rhs.getHash();
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator!=(const HashStringTemp<T, HashType>& rhs) const
{
	return _hash_value != rhs.getHash();
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator<(const HashStringTemp<T, HashType>& rhs) const
{
	return _hash_value < rhs.getHash();
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator>(const HashStringTemp<T, HashType>& rhs) const
{
	return _hash_value > rhs.getHash();
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator==(HashType rhs) const
{
	return _hash_value == rhs;
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator!=(HashType rhs) const
{
	return _hash_value != rhs;
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator<(HashType rhs) const
{
	return _hash_value < rhs;
}

template <class T, class HashType, class Allocator>
bool HashString<T, HashType, Allocator>::operator>(HashType rhs) const
{
	return _hash_value > rhs;
}

// WARNING: This function takes ownership of the string instead of copying
template <class T, class HashType, class Allocator>
void HashString<T, HashType, Allocator>::set(const T* string)
{
	_string.set(string);
	_hash_value = _hash_func(reinterpret_cast<const char*>(string), _string.size() * sizeof(T));
}

template <class T, class HashType, class Allocator>
void HashString<T, HashType, Allocator>::clear(void)
{
	_string.clear();
	_hash_value = 0;
}

template <class T, class HashType, class Allocator>
size_t HashString<T, HashType, Allocator>::size(void) const
{
	return _string.size();
}

template <class T, class HashType, class Allocator>
const String<T, Allocator>& HashString<T, HashType, Allocator>::getString(void) const
{
	return _string;
}

template <class T, class HashType, class Allocator>
const T* HashString<T, HashType, Allocator>::getBuffer(void) const
{
	return _string.data();
}

template <class T, class HashType, class Allocator>
HashType HashString<T, HashType, Allocator>::getHash(void) const
{
	return _hash_value;
}


template <class HashTypeA, class T, class HashTypeB, class Allocator>
bool operator==(HashTypeA lhs, const HashString<T, HashTypeB, Allocator>& rhs)
{
	return lhs == rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB, class Allocator>
bool operator!=(HashTypeA lhs, const HashString<T, HashTypeB, Allocator>& rhs)
{
	return lhs != rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB, class Allocator>
bool operator<(HashTypeA lhs, const HashString<T, HashTypeB, Allocator>& rhs)
{
	return lhs < rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB, class Allocator>
bool operator>(HashTypeA lhs, const HashString<T, HashTypeB, Allocator>& rhs)
{
	return lhs > rhs.getHash();
}


template <class HashTypeA, class T, class HashTypeB>
bool operator==(HashTypeA lhs, const HashStringTemp<T, HashTypeB>& rhs)
{
	return lhs == rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB>
bool operator!=(HashTypeA lhs, const HashStringTemp<T, HashTypeB>& rhs)
{
	return lhs != rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB>
bool operator<(HashTypeA lhs, const HashStringTemp<T, HashTypeB>& rhs)
{
	return lhs < rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB>
bool operator>(HashTypeA lhs, const HashStringTemp<T, HashTypeB>& rhs)
{
	return lhs > rhs.getHash();
}
