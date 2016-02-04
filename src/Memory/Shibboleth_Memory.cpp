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

#include "Shibboleth_Memory.h"
#include "Shibboleth_Allocator.h"
#include <Shibboleth_ProxyAllocator.h>

#ifdef USE_VLD
	#include <vld.h>
#endif

NS_SHIBBOLETH

static Allocator gAllocator;

void CreateMemoryPool(const char* pool_name, unsigned int alloc_tag)
{
	gAllocator.createMemoryPool(pool_name, alloc_tag);
}

IAllocator* GetAllocator(void)
{
	return &gAllocator;
}

void* ShibbolethAllocate(size_t size, unsigned int alloc_tag)
{
	return gAllocator.alloc(size, alloc_tag);
}

void* ShibbolethAllocate(size_t size)
{
	return gAllocator.alloc(size, 0);
}

void ShibbolethFree(void* data)
{
	gAllocator.free(data);
}

NS_END
