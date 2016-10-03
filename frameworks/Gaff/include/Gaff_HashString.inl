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

template <class T, class Allocator, class HashType>
HashType CalculateHash(const String<T, Allocator>& value, HashType (*hash_func)(const char*, size_t))
{
	if (hash_func) {
		return hash_func(reinterpret_cast<const char*>(_string.getBuffer()), _string.size() * sizeof(T));
	}

	uint32_t hash_value = FNV1aHash32(reinterpret_cast<const char*>(_string.getBuffer()), _string.size() * sizeof(T));
	return static_cast<HashType>(hash_value);
}

template <class T, class Allocator>
uint32_t CalculateHash(const String<T, Allocator>& value, uint32_t (*hash_func)(const char*, size_t))
{
	if (hash_func) {
		return hash_func(reinterpret_cast<const char*>(value.getBuffer()), value.size() * sizeof(T));
	}

	return FNV1aHash32(reinterpret_cast<const char*>(value.getBuffer()), value.size() * sizeof(T));
}

template <class T, class Allocator>
uint64_t CalculateHash(const String<T, Allocator>& value, uint64_t (*hash_func)(const char*, size_t))
{
	if (hash_func) {
		return hash_func(reinterpret_cast<const char*>(value.getBuffer()), value.size() * sizeof(T));
	}

	return FNV1aHash64(reinterpret_cast<const char*>(value.getBuffer()), value.size() * sizeof(T));
}


template <class T, class Allocator, class HashType>
HashString<T, Allocator, HashType>::HashString(const HashString<T, Allocator, HashType>& string, HashFunc hash):
	_string(string._string), _hash_value(0), _hash_func(hash)
{
	_hash_value = CalculateHash(_string, _hash_func);
}

template <class T, class Allocator, class HashType>
HashString<T, Allocator, HashType>::HashString(const String<T, Allocator>& string, HashFunc hash):
	_string(string), _hash_value(0), _hash_func(hash)
{
	_hash_value = CalculateHash(_string, _hash_func);
}

template <class T, class Allocator, class HashType>
HashString<T, Allocator, HashType>::HashString(const T* string, HashFunc hash, const Allocator& allocator):
	_string(string, allocator), _hash_value(0), _hash_func(hash)
{
	_hash_value = CalculateHash(_string, _hash_func);
}

template <class T, class Allocator, class HashType>
HashString<T, Allocator, HashType>::HashString(HashFunc hash, const Allocator& allocator):
	_string(allocator), _hash_value(0), _hash_func(hash)
{
}

template <class T, class Allocator, class HashType>
HashString<T, Allocator, HashType>::HashString(HashString<T, Allocator, HashType>&& rhs):
	_string(std::move(rhs._string)), _hash_value(rhs._hash_value), _hash_func(rhs._hash_func)
{
}

template <class T, class Allocator, class HashType>
HashString<T, Allocator, HashType>::~HashString(void)
{
}

template <class T, class Allocator, class HashType>
const HashString<T, Allocator, HashType>& HashString<T, Allocator, HashType>::operator=(const HashString<T, Allocator, HashType>& rhs)
{
	if (this == &rhs) {
		return *this;
	}

	_string = rhs._string;
	_hash_value = rhs._hash_value;
	_hash_func = rhs._hash_func;

	return *this;
}

template <class T, class Allocator, class HashType>
const HashString<T, Allocator, HashType>& HashString<T, Allocator, HashType>::operator=(const String<T, Allocator>& rhs)
{
	_string = rhs;
	 _hash_value = CalculateHash(_string, _hash_func);
	return *this;
}

template <class T, class Allocator, class HashType>
const HashString<T, Allocator, HashType>& HashString<T, Allocator, HashType>::operator=(HashString<T, Allocator, HashType>&& rhs)
{
	if (this == &rhs) {
		return *this;
	}

	_string = std::move(rhs._string);
	_hash_value = rhs._hash_value;
	_hash_func = rhs._hash_func;

	return *this;
}

template <class T, class Allocator, class HashType>
const HashString<T, Allocator, HashType>& HashString<T, Allocator, HashType>::operator=(String<T, Allocator>&& rhs)
{
	_string = std::move(rhs);
	 _hash_value = CalculateHash(_string, _hash_func);
	return *this;
}

template <class T, class Allocator, class HashType>
const HashString<T, Allocator, HashType>& HashString<T, Allocator, HashType>::operator=(const T* rhs)
{
	_string = rhs;
	 _hash_value = CalculateHash(_string, _hash_func);
	return *this;
}

template <class T, class Allocator, class HashType>
bool HashString<T, Allocator, HashType>::operator==(const HashString<T, Allocator, HashType>& rhs) const
{
	return _hash_value == rhs._hash_value;
}

template <class T, class Allocator, class HashType>
bool HashString<T, Allocator, HashType>::operator!=(const HashString<T, Allocator, HashType>& rhs) const
{
	return _hash_value != rhs._hash_value;
}

template <class T, class Allocator, class HashType>
const HashString<T, Allocator, HashType>& HashString<T, Allocator, HashType>::operator+=(const HashString<T, Allocator, HashType>& rhs)
{
	return (*this = _string + rhs._string);
}

template <class T, class Allocator, class HashType>
const HashString<T, Allocator, HashType>& HashString<T, Allocator, HashType>::operator+=(const String<T, Allocator>& rhs)
{
	return (*this = _string + rhs);
}

template <class T, class Allocator, class HashType>
const HashString<T, Allocator, HashType>& HashString<T, Allocator, HashType>::operator+=(const T* rhs)
{
	return (*this = _string + rhs);
}

template <class T, class Allocator, class HashType>
HashString<T, Allocator, HashType> HashString<T, Allocator, HashType>::operator+(const HashString<T, Allocator, HashType>& rhs) const
{
	return HashString(_string + rhs._string, _hash_func);
}

template <class T, class Allocator, class HashType>
HashString<T, Allocator, HashType> HashString<T, Allocator, HashType>::operator+(const String<T, Allocator>& rhs)
{
	return HashString(_string + rhs);
}

template <class T, class Allocator, class HashType>
HashString<T, Allocator, HashType> HashString<T, Allocator, HashType>::operator+(const T* rhs) const
{
	return HashString(_string + rhs, _hash_func);
}

// WARNING: This function takes ownership of the string instead of copying
/*!
	\brief Sets the raw string to be used internally. Does not allocate to make a copy.
	\param string The raw string to use.
	\note Using this function means you are passing ownership of \a string to HashString.
*/
template <class T, class Allocator, class HashType>
void HashString<T, Allocator, HashType>::set(T* string)
{
	_string.set(string);
	_hash_value = _hash_func(reinterpret_cast<const char*>(string), _string.size() * sizeof(T));
}

template <class T, class Allocator, class HashType>
void HashString<T, Allocator, HashType>::clear(void)
{
	_string.clear();
	_hash_value = 0;
}

template <class T, class Allocator, class HashType>
size_t HashString<T, Allocator, HashType>::size(void) const
{
	return _string.size();
}

template <class T, class Allocator, class HashType>
const String<T, Allocator>& HashString<T, Allocator, HashType>::getString(void) const
{
	return _string;
}

template <class T, class Allocator, class HashType>
const T* HashString<T, Allocator, HashType>::getBuffer(void) const
{
	return _string.getBuffer();
}

template <class T, class Allocator, class HashType>
HashType HashString<T, Allocator, HashType>::getHash(void) const
{
	return _hash_value;
}
