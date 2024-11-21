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

#include "Esprit_ProxyAllocator.h"
#include <Gaff_HashString.h>

NS_ESPRIT

template < class T, class HashType, Gaff::HashFunc<HashType> HashingFunc = Gaff::DefaultHashFunc<HashType> >
using HashString = Gaff::HashString<T, HashType, HashingFunc, ProxyAllocator, true>;

template < Gaff::HashFunc<Gaff::Hash32> HashingFunc = Gaff::DefaultHashFunc<Gaff::Hash32> >
using HashString32 = Gaff::HashString32<ProxyAllocator, HashingFunc>;

template < Gaff::HashFunc<Gaff::Hash64> HashingFunc = Gaff::DefaultHashFunc<Gaff::Hash64> >
using HashString64 = Gaff::HashString64<ProxyAllocator, HashingFunc>;

template < Gaff::HashFunc<Gaff::Hash32> HashingFunc = Gaff::DefaultHashFunc<Gaff::Hash32> >
using OptimizedHashString32 = Gaff::OptimizedHashString32<ProxyAllocator, HashingFunc>;

template < Gaff::HashFunc<Gaff::Hash64> HashingFunc = Gaff::DefaultHashFunc<Gaff::Hash64> >
using OptimizedHashString64 = Gaff::OptimizedHashString64<ProxyAllocator, HashingFunc>;

template < Gaff::HashFunc<Gaff::Hash32> HashingFunc = Gaff::DefaultHashFunc<Gaff::Hash32> >
using HashStringView32 = Gaff::HashStringView32<HashingFunc>;

template < Gaff::HashFunc<Gaff::Hash64> HashingFunc = Gaff::DefaultHashFunc<Gaff::Hash64> >
using HashStringView64 = Gaff::HashStringView64<HashingFunc>;

template < Gaff::HashFunc<Gaff::Hash32> HashingFunc = Gaff::DefaultHashFunc<Gaff::Hash32> >
using HashStringNoString32 = Gaff::HashStringNoString32<ProxyAllocator, HashingFunc>;

template < Gaff::HashFunc<Gaff::Hash64> HashingFunc = Gaff::DefaultHashFunc<Gaff::Hash64> >
using HashStringNoString64 = Gaff::HashStringNoString64<ProxyAllocator, HashingFunc>;

NS_END
