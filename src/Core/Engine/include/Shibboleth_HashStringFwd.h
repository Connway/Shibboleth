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

#include "Shibboleth_ProxyAllocator.h"
#include <Gaff_Hash.h>

NS_GAFF
	template <class T, class HashType, class Allocator, bool contains_string = true>
	template <class T, class HashType, HashFunc<HashType> HashingFunc, class Allocator>
	class HashString;

	template <class T, class HashType>
	class HashStringView;

	template <class Allocator>
	using HashString32 = HashString<char, Hash32, Allocator>;

	template <class Allocator>
	using HashString64 = HashString<char, Hash64, Allocator>;

	using HashStringView32 = HashStringView<char, Hash32>;
	using HashStringView64 = HashStringView<char, Hash64>;
NS_END

NS_SHIBBOLETH

template <class T, class HashType>
using HashString = Gaff::HashString<T, HashType, ProxyAllocator>;

using HashString32 = Gaff::HashString32<ProxyAllocator>;
using HashString64 = Gaff::HashString64<ProxyAllocator>;

using HashStringView32 = Gaff::HashStringView32<>;
using HashStringView64 = Gaff::HashStringView64<>;

NS_END
