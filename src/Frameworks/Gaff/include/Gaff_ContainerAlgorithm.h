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

#include "Gaff_Defines.h"
#include <EASTL/sort.h>

NS_GAFF

template <class Container, class Value, class Predicate>
auto LowerBound(Container& container, const Value& value, Predicate predicate)
{
	return eastl::lower_bound(container.begin(), container.end(), value, predicate);
}

template <class Container, class Value>
auto LowerBound(Container& container, const Value& value)
{
	return eastl::lower_bound(container.begin(), container.end(), value);
}

template <class Container, class Value, class Predicate>
auto UpperBound(Container& container, const Value& value, Predicate predicate)
{
	return eastl::upper_bound(container.begin(), container.end(), value, predicate);
}

template <class Container, class Value>
auto UpperBound(Container& container, const Value& value)
{
	return eastl::upper_bound(container.begin(), container.end(), value);
}

template <class Container, class Value, class Predicate>
auto FindSorted(Container& container, const Value& value, Predicate predicate)
{
	return eastl::binary_search_i(container.begin(), container.end(), value, predicate);
}

template <class Container, class Value>
auto FindSorted(Container& container, const Value& value)
{
	return eastl::binary_search_i(container.begin(), container.end(), value);
}

template <class Container, class Value, class Predicate>
auto Find(Container& container, const Value& value, Predicate predicate)
{
	return eastl::find(container.begin(), container.end(), value, predicate);
}

template <class Container, class Value>
auto Find(Container& container, const Value& value)
{
	return eastl::find(container.begin(), container.end(), value);
}

template <class Container, class Value, class Predicate>
bool ContainsSorted(Container& container, const Value& value, Predicate predicate)
{
	return eastl::binary_search(container.begin(), container.end(), value, predicate);
}

template <class Container, class Value>
bool ContainsSorted(Container& container, const Value& value)
{
	return eastl::binary_search(container.begin(), container.end(), value);
}

template <class Container, class Value, class Predicate>
bool Contains(Container& container, const Value& value, Predicate predicate)
{
	return Find(container, value, predicate) != container.end();
}

template <class Container, class Value>
bool Contains(Container& container, const Value& value)
{
	return Find(container, value) != container.end();
}

template <class Container, class Predicate>
auto Sort(Container& container, Predicate predicate)
{
	return eastl::sort(container.begin(), container.end(), predicate);
}

template <class Container>
auto Sort(Container& container)
{
	return eastl::sort(container.begin(), container.end());
}

template <class IndexType = int32_t, class Container, class Value, class Predicate>
IndexType IndexOf(const Container& container, const Value& value, Predicate predicate)
{
	const auto it = Find(container, value, predicate);
	return (it == container.end()) ? static_cast<IndexType>(-1) : static_cast<IndexType>(eastl::distance(container.begin(), it));
}

template <class IndexType = int32_t, class Container, class Value>
IndexType IndexOf(const Container& container, const Value& value)
{
	const auto it = Find(container, value);
	return (it == container.end()) ? static_cast<IndexType>(-1) : static_cast<IndexType>(eastl::distance(container.begin(), it));
}

template <class IndexType = int32_t, class T, size_t array_size>
constexpr IndexType IndexOfArray(const T (&array)[array_size], const T& value)
{
	for (int32_t i = 0; i < static_cast<int32_t>(array_size); ++i) {
		if (array[i] == value) {
			return static_cast<IndexType>(i);
		}
	}

	return static_cast<IndexType>(-1);
}

template <class IndexType = int32_t, class T, size_t array_size>
constexpr IndexType IndexOfArray(T (&array)[array_size], const T& value)
{
	for (int32_t i = 0; i < static_cast<int32_t>(array_size); ++i) {
		if (array[i] == value) {
			return static_cast<IndexType>(i);
		}
	}

	return static_cast<IndexType>(-1);
}

NS_END
