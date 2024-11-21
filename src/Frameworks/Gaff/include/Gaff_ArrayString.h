/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#include "Gaff_IncludeEASTLStringView.h"
#include "Gaff_IncludeEASTLArray.h"
#include "Gaff_Defines.h"

NS_GAFF

template <class T, size_t size>
class ArrayString final
{
public:
	constexpr ArrayString(const ArrayString<T, size>& str):
		data()
	{
		for (size_t i = 0; i < size; ++i) {
			data.mValue[i] = str.data.mValue[i];
		}
	}

	explicit constexpr ArrayString(const T(&str)[size]):
		data()
	{
		for (size_t i = 0; i < size; ++i) {
			data.mValue[i] = str[i];
		}
	}

	constexpr ArrayString(void):
		data()
	{
		data.mValue[0] = 0;
		data.mValue[size > 0 ? size - 1 : 1] = 0;
	}

	// size + rhs_size - 1 because we assume size includes the null terminator.
	template <size_t rhs_size>
	constexpr ArrayString<T, size + rhs_size - 1> operator+(const ArrayString<T, rhs_size>& rhs) const
	{
		ArrayString<T, size + rhs_size - 1> new_str;

		for (size_t i = 0; i < (size - 1); ++i) {
			new_str.data.mValue[i] = data.mValue[i];
		}

		for (size_t i = 0; i < rhs_size; ++i) {
			new_str.data.mValue[size + i - 1] = rhs.data.mValue[i];
		}

		return new_str;
	}

	eastl::array<T, size> data;
};

template <size_t size>
constexpr ArrayString<char8_t, size> MakeArrayString(const char8_t (&str)[size])
{
	return ArrayString<char8_t, size>(str);
}

template <size_t size>
constexpr ArrayString<char, size> MakeArrayString(const char (&str)[size])
{
	return ArrayString<char, size>(str);
}

NS_END
