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

#pragma once

#include "Gaff_DefaultAllocator.h"
#include "Gaff_IncludeAssert.h"

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
class String
{
public:
	static const unsigned int npos;

	String(const Allocator& allocator = Allocator());
	String(const T* string, unsigned int size, const Allocator& allocator = Allocator());
	String(const T* string, const Allocator& allocator = Allocator());
	String(String<T, Allocator>&& rhs);
	String(const String<T, Allocator>& string);
	~String(void);

	const String<T, Allocator>& operator=(String<T, Allocator>&& rhs);
	const String<T, Allocator>& operator=(const String& rhs);
	const String<T, Allocator>& operator=(const T* rhs);

	bool operator==(const String& rhs) const;
	bool operator==(const T* rhs) const;
	bool operator!=(const String& rhs) const;
	bool operator!=(const T* rhs) const;

	bool operator<(const String& rhs) const;
	bool operator<(const T* rhs) const;
	bool operator>(const String& rhs) const;
	bool operator>(const T* rhs) const;

	char operator[](unsigned int index) const;
	// required reference for operations like string[i] = 'a';
	char& operator[](unsigned int index);

	const String<T, Allocator>& operator+=(const String<T, Allocator>& rhs);
	const String<T, Allocator>& operator+=(const T* rhs);

	String<T, Allocator> operator+(const String<T, Allocator>& rhs) const;
	String<T, Allocator> operator+(const T* rhs) const;

	// WARNING: This function takes ownership of the string instead of copying
	void set(T* string);
	void clear(void);
	unsigned int size(void) const;

	const T* getBuffer(void) const;
	T* getBuffer(void);

	String<T, Allocator> getExtension(T delimiting_character) const;
	String<T, Allocator> substring(unsigned int begin, unsigned int end) const;
	String<T, Allocator> substring(unsigned int begin) const;

	unsigned int findFirstOf(T character) const;
	unsigned int findLastOf(T character) const;

private:
	Allocator _allocator;
	unsigned int _size;
	T* _string;

	// If my benchmarks from strlen() and wcslen() are any indicator, this is no slower than memcpy(),
	// and this gets rid of that damn compiler warning
	void copy(const T* src, T* dest) const;

	// allows for optimization
	template <class T2, class Allocator2>
	friend String<T2, Allocator2> operator+(const T2* lhs, const String<T2, Allocator2>& rhs);
};

// Helper Functions
template <class T>
unsigned int length(const T* string);

template <>
unsigned int length(const char* string);

template <>
unsigned int length(const wchar_t* string);

template <class T>
bool less(const T* s1, unsigned int n1, const T* s2, unsigned int n2);

template <class T>
bool less(const T* s1, unsigned int n1, const T* s2);

template <>
bool less(const char* s1, unsigned int n1, const char* s2, unsigned int n2);

template <>
bool less(const wchar_t* s1, unsigned int n1, const wchar_t* s2, unsigned int n2);

template <class T>
bool greater(const T* s1, unsigned int n1, const T* s2, unsigned int n2);

template <>
bool greater(const char* s1, unsigned int n1, const char* s2, unsigned int n2);

template <>
bool greater(const wchar_t* s1, unsigned int n1, const wchar_t* s2, unsigned int n2);

template <class T>
bool greater(const T* s1, unsigned int n1, const T* s2);

template <>
bool greater(const char* s1, unsigned int n1, const char* s2);

template <>
bool greater(const wchar_t* s1, unsigned int n1, const wchar_t* s2);

#include "Gaff_String.inl"

template <class Allocator = DefaultAllocator> using AString = String<char, Allocator>;
template <class Allocator = DefaultAllocator> using WString = String<wchar_t, Allocator>;
template <class Allocator = DefaultAllocator> using GString = String<GChar, Allocator>;

NS_END
