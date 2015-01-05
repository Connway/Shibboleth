/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

/*! \file */

#pragma once

#include "Gaff_DefaultAllocator.h"
#include "Gaff_IRefCounted.h"
#include "Gaff_Atomic.h"

NS_GAFF

/*!
	\brief A simple, default implementation of a reference counted object.
	\tparam Allocator The allocator type we will use to de-allocate ourselves.
*/
template <class Allocator = DefaultAllocator>
class RefCounted : public IRefCounted
{
public:
	RefCounted(const Allocator& allocator = Allocator()):
		_allocator(allocator), _count(0)
	{
	}

	~RefCounted(void)
	{
	}

	void addRef(void) const
	{
		AtomicIncrement(&_count);
	}

	void release(void) const
	{
		unsigned int new_count = AtomicDecrement(&_count);

		if (!new_count) {
			// When the allocator resides in the object we are deleting,
			// we need to make a copy, otherwise there will be crashes.
			Allocator allocator(_allocator);
			allocator.freeT(this);
		}
	}

	unsigned int getRefCount(void) const
	{
		return _count;
	}

private:
	mutable Allocator _allocator;
	mutable volatile unsigned int _count;

	GAFF_NO_COPY(RefCounted);
	GAFF_NO_MOVE(RefCounted);
};

NS_END
