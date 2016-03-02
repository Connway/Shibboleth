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

template <class T, class Allocator>
String<T, Allocator>::String(const Allocator& allocator):
	_string_ptr(nullptr), _size(0), _capacity(SMALL_STRING_SIZE), _allocator(allocator)
{
}

template <class T, class Allocator>
String<T, Allocator>::String(const T* string, size_t size, const Allocator& allocator):
	_string_ptr(nullptr), _size(0), _capacity(SMALL_STRING_SIZE), _allocator(allocator)
{
	if (size <= SMALL_STRING_SIZE) {
		memcpy(_string_buf, string, sizeof(T) * size);
		_string_buf[size] = 0; // set null-terminator

	} else {
		_string_ptr = reinterpret_cast<T*>(_allocator.alloc(sizeof(T) * (size + 1)));

		memcpy(_string_ptr, string, sizeof(T) * size);
		_string_ptr[size] = 0; // set null-terminator

		_capacity = size;
	}

	_size = size;
}

template <class T, class Allocator>
String<T, Allocator>::String(const T* string, const Allocator& allocator):
	_string_ptr(nullptr), _size(0), _capacity(0), _allocator(allocator)
{
	*this = string;
}

template <class T, class Allocator>
String<T, Allocator>::String(String<T, Allocator>&& rhs):
	_string_ptr(nullptr), _size(0), _capacity(SMALL_STRING_SIZE), _allocator(rhs._allocator)
{
	*this = std::move(rhs);
}

template <class T, class Allocator>
String<T, Allocator>::String(const String<T, Allocator>& string):
	_string_ptr(nullptr), _size(0), _capacity(SMALL_STRING_SIZE), _allocator(string._allocator)
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

	_size = rhs._size;
	_capacity = rhs._capacity;

	if (_size <= SMALL_STRING_SIZE) {
		memcpy(_string_buf, rhs.getBuffer(), sizeof(T) * (_size + 1));

		if (_capacity > SMALL_STRING_SIZE) {
			_allocator.free(rhs._string_ptr);
		}

		_capacity = SMALL_STRING_SIZE;

	} else {
		_string_ptr = rhs._string_ptr;
	}

	rhs._string_ptr = nullptr;
	rhs._size = 0;
	rhs._capacity = SMALL_STRING_SIZE;

	return *this;
}

template <class T, class Allocator>
const String<T, Allocator>& String<T, Allocator>::operator=(const String<T, Allocator>& rhs)
{
	if (this == &rhs) {
		return *this;
	}

	clear();

	_size = rhs._size;
	_capacity = rhs._capacity;

	if (_size <= SMALL_STRING_SIZE) {
		memcpy(_string_buf, rhs.getBuffer(), sizeof(T) * (_size + 1));

	} else {
		_string_ptr = reinterpret_cast<T*>(_allocator.alloc(sizeof(T) * (_size + 1)));
		memcpy(_string_ptr, rhs._string_ptr, sizeof(T) * (_size + 1));
	}

	return *this;
}

template <class T, class Allocator>
const String<T, Allocator>& String<T, Allocator>::operator=(const T* rhs)
{
	clear();

	_size = length(rhs);

	if (_size <= SMALL_STRING_SIZE) {
		memcpy(_string_buf, rhs, sizeof(T) * (_size + 1));

	} else {
		_string_ptr = reinterpret_cast<T*>(_allocator.alloc(sizeof(T) * (_size + 1)));
		memcpy(_string_ptr, rhs, sizeof(T) * (_size + 1));
		_capacity = _size;
	}

	return *this;
}

template <class T, class Allocator>
bool String<T, Allocator>::operator==(const String<T, Allocator>& rhs) const
{
	if (_size != rhs._size) {
		return false;
	}

	return !memcmp(getBuffer(), rhs.getBuffer(), _size);
}

template <class T, class Allocator>
bool String<T, Allocator>::operator==(const T* rhs) const
{
	//if (_size != length(rhs)) {
	//	return false;
	//}

	const T* buffer = getBuffer();
	size_t i = 0;

	for (; i < _size; ++i) {
		if (buffer[i] != rhs[i]) {
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
	return less(getBuffer(), _size, rhs.getBuffer(), rhs._size);
}

template <class T, class Allocator>
bool String<T, Allocator>::operator<(const T* rhs) const
{
	return less(getBuffer(), _size, rhs);
}

template <class T, class Allocator>
bool String<T, Allocator>::operator>(const String& rhs) const
{
	return greater(getBuffer(), _size, rhs.getBuffer(), rhs._size);
}

template <class T, class Allocator>
bool String<T, Allocator>::operator>(const T* rhs) const
{
	return greater(getBuffer(), _size, rhs);
}

template <class T, class Allocator>
const T& String<T, Allocator>::operator[](size_t index) const
{
	GAFF_ASSERT(index < _size);
	return getBuffer()[index];
}

template <class T, class Allocator>
T& String<T, Allocator>::operator[](size_t index)
{
	GAFF_ASSERT(index < _size);
	return getBuffer()[index];
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
	return *this + rhs.getBuffer();
}

template <class T, class Allocator>
String<T, Allocator> String<T, Allocator>::operator+(const T* rhs) const
{
	String<T, Allocator> string(_allocator);
	size_t rhs_length = length(rhs);
	size_t new_size = _size + rhs_length;

	string.resize(new_size);
	T* buffer = string.getBuffer();

	memcpy(buffer, getBuffer(), sizeof(T) * _size);
	memcpy(buffer + _size, rhs, sizeof(T) * (rhs_length + 1));

	return string;
}

template <class T, class Allocator>
String<T, Allocator> String<T, Allocator>::operator+(T rhs) const
{
	T temp[2] = { rhs, 0 };
	return *this + temp;
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

	if (_size <= SMALL_STRING_SIZE) {
		memcpy(_string_buf, string, sizeof(T) * (_size + 1));
		_allocator.free(string);

	} else {
		_string_ptr = string;
		_capacity = _size;
	}
}

template <class T, class Allocator>
void String<T, Allocator>::clear(void)
{
	if (_capacity > SMALL_STRING_SIZE && _string_ptr) {
		_allocator.free(_string_ptr);
		_string_ptr = nullptr;
	}

	_string_buf[0] = 0;
	_size = 0;
	_capacity = SMALL_STRING_SIZE;
}

template <class T, class Allocator>
size_t String<T, Allocator>::size(void) const
{
	return _size;
}

template <class T, class Allocator>
size_t String<T, Allocator>::capacity(void) const
{
	return _capacity;
}

template <class T, class Allocator>
const T* String<T, Allocator>::getBuffer(void) const
{
	return (_capacity > SMALL_STRING_SIZE) ? _string_ptr : _string_buf;
}

template <class T, class Allocator>
T* String<T, Allocator>::getBuffer(void)
{
	return (_capacity > SMALL_STRING_SIZE) ? _string_ptr : _string_buf;
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
	size_t index = findLastOf(delimiting_character);
	return (index == SIZE_T_FAIL) ? String<T, Allocator>() : substring(index);
}

template <class T, class Allocator>
String<T, Allocator> String<T, Allocator>::substring(size_t begin, size_t end) const
{
	GAFF_ASSERT(end > begin && begin < _size && end < _size);
	return String<T, Allocator>(getBuffer() + begin, end - begin);
}

template <class T, class Allocator>
String<T, Allocator> String<T, Allocator>::substring(size_t begin) const
{
	GAFF_ASSERT(begin < _size);
	return String<T, Allocator>(getBuffer() + begin, _size - begin);
}

template <class T, class Allocator>
void String<T, Allocator>::append(const T* string, size_t size)
{
	size_t new_size = _size + size;

	if (new_size <= SMALL_STRING_SIZE) {
		memcpy(_string_buf + _size, string, sizeof(T) * size);
		_string_buf[new_size] = 0;

	} else {
		T* new_string = reinterpret_cast<T*>(_allocator.alloc(sizeof(T) * (new_size + 1)));
		new_string[new_size] = 0;

		if (_size) {
			memcpy(new_string, getBuffer(), sizeof(T) * _size);
		}

		memcpy(new_string + _size, string, sizeof(T) * size);

		if (_size > SMALL_STRING_SIZE) {
			_allocator.free(_string_ptr);
		}

		_string_ptr = new_string;
		_capacity = new_size;
	}

	_size = new_size;
}

template <class T, class Allocator>
void String<T, Allocator>::append(const T* string)
{
	append(string, length(string));
}

template <class T, class Allocator>
void String<T, Allocator>::resize(size_t new_size)
{
	if (new_size == _size) {
		return;
	}

	size_t copy_size = (new_size < _size) ? new_size : _size;
	T* old_string = _string_ptr;

	if (new_size <= SMALL_STRING_SIZE) {
		if (_capacity > SMALL_STRING_SIZE) {
			memcpy(_string_buf, old_string, sizeof(T) * copy_size);
			_allocator.free(old_string);
		}

		_string_buf[new_size] = 0;
		_capacity = SMALL_STRING_SIZE;

	} else {
		T* new_string = old_string;

		if (_capacity < new_size) {
			new_string = reinterpret_cast<T*>(_allocator.alloc(sizeof(T) * (new_size + 1)));
			memcpy(new_string, getBuffer(), sizeof(T) * _size);

			if (_capacity > SMALL_STRING_SIZE) {
				_allocator.free(old_string);
			}

			_capacity = new_size;
		}

		new_string[new_size] = 0;
		_string_ptr = new_string;
	}

	_size = new_size;
}

template <class T, class Allocator>
void String<T, Allocator>::erase(size_t begin_index, size_t end_index)
{
	fastErase(begin_index, end_index);
	trim();
}

template <class T, class Allocator>
void String<T, Allocator>::erase(size_t index)
{
	GAFF_ASSERT(index < _size);
	fastErase(index, index + 1);
	trim();
}

template <class T, class Allocator>
void String<T, Allocator>::erase(T character)
{
	fastErase(character);
	trim();
}

template <class T, class Allocator>
void String<T, Allocator>::fastErase(size_t begin_index, size_t end_index)
{
	GAFF_ASSERT(begin_index < end_index && begin_index < _size && end_index < _size);

	size_t new_size = _size - (end_index - begin_index);
	T* old_string = _string_ptr;

	if (new_size <= SMALL_STRING_SIZE) {
		if (_capacity <= SMALL_STRING_SIZE) {
			memcpy(_string_buf + begin_index, _string_buf + end_index, sizeof(T) * (_size - end_index));

		} else {
			memcpy(_string_buf, old_string, sizeof(T) * begin_index);
			memcpy(_string_buf + begin_index, old_string + end_index, sizeof(T) * (_size - end_index));
			_allocator.free(old_string);
		}

		_string_buf[new_size] = 0;
		_capacity = SMALL_STRING_SIZE;

	} else {
		memcpy(_string_ptr + begin_index, _string_ptr + end_index, sizeof(T) * (_size - end_index));
		_string_ptr[new_size] = 0;
	}

	_size = new_size;
}

template <class T, class Allocator>
void String<T, Allocator>::fastErase(size_t index)
{
	fastErase(index, index + 1);
}

template <class T, class Allocator>
void String<T, Allocator>::fastErase(T character)
{
	size_t index = findFirstOf(character);

	while (index != SIZE_T_FAIL) {
		fastErase(index);
		index = findFirstOf(character);
	}
}

template <class T, class Allocator>
void String<T, Allocator>::trim(void)
{
	if (_capacity > SMALL_STRING_SIZE && _size != _capacity) {
		T* old_string = _string_ptr;

		_string_ptr = reinterpret_cast<T*>(_allocator.alloc(sizeof(T) * (_size + 1)));
		memcpy(_string_ptr, old_string, sizeof(T) * (_size + 1));

		_allocator.free(old_string);
	}
}

template <class T, class Allocator>
size_t String<T, Allocator>::findFirstOf(const T* string) const
{
	size_t len = length(string);

	if (_size < len) {
		return SIZE_T_FAIL;
	}

	size_t num_iterations = _size - len + 1;
	const T* buffer = getBuffer();

	for (size_t i = 0; i < num_iterations; ++i) {
		if (!memcmp(buffer + i, string, sizeof(T) * len)) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

template <class T, class Allocator>
size_t String<T, Allocator>::findLastOf(const T* string) const
{
	size_t len = length(string);

	if (_size < len) {
		return SIZE_T_FAIL;
	}

	const T* buffer = getBuffer();

	for (int i = static_cast<int>(_size) - static_cast<int>(len) - 2; i >= 0; --i) {
		if (!memcmp(buffer + i, string, sizeof(T) * len)) {
			return static_cast<size_t>(i);
		}
	}

	return SIZE_T_FAIL;
}

template <class T, class Allocator>
size_t String<T, Allocator>::findFirstOf(T character) const
{
	const T* buffer = getBuffer();

	for (size_t i = 0; i < _size; ++i) {
		if (buffer[i] == character) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

template <class T, class Allocator>
size_t String<T, Allocator>::findLastOf(T character) const
{
	const T* buffer = getBuffer();

	for (int i = static_cast<int>(_size) - 1; i >= 0; --i) {
		if (buffer[i] == character) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

/*!
	\brief Converts a UTF-16 encoded string to UTF-8 encoding.
*/
template <class T, class Allocator>
void String<T, Allocator>::convertToUTF8(const wchar_t* string, size_t size)
{
	resize(size * 4); // If somehow every character generates 4 octets
	T* buffer = getBuffer();

	memset(buffer, 0, sizeof(T) * _size);
	ConvertToUTF8(buffer, string, size);

	size_t new_size = findFirstOf(static_cast<T>(0));
	_size = (new_size != SIZE_T_FAIL) ? new_size : _size;
}

/*!
	\brief Converts a UTF-8 encoded string to UTF-16 encoding.
*/
template <class T, class Allocator>
void String<T, Allocator>::convertToUTF16(const char* string, size_t size)
{
	resize(size * 2); // If somehow every character generates surrogate pairs
	T* buffer = getBuffer();

	memset(buffer, 0, sizeof(T) * _size);
	ConvertToUTF16(buffer, string, size);

	size_t new_size = findFirstOf(static_cast<T>(0));
	_size = (new_size != SIZE_T_FAIL) ? new_size : _size;
}

/*!
	\brief Find an invalid UTF-8 encoded character and returns its position.
	\return The position of the first invalid UTF-8 encoded character.
*/
template <class T, class Allocator>
size_t String<T, Allocator>::findInvalidUTF8(void) const
{
	return FindInvalidUTF8(getBuffer(), _size);
}

/*!
	\brief Checks if the string is a valid UTF-8 encoded string.
*/
template <class T, class Allocator>
bool String<T, Allocator>::isValidUTF8(void) const
{
	return IsValidUTF8(getBuffer(), _size);
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
String<T, Allocator> operator+(const T* lhs, const String<T, Allocator>& rhs)
{
	String<T, Allocator> string(rhs._allocator);
	size_t lhs_length = length(lhs);
	size_t new_size = rhs._size + lhs_length;

	string.resize(new_size);
	T* buffer = string.getBuffer();

	memcpy(buffer, lhs, sizeof(T) * lhs_length);
	memcpy(buffer + lhs_length, rhs.getBuffer(), sizeof(T) * (rhs.size() + 1));

	return string;
}

template <class T>
size_t length(const T* string)
{
	size_t i = 0;

	while (string[i] != 0) {
		++i;
	}

	return i;
}

template <class T>
bool less(const T* s1, size_t n1, const T* s2, size_t n2)
{
	size_t size = (n1 < n2) ? n1 : n2;

	for (size_t i = 0; i < size; ++i) {
		if (s1[i] < s2[i]) {
			return true;
		} else if (s1[i] > s2[i]) {
			return false;
		}
	}

	return false;
}

template <class T>
bool less(const T* s1, size_t n1, const T* s2)
{
	return less(s1, n1, s2, length(s2));
}

template <class T>
bool greater(const T* s1, size_t n1, const T* s2, size_t n2)
{
	size_t size = (n1 < n2) ? n1 : n2;

	for (size_t i = 0; i < size; ++i) {
		if (s1[i] > s2[i]) {
			return true;
		} else if (s1[i] < s2[i]) {
			return false;
		}
	}

	return false;
}

template <class T>
bool greater(const T* s1, size_t n1, const T* s2)
{
	return greater(s1, n1, s2, length(s2));
}
