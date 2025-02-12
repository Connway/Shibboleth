/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#ifndef EASTL_QUEUE_DEFAULT_NAME
	#define EASTL_QUEUE_DEFAULT_NAME "Untagged"
#endif

#ifndef EASTL_DEQUE_DEFAULT_NAME
	#define EASTL_DEQUE_DEFAULT_NAME "Untagged"
#endif

#include "Gaff_DefaultAllocator.h"
#include <EASTL/queue.h>

NS_GAFF

template <class T, class Allocator = DefaultAllocator>
using Queue = eastl::queue< T, eastl::deque<T, Allocator, DEQUE_DEFAULT_SUBARRAY_SIZE(T)> >;

NS_END
