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

#include "Gaff_IAllocator.h"
#include <stdlib.h>

NS_GAFF

class DefaultAllocator : public IAllocator
{
public:
	DefaultAllocator(void):
		_total_bytes_allocated(0), _num_allocations(0), _num_frees(0)
	{
	}

	void* alloc(unsigned int size_bytes)
	{
		void* data = malloc(size_bytes);

		if (data) {
			_total_bytes_allocated += size_bytes;
			++_num_allocations;
		}

		return data;
	}

	void free(void* data)
	{
		++_num_frees;
		::free(data);
	}

	unsigned int getTotalBytesAllocated(void) const
	{
		return _total_bytes_allocated;
	}

	unsigned int getNumAllocations(void) const
	{
		return _num_allocations;
	}

	unsigned int getNumFrees(void) const
	{
		return _num_frees;
	}

private:
	unsigned int _total_bytes_allocated;
	unsigned int _num_allocations;
	unsigned int _num_frees;
};

NS_END
