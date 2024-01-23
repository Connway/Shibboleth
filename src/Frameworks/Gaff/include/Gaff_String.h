/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#ifndef EASTL_BASIC_STRING_DEFAULT_NAME
	#define EASTL_BASIC_STRING_DEFAULT_NAME "Untagged"
#endif

#include "Gaff_DefaultAllocator.h"
#include "Gaff_Assert.h"

GCC_CLANG_DISABLE_WARNING_PUSH("-Wunused-parameter")
#include <EASTL/string.h>
GCC_CLANG_DISABLE_WARNING_POP()

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

template <class T>
size_t ReverseFind(const T* string, size_t string_size, const T* substring, size_t substring_size)
{
	const T* const pos = eastl::CharTypeStringRSearch(string, string + string_size, substring, substring + substring_size);
	return (pos == (string + string_size)) ? GAFF_SIZE_T_FAIL : eastl::distance(string, pos);
}

template <class T>
size_t ReverseFind(const T* string, size_t string_size, const T* substring)
{
	return ReverseFind(string, string_size, substring, eastl::CharStrlen(substring));
}

template <class T>
size_t ReverseFind(const T* string, const T* substring, size_t substring_size)
{
	return ReverseFind(string, eastl::CharStrlen(string), substring, substring_size);
}

template <class T>
size_t ReverseFind(const T* string, const T* substring)
{
	return ReverseFind(string, eastl::CharStrlen(string), substring, eastl::CharStrlen(substring));
}

template <class T>
size_t ReverseFind(const T* string, size_t string_size, T character)
{
	const T* const pos = eastl::CharTypeStringRFind(string + string_size, string, character);
	return (pos == (string + string_size)) ? GAFF_SIZE_T_FAIL : eastl::distance(string, pos - 1);
}

template <class T>
size_t ReverseFind(const T* string, T character)
{
	return ReverseFind(string, eastl::CharStrlen(string), character);
}

template <class T>
size_t Find(const T* string, size_t string_size, const T* substring, size_t substring_size)
{
	const T* const pos = eastl::search(string, string + string_size, substring, substring + substring_size);
	return (pos == (string + string_size)) ? GAFF_SIZE_T_FAIL : eastl::distance(string, pos);
}

template <class T>
size_t Find(const T* string, size_t string_size, const T* substring)
{
	return Find(string, string_size, substring, eastl::CharStrlen(substring));
}

template <class T>
size_t Find(const T* string, const T* substring, size_t substring_size)
{
	return Find(string, eastl::CharStrlen(string), substring, substring_size);
}

template <class T>
size_t Find(const T* string, const T* substring)
{
	return Find(string, eastl::CharStrlen(string), substring, eastl::CharStrlen(substring));
}

template <class T>
size_t Find(const T* string, size_t string_size, T character)
{
	const T* const pos = eastl::find(string, string + string_size, character);
	return (pos == (string + string_size)) ? GAFF_SIZE_T_FAIL : eastl::distance(string, pos);
}

template <class T>
size_t Find(const T* string, T character)
{
	return Find(string, eastl::CharStrlen(string), character);
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
	size_t index = string.find(substring, 0, size);

	while (index != String<T>::npos) {
		string.erase(index, size);
		index = string.find(substring, index, size);
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
	size_t index = string.find(character);

	while (index != String<T>::npos) {
		string.erase(index, 1);
		index = string.find(character);
	}
}

template <class To, class From>
/*constexpr*/ To ConvertChar(From character)
{
	To converted_character[7] = { 0 };

	const From* from_start = &character;
	To* start = converted_character;

	eastl::DecodePart(from_start, from_start + 1, start, start + std::size(converted_character));

	GAFF_ASSERT(converted_character[1] == 0);

	// This is obviously not going to work for converting from characters that are more than 1 of sizeof(To) bytes.
	return converted_character[0];
}

NS_END

#define STRING_CONVERSION_BUFFER_SIZE 256

#define CONVERT_STRING_ARRAY(ToType, TempBufferName, string) \
	ToType TempBufferName[std::size(string)] = { 0 }; \
	{ \
		auto* str_start = string; \
		ToType* TempBufferName##_begin = TempBufferName; \
		ToType* TempBufferName##_end = TempBufferName + std::size(string); \
		eastl::DecodePart(str_start, str_start + std::size(string), TempBufferName##_begin, TempBufferName##_end); \
	}

#define CONVERT_STRING(ToType, TempBufferName, string) \
	ToType TempBufferName[STRING_CONVERSION_BUFFER_SIZE] = { 0 }; \
	{ \
		auto* str_start = string; \
		ToType* TempBufferName##_begin = TempBufferName; \
		ToType* TempBufferName##_end = TempBufferName + STRING_CONVERSION_BUFFER_SIZE; \
		eastl::DecodePart(str_start, str_start + eastl::CharStrlen(string), TempBufferName##_begin, TempBufferName##_end); \
	}
