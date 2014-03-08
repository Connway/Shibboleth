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

#include "Shibboleth_Allocator.h"
#include <Gaff_ScopedLock.h>

NS_SHIBBOLETH

static Allocator gAllocator;

Allocator::Allocator(void):
	_total_bytes_allocated(0), _num_allocations(0),
	_num_frees(0)
{
}

Allocator::~Allocator(void)
{
}

void* Allocator::alloc(unsigned int size_bytes)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_lock);

	void* data = malloc(size_bytes);

	if (data) {
		_total_bytes_allocated += size_bytes;
		++_num_allocations;
	}

	return data;
}

void Allocator::free(void* data)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_lock);
	++_num_frees;
	::free(data);
}

unsigned int Allocator::getTotalBytesAllocated(void) const
{
	return _total_bytes_allocated;
}

unsigned int Allocator::getNumAllocations(void) const
{
	return _num_allocations;
}

unsigned int Allocator::getNumFrees(void) const
{
	return _num_frees;
}

Allocator& GetAllocator(void)
{
	return gAllocator;
}

void* ShibbolethAllocate(size_t size)
{
	return gAllocator.alloc((unsigned int)size);
}

void ShibbolethFree(void* data)
{
	gAllocator.free(data);
}

NS_END
