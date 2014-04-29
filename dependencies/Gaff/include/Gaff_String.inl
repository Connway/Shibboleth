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
const unsigned int String<T, Allocator>::npos = (unsigned int)-1;


template <class T, class Allocator>
String<T, Allocator>::String(const Allocator& allocator):
	_allocator(allocator), _size(0), _string(nullptr)
{
}

template <class T, class Allocator>
String<T, Allocator>::String(const T* string, unsigned int size, const Allocator& allocator):
	_allocator(allocator), _size(0), _string(nullptr)
{
	_string = (T*)_allocator.alloc(sizeof(T) * (size + 1));
	_size = size;

	_string[_size] = 0; // set null-terminator

	for (unsigned int i = 0; i < _size; ++i) {
		_string[i] = string[i];
	}
}

template <class T, class Allocator>
String<T, Allocator>::String(const T* string, const Allocator& allocator):
	_allocator(allocator), _size(0), _string(nullptr)
{
	*this = string;
}

template <class T, class Allocator>
String<T, Allocator>::String(String<T, Allocator>&& rhs):
	_allocator(rhs._allocator), _size(rhs._size),
	_string(rhs._string)
{
	rhs._string = nullptr;
	rhs._size = 0;
}

template <class T, class Allocator>
String<T, Allocator>::String(const String<T, Allocator>& string):
	_allocator(string._allocator), _size(0), _string(nullptr)
{
	*this = string;
}

template <class T, class Allocator>
String<T, Allocator>::~String(void)
{
	clear();
}

template <class T, class Allocator>
const String<T, Allocator>& String<T, Allocator>::operator=(String<T, Allocator>&& rhs)
{
	clear();

	_string = rhs._string;
	_size = rhs._size;

	rhs._string = nullptr;
	rhs._size = 0;

	return *this;
}

template <class T, class Allocator>
const String<T, Allocator>& String<T, Allocator>::operator=(const String<T, Allocator>& rhs)
{
	//if (this == &rhs) {
	//	return *this;
	//}

	clear();

	_size = rhs._size;
	_string = (T*)_allocator.alloc(sizeof(T) * (_size + 1));

	_string[_size] = 0; // set null-terminator

	for (unsigned int i = 0; i < _size; ++i) {
		_string[i] = rhs._string[i];
	}

	return *this;
}

template <class T, class Allocator>
const String<T, Allocator>& String<T, Allocator>::operator=(const T* rhs)
{
	//if (_string == rhs) {
	//	return *this;
	//}

	clear();

	_size = length(rhs);
	_string = (T*)_allocator.alloc(sizeof(T) * (_size + 1));

	_string[_size] = 0; // set null-terminator

	for (unsigned int i = 0; i < _size; ++i) {
		_string[i] = rhs[i];
	}

	return *this;
}

template <class T, class Allocator>
bool String<T, Allocator>::operator==(const String<T, Allocator>& rhs) const
{
	if (_size != rhs._size) {
		return false;
	}

	for (unsigned int i = 0; i < _size; ++i) {
		if (_string[i] != rhs._string[i]) {
			return false;
		}
	}

	return true;
}

template <class T, class Allocator>
bool String<T, Allocator>::operator==(const T* rhs) const
{
	if (_string == rhs) {
		return true;
	}

	//if (_size != length(rhs)) {
	//	return false;
	//}

	unsigned int i = 0;
	for (; i < _size; ++i) {
		if (_string[i] != rhs[i]) {
			return false;
		}
	}

	// This is to avoid calling length(), which would make us iterate
	// over rhs twice to do a comparison. My tests show that this is
	// faster than calling length() to compare size, but not very.
	// Difference is only notable if measuring on the microsecond level.
	return (i == _size && rhs[i] == 0);
}

template <class T, class Allocator>
bool String<T, Allocator>::operator!=(const String<T, Allocator>& rhs) const
{
	return !(*this == rhs);
}

template <class T, class Allocator>
bool String<T, Allocator>::operator!=(const T* rhs) const
{
	return !(*this == rhs);
}

template <class T, class Allocator>
char String<T, Allocator>::operator[](unsigned int index) const
{
	assert(index < _size);
	return _string[index];
}

template <class T, class Allocator>
char& String<T, Allocator>::operator[](unsigned int index)
{
	assert(index < _size);
	return _string[index];
}

template <class T, class Allocator>
const String<T, Allocator>& String<T, Allocator>::operator+=(const String<T, Allocator>& rhs)
{
	unsigned int new_size = _size + rhs._size;
	T* new_string = (T*)_allocator.alloc(sizeof(T) * (new_size + 1));

	copy(_string, new_string);
	copy(rhs._string, new_string + _size);

	_size = new_size;

	_allocator.free(_string);
	_string = new_string;

	return *this;
}

template <class T, class Allocator>
const String<T, Allocator>& String<T, Allocator>::operator+=(const T* rhs)
{
	unsigned int new_size = _size + length(rhs);
	T* new_string = (T*)_allocator.alloc(sizeof(T) * (new_size + 1));

	copy(_string, new_string);
	copy(rhs, new_string + _size);

	_size = new_size;

	_allocator.free(_string);
	_string = new_string;

	return *this;
}

template <class T, class Allocator>
String<T, Allocator> String<T, Allocator>::operator+(const String<T, Allocator>& rhs) const
{
	String<T, Allocator> str(*this);
	str += rhs;
	return str;
}

template <class T, class Allocator>
String<T, Allocator> String<T, Allocator>::operator+(const T* rhs) const
{
	String<T, Allocator> str(*this);
	str += rhs;
	return str;
}

// WARNING: This function takes ownership of the string instead of copying
template <class T, class Allocator>
void String<T, Allocator>::set(T* string)
{
	clear();
	_size = length(string);
	_string = string;
}

template <class T, class Allocator>
void String<T, Allocator>::clear(void)
{
	if (_string) {
		_allocator.free(_string);
		_string = nullptr;
		_size = 0;
	}
}

template <class T, class Allocator>
unsigned int String<T, Allocator>::size(void) const
{
	return _size;
}

template <class T, class Allocator>
const T* String<T, Allocator>::getBuffer(void) const
{
	return _string;
}

template <class T, class Allocator>
T* String<T, Allocator>::getBuffer(void)
{
	return _string;
}

template <class T, class Allocator>
String<T, Allocator> String<T, Allocator>::getExtension(T delimiting_character) const
{
	assert(_string && _size);
	unsigned int index = findLastOf(delimiting_character);
	return (index == npos) ? String<T, Allocator>() : substring(index);
}

template <class T, class Allocator>
String<T, Allocator> String<T, Allocator>::substring(unsigned int begin, unsigned int end) const
{
	assert(end > begin && begin < _size && end < _size);
	return String<T, Allocator>(_string + begin, end - begin);
}

template <class T, class Allocator>
String<T, Allocator> String<T, Allocator>::substring(unsigned int begin) const
{
	assert(begin < _size);
	return String<T, Allocator>(_string + begin);
}

template <class T, class Allocator>
unsigned int String<T, Allocator>::findFirstOf(T character) const
{
	for (unsigned int i = 0; i < _size; ++i) {
		if (_string[i] == character) {
			return i;
		}
	}

	return npos;
}

template <class T, class Allocator>
unsigned int String<T, Allocator>::findLastOf(T character) const
{
	if (!_size) {
		return npos;
	}

	for (int i = (int)_size - 1; i >= 0; --i) {
		if (_string[i] == character) {
			return i;
		}
	}

	return npos;
}

// From my benchmark tests, the loop isn't any slower than calling strlen() or wcslen()
template <class T, class Allocator>
unsigned int String<T, Allocator>::length(const T* string) const
{
	unsigned int i = 0;

	while (string[i] != 0) {
		++i;
	}

	return i;
}

// If my benchmarks from strlen() and wcslen() are any indicator, this is no slower than memcpy(),
// and this gets rid of that damn compiler warning
template <class T, class Allocator>
void String<T, Allocator>::copy(const T* src, T* dest) const
{
	unsigned int i = 0;

	while (src[i] != 0) {
		dest[i] = src[i];
		++i;
	}

	dest[i] = 0;
}


template <class T, class Allocator>
bool operator==(const T* lhs, const String<T, Allocator>& rhs)
{
	return rhs == lhs;
}

template <class T, class Allocator>
bool operator!=(const T* lhs, const String<T, Allocator>& rhs)
{
	return rhs != lhs;
}

template <class T, class Allocator>
std::ostream& operator<<(std::ostream& os, const String<T, Allocator>& string)
{
	os << string.getBuffer();
	return os;
}

template <class T, class Allocator>
std::wostream& operator<<(std::wostream& os, const String<T, Allocator>& string)
{
	os << string.getBuffer();
	return os;
}

/*template <class T, class Allocator>
std::istream& operator>>(std::istream& is, String<T, Allocator>& string)
{
	return is;
}

template <class T, class Allocator>
std::wistream& operator>>(std::wistream& is, String<T, Allocator>& string)
{
	return is;
}*/

// This long and fancy version of operator+ avoids an allocation and copy
template <class T, class Allocator>
String<T, Allocator> operator+(const T* lhs, const String<T, Allocator>& rhs)
{
	unsigned int lhs_length = rhs.length(lhs);
	unsigned int new_size = rhs._size + lhs_length;
	Allocator* allocator = const_cast<Allocator*>(&rhs._allocator); // this is to get rid of compiler error from rhs._allocator being const
	T* new_string = (T*)allocator->alloc(sizeof(T) * (new_size + 1));

	rhs.copy(lhs, new_string);
	rhs.copy(rhs._string, new_string + lhs_length);

	String<T, Allocator> string(rhs._allocator);
	string._string = new_string;
	string._size = new_size;

	return string;
}