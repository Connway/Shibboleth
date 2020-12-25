/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
HashType CalculateHash(const T* value, size_t size, HashFunc<HashType> hash_func)
{
	return hash_func(reinterpret_cast<const char*>(value), size * sizeof(T));
}

template <class T, class HashType, class Allocator>
HashType CalculateHash(const String<T, Allocator>& value, HashFunc<HashType> hash_func)
{
	return hash_func(reinterpret_cast<const char*>(value.data()), value.size() * sizeof(T));
}



// HashStringView
template <class T, class HashType, HashFunc<HashType> HashingFunc>
template <class Allocator>
HashStringView<T, HashType, HashingFunc>::HashStringView(const HashString<T, HashType, HashingFunc, Allocator, true>& hash_string):
	_string(hash_string.getBuffer()), _hash_value(hash_string.getHash())
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
template <class Allocator>
HashStringView<T, HashType, HashingFunc>::HashStringView(const String<T, Allocator>& string):
	_string(string.data()), _hash_value(CalculateHash(string, HashingFunc))
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
template <size_t size>
HashStringView<T, HashType, HashingFunc>::HashStringView(const T (&string)[size]):
	_string(string), _hash_value(CalculateHash(string, size - 1, HashingFunc))
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
HashStringView<T, HashType, HashingFunc>::HashStringView(const T* string, size_t size):
	_string(string), _hash_value(CalculateHash(string, size, HashingFunc))
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
HashStringView<T, HashType, HashingFunc>::HashStringView(const T* string):
	HashStringView(string, eastl::CharStrlen(string))
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
template <class Allocator>
bool HashStringView<T, HashType, HashingFunc>::operator==(const HashString<T, HashType, HashingFunc, Allocator, true>& rhs) const
{
	return _hash_value == rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
template <class Allocator>
bool HashStringView<T, HashType, HashingFunc>::operator!=(const HashString<T, HashType, HashingFunc, Allocator, true>& rhs) const
{
	return _hash_value != rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
template <class Allocator>
bool HashStringView<T, HashType, HashingFunc>::operator<(const HashString<T, HashType, HashingFunc, Allocator, true>& rhs) const
{
	return _hash_value < rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
template <class Allocator>
bool HashStringView<T, HashType, HashingFunc>::operator>(const HashString<T, HashType, HashingFunc, Allocator, true>& rhs) const
{
	return _hash_value > rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
bool HashStringView<T, HashType, HashingFunc>::operator==(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value == rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
bool HashStringView<T, HashType, HashingFunc>::operator!=(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value != rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
bool HashStringView<T, HashType, HashingFunc>::operator<(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value < rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
bool HashStringView<T, HashType, HashingFunc>::operator>(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value > rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
bool HashStringView<T, HashType, HashingFunc>::operator==(HashType rhs) const
{
	return _hash_value == rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
bool HashStringView<T, HashType, HashingFunc>::operator!=(HashType rhs) const
{
	return _hash_value != rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
bool HashStringView<T, HashType, HashingFunc>::operator<(HashType rhs) const
{
	return _hash_value < rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
bool HashStringView<T, HashType, HashingFunc>::operator>(HashType rhs) const
{
	return _hash_value > rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
const T* HashStringView<T, HashType, HashingFunc>::getBuffer(void) const
{
	return _string;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
HashType HashStringView<T, HashType, HashingFunc>::getHash(void) const
{
	return _hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc>
HashStringView<T, HashType, HashingFunc>::operator HashType(void) const
{
	return _hash_value;
}




// HashString<true>
template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>::HashString(const String<T, Allocator>& string):
	_string(string), _hash_value(CalculateHash(string, HashingFunc))
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>::HashString(const HashStringView<T, HashType, HashingFunc>& string, const Allocator& allocator):
	_string(string.getBuffer(), allocator), _hash_value(string.getHash())
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>::HashString(const T* string, size_t size, const Allocator& allocator):
	_string(string, size, allocator), _hash_value(CalculateHash(string, size, HashingFunc))
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>::HashString(const T* string, const Allocator& allocator):
	_string(string, allocator), _hash_value(CalculateHash(string, eastl::CharStrlen(string), HashingFunc))
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>::HashString(const T* string, size_t size, HashType hash, const Allocator& allocator):
	_string(string, size, allocator), _hash_value(hash)
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>::HashString(const T* string, HashType hash, const Allocator& allocator):
	_string(string, allocator), _hash_value(hash)
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>::HashString(HashType hash, const Allocator& allocator):
	_string(allocator), _hash_value(hash)
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>::HashString(const Allocator& allocator):
	_string(allocator), _hash_value(0)
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>& HashString<T, HashType, HashingFunc, Allocator, true>::operator=(const HashStringView<T, HashType, HashingFunc>& rhs)
{
	_string = rhs.getBuffer();
	_hash_value = rhs.getHash();
	return *this;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>& HashString<T, HashType, HashingFunc, Allocator, true>::operator=(const String<T, Allocator>& rhs)
{
	_string = rhs;
	 _hash_value = CalculateHash(_string, HashingFunc);
	return *this;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>& HashString<T, HashType, HashingFunc, Allocator, true>::operator=(String<T, Allocator>&& rhs)
{
	_string = std::move(rhs);
	 _hash_value = CalculateHash(_string, HashingFunc);
	return *this;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>& HashString<T, HashType, HashingFunc, Allocator, true>::operator=(const T* rhs)
{
	_string = rhs;
	 _hash_value = CalculateHash(_string, HashingFunc);
	return *this;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
template <bool HasString>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator==(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const
{
	return _hash_value == rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
template <bool HasString>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator!=(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const
{
	return _hash_value != rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
template <bool HasString>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator<(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const
{
	return _hash_value < rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
template <bool HasString>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator>(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const
{
	return _hash_value > rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator==(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value == rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator!=(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value != rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator<(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value < rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator>(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value > rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator==(HashType rhs) const
{
	return _hash_value == rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator!=(HashType rhs) const
{
	return _hash_value != rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator<(HashType rhs) const
{
	return _hash_value < rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, true>::operator>(HashType rhs) const
{
	return _hash_value > rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, true>::operator HashStringView<T, HashType, HashingFunc>(void) const
{
	return HashStringView<T, HashType, HashingFunc>(*this);
}

// WARNING: This function takes ownership of the string instead of copying
template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
void HashString<T, HashType, HashingFunc, Allocator, true>::set(const T* string)
{
	_string.set(string);
	_hash_value = HashingFunc(reinterpret_cast<const char*>(string), _string.size() * sizeof(T));
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
void HashString<T, HashType, HashingFunc, Allocator, true>::clear(void)
{
	_string.clear();
	_hash_value = 0;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
size_t HashString<T, HashType, HashingFunc, Allocator, true>::size(void) const
{
	return _string.size();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
const String<T, Allocator>& HashString<T, HashType, HashingFunc, Allocator, true>::getString(void) const
{
	return _string;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
const T* HashString<T, HashType, HashingFunc, Allocator, true>::getBuffer(void) const
{
	return _string.data();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashType HashString<T, HashType, HashingFunc, Allocator, true>::getHash(void) const
{
	return _hash_value;
}



// HashString<false>
template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, false>::HashString(const String<T, Allocator>& string):
	_hash_value(CalculateHash(string, HashingFunc))
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, false>::HashString(const HashStringView<T, HashType, HashingFunc>& string, const Allocator&):
	_hash_value(string.getHash())
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, false>::HashString(const T* string, size_t size, const Allocator&):
	_hash_value(CalculateHash(string, size, HashingFunc))
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, false>::HashString(const T* string, const Allocator&):
	_hash_value(CalculateHash(string, eastl::CharStrlen(string), HashingFunc))
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, false>::HashString(const T* string, size_t size, HashType hash, const Allocator&):
	_hash_value(hash)
{
}

//template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
//HashString<T, HashType, HashingFunc, Allocator, false>::HashString(const T* string, HashType hash, const Allocator&):
//	_hash_value(hash)
//{
//}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, false>::HashString(HashType hash, const Allocator&):
	_hash_value(hash)
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, false>::HashString(const Allocator&):
	_hash_value(0)
{
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
template <bool HasString>
HashString<T, HashType, HashingFunc, Allocator, false>& HashString<T, HashType, HashingFunc, Allocator, false>::operator=(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs)
{
	_hash_value = rhs._hash_value;
	return *this;

}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, false>& HashString<T, HashType, HashingFunc, Allocator, false>::operator=(const HashStringView<T, HashType, HashingFunc>& rhs)
{
	_hash_value = rhs.getHash();
	return *this;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, false>& HashString<T, HashType, HashingFunc, Allocator, false>::operator=(const String<T, Allocator>& rhs)
{
	_hash_value = CalculateHash(rhs, HashingFunc);
	return *this;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashString<T, HashType, HashingFunc, Allocator, false>& HashString<T, HashType, HashingFunc, Allocator, false>::operator=(const T* rhs)
{
	_hash_value = CalculateHash(rhs, eastl::CharStrlen(rhs), HashingFunc);
	return *this;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
template <bool HasString>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator==(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const
{
	return _hash_value == rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
template <bool HasString>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator!=(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const
{
	return _hash_value != rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
template <bool HasString>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator<(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const
{
	return _hash_value < rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
template <bool HasString>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator>(const HashString<T, HashType, HashingFunc, Allocator, HasString>& rhs) const
{
	return _hash_value > rhs._hash_value;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator==(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value == rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator!=(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value != rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator<(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value < rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator>(const HashStringView<T, HashType, HashingFunc>& rhs) const
{
	return _hash_value > rhs.getHash();
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator==(HashType rhs) const
{
	return _hash_value == rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator!=(HashType rhs) const
{
	return _hash_value != rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator<(HashType rhs) const
{
	return _hash_value < rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
bool HashString<T, HashType, HashingFunc, Allocator, false>::operator>(HashType rhs) const
{
	return _hash_value > rhs;
}

template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
HashType HashString<T, HashType, HashingFunc, Allocator, false>::getHash(void) const
{
	return _hash_value;
}



// HashString helpers
template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc, class Allocator, bool contains_string>
bool operator==(HashTypeA lhs, const HashString<T, HashTypeB, HashingFunc, Allocator, contains_string>& rhs)
{
	return lhs == rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc, class Allocator, bool contains_string>
bool operator!=(HashTypeA lhs, const HashString<T, HashTypeB, HashingFunc, Allocator, contains_string>& rhs)
{
	return lhs != rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc, class Allocator, bool contains_string>
bool operator<(HashTypeA lhs, const HashString<T, HashTypeB, HashingFunc, Allocator, contains_string>& rhs)
{
	return lhs < rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc, class Allocator, bool contains_string>
bool operator>(HashTypeA lhs, const HashString<T, HashTypeB, HashingFunc, Allocator, contains_string>& rhs)
{
	return lhs > rhs.getHash();
}


template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc>
bool operator==(HashTypeA lhs, const HashStringView<T, HashTypeB, HashingFunc>& rhs)
{
	return lhs == rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc>
bool operator!=(HashTypeA lhs, const HashStringView<T, HashTypeB, HashingFunc>& rhs)
{
	return lhs != rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc>
bool operator<(HashTypeA lhs, const HashStringView<T, HashTypeB, HashingFunc>& rhs)
{
	return lhs < rhs.getHash();
}

template <class HashTypeA, class T, class HashTypeB, HashFunc<HashTypeB> HashingFunc>
bool operator>(HashTypeA lhs, const HashStringView<T, HashTypeB, HashingFunc>& rhs)
{
	return lhs > rhs.getHash();
}
