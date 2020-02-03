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

#pragma once

#include "Gaff_DefaultAllocator.h"
#include <Gaff_Assert.h>
#include <EASTL/string.h>

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
using String = eastl::basic_string<T, Allocator>;

template <class Allocator = DefaultAllocator>
using U8String = eastl::basic_string<char8_t, Allocator>;

template <class Allocator = DefaultAllocator>
using U16String = eastl::basic_string<char16_t, Allocator>;

template <class Allocator = DefaultAllocator>
using U32String = eastl::basic_string<char32_t, Allocator>;


template <class T>
using StringView = eastl::basic_string_view<T>;

using U8StringView = eastl::u8string_view;
using U16StringView = eastl::u16string_view;
using U32StringView = eastl::u32string_view;

// Helper Functions
template <class T>
size_t FindFirstOf(const T* string, size_t str_size, const T* substr, size_t substr_size)
{
	if (str_size < substr_size) {
		return SIZE_T_FAIL;
	}

	size_t num_iterations = str_size - substr_size + 1;

	for (size_t i = 0; i < num_iterations; ++i) {
		if (!memcmp(string + i, substr, sizeof(T) * substr_size)) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

template <class T>
size_t FindFirstOf(const T* string, size_t str_size, const T* substr)
{
	return FindFirstOf(string, str_size, substr, eastl::CharStrlen(substr));
}

template <class T>
size_t FindFirstOf(const T* string, const T* substr)
{
	return FindFirstOf(string, eastl::CharStrlen(string), substr, eastl::CharStrlen(substr));
}

template <class T>
size_t FindFirstOf(const T* string, size_t size, T character)
{
	for (size_t i = 0; i < size; ++i) {
		if (string[i] == character) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

template <class T>
size_t FindFirstOf(const T* string, T character)
{
	return FindFirstOf(string, eastl::CharStrlen(string), character);
}

template <class T>
size_t FindLastOf(const T* string, size_t str_size, const T* substr, size_t substr_size)
{
	if (str_size < substr_size) {
		return SIZE_T_FAIL;
	}

	for (int i = static_cast<int>(str_size) - static_cast<int>(substr_size); i > -1; --i) {
		if (!memcmp(string + i, substr, sizeof(T) * substr_size)) {
			return static_cast<size_t>(i);
		}
	}

	return SIZE_T_FAIL;
}

template <class T>
size_t FindLastOf(const T* string, size_t str_size, const T* substr)
{
	return FindLastOf(string, str_size, substr, eastl::CharStrlen(substr));
}

template <class T>
size_t FindLastOf(const T* string, const T* substr)
{
	return FindLastOf(string, eastl::CharStrlen(string), substr, eastl::CharStrlen(substr));
}

template <class T>
size_t FindLastOf(const T* string, size_t size, T character)
{
	for (int i = static_cast<int>(size) - 1; i > -1; --i) {
		if (string[i] == character) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

template <class T>
size_t FindLastOf(const T* string, T character)
{
	return FindLastOf(string, eastl::CharStrlen(string), character);
}

template <class T>
bool EndsWith(const T* string, size_t string_size, const T* end_string, size_t end_string_size)
{
	GAFF_ASSERT(string && end_string);
	return string_size >= end_string_size && !eastl::Compare(string + string_size - end_string_size, end_string, end_string_size);
}

template <class T>
bool EndsWith(const T* string, size_t string_size, const T* end_string)
{
	const size_t end_string_size = eastl::CharStrlen(end_string);
	GAFF_ASSERT(string && end_string && end_string_size);
	return EndsWith(string, string_size, end_string, end_string_size);
}

template <class T>
bool EndsWith(const T* string, const T* end_string, size_t end_string_size)
{
	const size_t string_size = eastl::CharStrlen(string);
	GAFF_ASSERT(string && end_string && string_size);
	return EndsWith(string, string_size, end_string, end_string_size);
}

template <class T>
bool EndsWith(const T* string, const T* end_string)
{
	const size_t string_size = eastl::CharStrlen(string);
	const size_t end_string_size = eastl::CharStrlen(end_string);
	GAFF_ASSERT(string && end_string && string_size && end_string_size);
	return EndsWith(string, string_size, end_string, end_string_size);
}

template <class T, class Allocator>
void EraseAllOccurences(String<T, Allocator>& string, const T* substring, size_t size)
{
	size_t index = string.find_first_of(substring, 0, size);

	while (index != String<T>::npos) {
		string.erase(index, size);
		index = string.find_first_of(substring, index, size);
	}
}

template <class T, class Allocator>
void EraseAllOccurences(String<T, Allocator>& string, const T* substring)
{
	EraseAllOccurences(string, substring, eastl::CharStrlen(substring));
}

template <class T, class Allocator>
void EraseAllOccurences(String<T, Allocator>& string, T character)
{
	size_t index = string.find_first_of(character);

	while (index != String<T>::npos) {
		string.erase(index, 1);
		index = string.find_first_of(character);
	}
}

NS_END

#define STRING_CONVERSION_BUFFER_SIZE 256

#define CONVERT_STRING(ToType, TempBufferName, string) \
	ToType TempBufferName[STRING_CONVERSION_BUFFER_SIZE] = { 0 }; \
	{ \
		ToType* TempBufferName##_begin = TempBufferName; \
		ToType* TempBufferName##_end = TempBufferName + STRING_CONVERSION_BUFFER_SIZE; \
		eastl::DecodePart(string, string + eastl::CharStrlen(string), TempBufferName##_begin, TempBufferName##_end); \
	}
