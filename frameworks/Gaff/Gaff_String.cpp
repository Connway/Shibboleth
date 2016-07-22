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

#include "Gaff_String.h"
#include <cstring>

NS_GAFF

template <>
size_t length(const char* string)
{
	return strlen(string);
}

template <>
size_t length(const wchar_t* string)
{
	return wcslen(string);
}

template <>
bool less(const char* s1, size_t, const char* s2, size_t)
{
	return strcmp(s1, s2) < 0;
}

template <>
bool less(const wchar_t* s1, size_t, const wchar_t* s2, size_t)
{
	return wcscmp(s1, s2) < 0;
}

template <>
bool less(const char* s1, size_t, const char* s2)
{
	return strcmp(s1, s2) < 0;
}

template <>
bool less(const wchar_t* s1, size_t, const wchar_t* s2)
{
	return wcscmp(s1, s2) < 0;
}

template <>
bool greater(const char* s1, size_t, const char* s2, size_t)
{
	return strcmp(s1, s2) > 0;
}

template <>
bool greater(const wchar_t* s1, size_t, const wchar_t* s2, size_t)
{
	return wcscmp(s1, s2) > 0;
}

template <>
bool greater(const char* s1, size_t, const char* s2)
{
	return strcmp(s1, s2) > 0;
}

template <>
bool greater(const wchar_t* s1, size_t, const wchar_t* s2)
{
	return wcscmp(s1, s2) > 0;
}

void ConvertToUTF8(char* output, const wchar_t* input, size_t size)
{
	utf8::utf16to8(input, input + size, output);
}

void ConvertToUTF16(wchar_t* output, const char* input, size_t size)
{
	utf8::utf8to16(input, input + size, output);
}

size_t FindInvalidUTF8(const char* string, size_t size)
{
	const char* position = utf8::find_invalid(string, string + size);
	return static_cast<size_t>(position - string);
}

bool IsValidUTF8(const char* string, size_t size)
{
	return utf8::is_valid(string, string + size);
}

NS_END
