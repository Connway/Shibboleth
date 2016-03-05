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

#include "Gaff_DefaultAllocator.h"
#include "Gaff_Assert.h"
#include <utf8.h>

#define SMALL_STRING_SIZE 15

NS_GAFF

/*!
	\brief A simple string implementation.
	\param T The underlying data type of the string. Traditionally char or wchar_t.
	\Allocator The allocator type to be used for memory operations.
	\note Does not support iterators. Values accessed via indices only.
*/
template <class T, class Allocator = DefaultAllocator>
class String
{
public:
	String(const Allocator& allocator = Allocator());
	String(const T* string, size_t size, const Allocator& allocator = Allocator());
	explicit String(const T* string, const Allocator& allocator = Allocator());
	String(String<T, Allocator>&& rhs);
	String(const String<T, Allocator>& string);
	~String(void);

	const String<T, Allocator>& operator=(String<T, Allocator>&& rhs);
	const String<T, Allocator>& operator=(const String<T, Allocator>& rhs);
	const String<T, Allocator>& operator=(const T* rhs);

	bool operator==(const String<T, Allocator>& rhs) const;
	bool operator==(const T* rhs) const;
	bool operator!=(const String<T, Allocator>& rhs) const;
	bool operator!=(const T* rhs) const;

	bool operator<(const String<T, Allocator>& rhs) const;
	bool operator<(const T* rhs) const;
	bool operator>(const String<T, Allocator>& rhs) const;
	bool operator>(const T* rhs) const;

	const T& operator[](size_t index) const;
	T& operator[](size_t index);

	const String<T, Allocator>& operator+=(const String<T, Allocator>& rhs);
	const String<T, Allocator>& operator+=(const T* rhs);
	const String<T, Allocator>& operator+=(T rhs);

	String<T, Allocator> operator+(const String<T, Allocator>& rhs) const;
	String<T, Allocator> operator+(const T* rhs) const;
	String<T, Allocator> operator+(T rhs) const;

	// WARNING: This function takes ownership of the string instead of copying
	void set(T* string);
	void clear(void);
	size_t size(void) const;
	size_t capacity(void) const;

	const T* getBuffer(void) const;
	T* getBuffer(void);

	String<T, Allocator> getExtension(T delimiting_character) const;
	String<T, Allocator> substring(size_t begin, size_t end) const;
	String<T, Allocator> substring(size_t begin) const;

	void append(const T* string, size_t size);
	void append(const T* string);
	void resize(size_t new_size);

	void erase(size_t begin_index, size_t end_index);
	void erase(size_t index);
	void erase(T character);

	void fastErase(size_t begin_index, size_t end_index);
	void fastErase(size_t index);
	void fastErase(T character);

	void trim(void);

	size_t findFirstOf(const T* string, size_t size) const;
	size_t findFirstOf(const T* string) const;
	size_t findLastOf(const T* string, size_t size) const;
	size_t findLastOf(const T* string) const;
	size_t findFirstOf(T character) const;
	size_t findLastOf(T character) const;

	// Only valid on String with type char. Conversion functions will erase current string.
	void convertToUTF8(const wchar_t* string, size_t size);
	void convertToUTF16(const char* string, size_t size);
	size_t findInvalidUTF8(void) const;
	bool isValidUTF8(void) const;

private:
	union
	{
		T* _string_ptr;
		T _string_buf[SMALL_STRING_SIZE + 1];
	};

	size_t _size;
	size_t _capacity;
	Allocator _allocator;

	// allows for optimization
	template <class T2, class Allocator2>
	friend String<T2, Allocator2> operator+(const T2* lhs, const String<T2, Allocator2>& rhs);
};

// Helper Functions
template <class T>
size_t length(const T* string);

template <>
size_t length(const char* string);

template <>
size_t length(const wchar_t* string);

template <class T>
bool less(const T* s1, size_t n1, const T* s2, size_t n2);

template <class T>
bool less(const T* s1, size_t n1, const T* s2);

template <>
bool less(const char* s1, size_t n1, const char* s2, size_t n2);

template <>
bool less(const wchar_t* s1, size_t n1, const wchar_t* s2, size_t n2);

template <class T>
bool greater(const T* s1, size_t n1, const T* s2, size_t n2);

template <>
bool greater(const char* s1, size_t n1, const char* s2, size_t n2);

template <>
bool greater(const wchar_t* s1, size_t n1, const wchar_t* s2, size_t n2);

template <class T>
bool greater(const T* s1, size_t n1, const T* s2);

template <>
bool greater(const char* s1, size_t n1, const char* s2);

template <>
bool greater(const wchar_t* s1, size_t n1, const wchar_t* s2);

template <class T>
size_t FindFirstOf(const T* string, size_t size1, const T* substr, size_t substr_size);

template <class T>
size_t FindFirstOf(const T* string, size_t size, const T* substr);

template <class T>
size_t FindFirstOf(const T* string, const T* substr);

template <class T>
size_t FindLastOf(const T* string, size_t size1, const T* substr, size_t substr_size);

template <class T>
size_t FindLastOf(const T* string, size_t size, const T* substr);

template <class T>
size_t FindLastOf(const T* string, const T* substr);

template <class T>
size_t FindFirstOf(const T* string, size_t size, T character);

template <class T>
size_t FindLastOf(const T* string, size_t size, T character);


INLINE void ConvertToUTF8(char* output, const wchar_t* input, size_t size);
INLINE void ConvertToUTF16(wchar_t* output, const char* input, size_t size); // size is in bytes, not character count
INLINE size_t FindInvalidUTF8(const char* string, size_t size);
INLINE bool IsValidUTF8(const char* string, size_t size);

#define STRING_CONVERSION_BUFFER_SIZE 256
#define UTF8_STRLEN(string) mbstowcs(NULL, string, 0)
#define UTF8_BYTELEN(string) strlen(string)

#define CONVERT_TO_UTF8(TempBufferName, string) \
	char TempBufferName[STRING_CONVERSION_BUFFER_SIZE] = { 0 }; \
	Gaff::ConvertToUTF16(TempBufferName, string, wcslen(string))

#define CONVERT_TO_UTF16(TempBufferName, string) \
	wchar_t TempBufferName[STRING_CONVERSION_BUFFER_SIZE] = { 0 }; \
	Gaff::ConvertToUTF16(TempBufferName, string, strlen(string))


#include "Gaff_String.inl"

template <class Allocator = DefaultAllocator> using AString = String<char, Allocator>;
template <class Allocator = DefaultAllocator> using WString = String<wchar_t, Allocator>;

NS_END
