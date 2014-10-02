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

#include "Gaff_String.h"
#include <cstring>

NS_GAFF

template <>
unsigned int length(const char* string)
{
	return (unsigned int)strlen(string);
}

template <>
unsigned int length(const wchar_t* string)
{
	return (unsigned int)wcslen(string);
}

template <>
bool less(const char* s1, unsigned int, const char* s2, unsigned int)
{
	return strcmp(s1, s2) < 0;
}

template <>
bool less(const wchar_t* s1, unsigned int, const wchar_t* s2, unsigned int)
{
	return wcscmp(s1, s2) < 0;
}

template <>
bool less(const char* s1, unsigned int, const char* s2)
{
	return strcmp(s1, s2) < 0;
}

template <>
bool less(const wchar_t* s1, unsigned int, const wchar_t* s2)
{
	return wcscmp(s1, s2) < 0;
}

template <>
bool greater(const char* s1, unsigned int, const char* s2, unsigned int)
{
	return strcmp(s1, s2) > 0;
}

template <>
bool greater(const wchar_t* s1, unsigned int, const wchar_t* s2, unsigned int)
{
	return wcscmp(s1, s2) > 0;
}

template <>
bool greater(const char* s1, unsigned int, const char* s2)
{
	return strcmp(s1, s2) > 0;
}

template <>
bool greater(const wchar_t* s1, unsigned int, const wchar_t* s2)
{
	return wcscmp(s1, s2) > 0;
}

NS_END
