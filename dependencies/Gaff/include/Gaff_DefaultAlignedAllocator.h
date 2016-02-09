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

/*! \file */

#pragma once

#include "Gaff_IAllocator.h"
#include "Gaff_Utils.h"
#include <stdlib.h>

NS_GAFF

/*!
	\brief Simple malloc/free allocator that makes aligned allocations.
	\note Alignment is in bytes.
*/
class DefaultAlignedAllocator : public IAllocator
{
public:
	DefaultAlignedAllocator(size_t alignment = 16):
		_alignment(alignment)
	{
	}

	void* alloc(size_t size_bytes)
	{
		return AlignedMalloc(size_bytes, _alignment);
	}

	void free(void* data)
	{
		AlignedFree(data);
	}

private:
	size_t _alignment;
};

NS_END
