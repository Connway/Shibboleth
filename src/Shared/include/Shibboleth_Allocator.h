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

#include "Shibboleth_Defines.h"
#include <Gaff_IAllocator.h>
#include <stdlib.h>

NS_SHIBBOLETH

class Allocator : public Gaff::IAllocator
{
public:
	Allocator(size_t alignment = 16);
	~Allocator(void);

	void* alloc(unsigned int size_bytes);
	void free(void* data);

	INLINE unsigned int getTotalBytesAllocated(void) const;
	INLINE unsigned int getNumAllocations(void) const;
	INLINE unsigned int getNumFrees(void) const;

private:
	volatile unsigned int _total_bytes_allocated;
	volatile unsigned int _num_allocations;
	volatile unsigned int _num_frees;
	size_t _alignment;

	GAFF_NO_MOVE(Allocator);
};

INLINE void SetAllocator(Allocator& allocator);
INLINE Allocator& GetAllocator(void);

void* ShibbolethAllocate(size_t size);
void ShibbolethFree(void* data);

NS_END
