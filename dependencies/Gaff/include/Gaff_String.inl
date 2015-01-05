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
bool String<T, Allocator>::operator<(const String& rhs) const
{
	return less(_string, _size, rhs._string, rhs._size);
}

template <class T, class Allocator>
bool String<T, Allocator>::operator<(const T* rhs) const
{
	return less(_string, _size, rhs);
}

template <class T, class Allocator>
bool String<T, Allocator>::operator>(const String& rhs) const
{
	return greater(_string, _size, rhs._string, rhs._size);
}

template <class T, class Allocator>
bool String<T, Allocator>::operator>(const T* rhs) const
{
	return greater(_string, _size, rhs);
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
	append(rhs.getBuffer(), rhs.size());
	return *this;
}

template <class T, class Allocator>
const String<T, Allocator>& String<T, Allocator>::operator+=(const T* rhs)
{
	append(rhs);
	return *this;
}

template <class T, class Allocator>
const String<T, Allocator>& String<T, Allocator>::operator+=(T rhs)
{
	append(&rhs, 1);
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

template <class T, class Allocator>
String<T, Allocator> String<T, Allocator>::operator+(T rhs) const
{
	String<T, Allocator> str(*this);
	str += rhs;
	return str;
}

// WARNING: This function takes ownership of the string instead of copying
/*!
	\brief Sets the raw string to be used internally. Does not allocate to make a copy.
	\param string The raw string to use.
	\note Using this function means you are passing ownership of \a string to String.
*/
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

/*!
	\brief Returns the substring of the extension denoted by the \a delimiting_character.

	\return
		The substring of all characters that occurs after the last instance of \a delimiting_character.
		If \a delimiting_character was not found, it returns and empty string.

	\note This is the same as calling substring(findLastOf(\a delimiting_character)). (but with some error checking)
*/
template <class T, class Allocator>
String<T, Allocator> String<T, Allocator>::getExtension(T delimiting_character) const
{
	assert(_string && _size);
	unsigned int index = findLastOf(delimiting_character);
	return (index == UINT_FAIL) ? String<T, Allocator>() : substring(index);
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
	return String<T, Allocator>(_string + begin, _size - begin);
}

template <class T, class Allocator>
void String<T, Allocator>::append(const T* string, unsigned int size)
{
	unsigned int new_size = _size + size;
	T* new_string = (T*)_allocator.alloc(sizeof(T) * (new_size + 1));
	new_string[new_size] = 0;

	if (_string) {
		copy(_string, new_string);
	}

	copy(string, new_string + _size, size);

	_size = new_size;

	if (_string) {
		_allocator.free(_string);
	}

	_string = new_string;
}

template <class T, class Allocator>
void String<T, Allocator>::append(const T* string)
{
	append(string, length(string));
}

template <class T, class Allocator>
void String<T, Allocator>::resize(unsigned int new_size)
{
	if (new_size == _size) {
		return;
	}

	unsigned int copy_size = (new_size < _size) ? new_size : _size;
	T* new_string = (T*)_allocator.alloc(sizeof(T) * (new_size + 1));

	if (_string) {
		copy(_string, new_string, copy_size);
	}

	zeroOut(new_string + copy_size, new_size + 1 - copy_size);

	_size = new_size;

	_allocator.free(_string);
	_string = new_string;
}

template <class T, class Allocator>
void String<T, Allocator>::erase(unsigned int begin_index, unsigned int end_index)
{
	assert(begin_index < end_index && begin_index < _size && end_index < _size);
	copy(_string + end_index, _string + begin_index);
	_size -= end_index - begin_index;
}

template <class T, class Allocator>
void String<T, Allocator>::erase(unsigned int index)
{
	assert(index < _size);
	erase(index, index + 1);
}

template <class T, class Allocator>
void String<T, Allocator>::erase(T character)
{
	for (unsigned int i = 0; i < _size; ++i) {
		if (_string[i] == character) {
			// Shift all characters above the erased character down one
			for (unsigned int j = i; j < _size; ++j) {
				_string[j] = _string[j +1];
				_string[j + 1] = 0;
			}

			--_size;
		}
	}
}

template <class T, class Allocator>
unsigned int String<T, Allocator>::findFirstOf(const T* string) const
{
	unsigned int len = length(string);

	if (_size < len) {
		return UINT_FAIL;
	}

	unsigned int num_iterations = _size - len + 1;

	for (unsigned int i = 0; i < num_iterations; ++i) {
		if (equal(_string + i, string, len)) {
			return i;
		}
	}

	return UINT_FAIL;
}

template <class T, class Allocator>
unsigned int String<T, Allocator>::findLastOf(const T* string) const
{
	unsigned int len = length(string);

	if (_size < len) {
		return UINT_FAIL;
	}

	for (int i = (int)_size - len - 2; i >= 0; --i) {
		if (equal(_string + i, string, len)) {
			return (unsigned int)i;
		}
	}

	return UINT_FAIL;
}
template <class T, class Allocator>
unsigned int String<T, Allocator>::findFirstOf(T character) const
{
	for (unsigned int i = 0; i < _size; ++i) {
		if (_string[i] == character) {
			return i;
		}
	}

	return UINT_FAIL;
}

template <class T, class Allocator>
unsigned int String<T, Allocator>::findLastOf(T character) const
{
	for (int i = (int)_size - 1; i >= 0; --i) {
		if (_string[i] == character) {
			return i;
		}
	}

	return UINT_FAIL;
}

/*!
	\brief Converts a UTF-16 encoded string to UTF-8 encoding.
*/
template <class T, class Allocator>
void String<T, Allocator>::convertToUTF8(const wchar_t* string, unsigned int size)
{
	resize(size * 4); // If somehow every character generates 4 octets
	utf8::utf16to8(string, string + size, _string);
	trimZeroes(); // Make string exact size
}

/*!
	\brief Converts a UTF-8 encoded string to UTF-16 encoding.
*/
template <class T, class Allocator>
void String<T, Allocator>::convertToUTF16(const char* string, unsigned int size)
{
	resize(size * 2); // If somehow every character generates surrogate pairs
	utf8::utf8to16(string, string + size, _string);
	trimZeroes(); // Make string exact size
}

/*!
	\brief Find an invalid UTF-8 encoded character and returns its position.
	\return The position of the first invalid UTF-8 encoded character.
*/
template <class T, class Allocator>
unsigned int String<T, Allocator>::findInvalidUTF8(void) const
{
	char* position = utf8::find_invalid(_string, _string + _size);
	return (unsigned int)(position - _string);
}

/*!
	\brief Checks if the string is a valid UTF-8 encoded string.
*/
template <class T, class Allocator>
bool String<T, Allocator>::isValidUTF8(void) const
{
	return utf8::is_valid(_string, _string + _size);
}


// If my benchmarks from strlen() and wcslen() are any indicator, this is no slower than memcpy(),
// and this gets rid of that damn compiler warning
template <class T, class Allocator>
void String<T, Allocator>::copy(const T* src, T* dest, unsigned int dest_size) const
{
	unsigned int i = 0;

	while (src[i] != 0 && i < dest_size) {
		dest[i] = src[i];
		++i;
	}

	dest[i] = 0;
}

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
void String<T, Allocator>::zeroOut(T* string, unsigned int size) const
{
	for (unsigned int i = 0; i < size; ++i) {
		string[i] = 0;
	}
}

template <class T, class Allocator>
void String<T, Allocator>::trimZeroes(void)
{
	for (unsigned int i = 0; i < _size; ++i) {
		if (_string[i] == 0) {
			resize(i);
			break;
		}
	}
}

template <class T, class Allocator>
bool String<T, Allocator>::equal(const T* str1, const T* str2, unsigned int size) const
{
	for (unsigned int i = 0; i < size; ++i) {
		if (str1[i] != str2[i]) {
			return false;
		}
	}

	return true;
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

// This long and fancy version of operator+ avoids an allocation and copy
template <class T, class Allocator>
String<T, Allocator> operator+(const T* lhs, const String<T, Allocator>& rhs)
{
	unsigned int lhs_length = length(lhs);
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

template <class T>
unsigned int length(const T* string)
{
	unsigned int i = 0;

	while (string[i] != 0) {
		++i;
	}

	return i;
}

template <class T>
bool less(const T* s1, unsigned int n1, const T* s2, unsigned int n2)
{
	unsigned int size = (n1 < n2) ? n1 : n2;

	for (unsigned int i = 0; i < size; ++i) {
		if (s1[i] < s2[i]) {
			return true;
		} else if (s1[i] > s2[i]) {
			return false;
		}
	}

	return false;
}

template <class T>
bool less(const T* s1, unsigned int n1, const T* s2)
{
	return less(s1, n1, s2, length(s2));
}

template <class T>
bool greater(const T* s1, unsigned int n1, const T* s2, unsigned int n2)
{
	unsigned int size = (n1 < n2) ? n1 : n2;

	for (unsigned int i = 0; i < size; ++i) {
		if (s1[i] > s2[i]) {
			return true;
		} else if (s1[i] < s2[i]) {
			return false;
		}
	}

	return false;
}

template <class T>
bool greater(const T* s1, unsigned int n1, const T* s2)
{
	return greater(s1, n1, s2, length(s2));
}
