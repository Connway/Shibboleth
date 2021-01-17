/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
#include <EASTL/vector_map.h>

NS_GAFF

template < class Key, class Value, class Allocator, class Compare = eastl::less<Key> >
using VectorMap = eastl::vector_map<Key, Value, Compare, Allocator>;

template <class BinaryCompare, class T, class Key, class Value, class Allocator, class Compare>
typename VectorMap<Key, Value, Allocator, Compare>::const_iterator Find(const VectorMap<Key, Value, Allocator, Compare>& map, const T& value, const BinaryCompare& binary_compare = BinaryCompare())
{
	using Container = VectorMap<Key, Value, Allocator, Compare>;

	auto it = eastl::lower_bound(map.begin(), map.end(), value,
		[&binary_compare](const typename Container::value_type& lhs, const T& rhs) -> bool
	{
		return binary_compare(lhs.first, rhs);
	});

	if (it != map.end() && value != it->first) {
		return map.end();
	}

	return it;
}

template <class BinaryCompare, class T, class Key, class Value, class Allocator, class Compare>
typename VectorMap<Key, Value, Allocator, Compare>::iterator Find(VectorMap<Key, Value, Allocator, Compare>& map, const T& value, const BinaryCompare& binary_compare = BinaryCompare())
{
	using Container = VectorMap<Key, Value, Allocator, Compare>;

	auto it = eastl::lower_bound(map.begin(), map.end(), value,
	[&binary_compare](const typename Container::value_type& lhs, const T& rhs) -> bool
	{
		return binary_compare(lhs.first, rhs);
	});

	if (it != map.end() && value != it->first) {
		return map.end();
	}

	return it;
}

template <class T, class Key, class Value, class Allocator, class Compare>
typename VectorMap<Key, Value, Allocator, Compare>::const_iterator Find(const VectorMap<Key, Value, Allocator, Compare>& map, const T& value)
{
	using Container = VectorMap<Key, Value, Allocator, Compare>;

	auto it = eastl::lower_bound(map.begin(), map.end(), value,
		[](const typename Container::value_type& lhs, const T& rhs) -> bool
	{
		return eastl::less_2<Key, T>{}(lhs.first, rhs);
	});

	if (it != map.end() && value != it->first) {
		return map.end();
	}

	return it;
}

template <class T, class Key, class Value, class Allocator, class Compare>
typename VectorMap<Key, Value, Allocator, Compare>::iterator Find(VectorMap<Key, Value, Allocator, Compare>& map, const T& value)
{
	using Container = VectorMap<Key, Value, Allocator, Compare>;

	auto it = eastl::lower_bound(map.begin(), map.end(), value,
	[](const typename Container::value_type& lhs, const T& rhs) -> bool
	{
		return eastl::less_2<Key, T>{}(lhs.first, rhs);
	});

	if (it != map.end() && value != it->first) {
		return map.end();
	}

	return it;
}

NS_END
