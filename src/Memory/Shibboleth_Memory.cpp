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

NS_SHIBBOLETH

static Allocator g_allocator;

size_t GetPoolIndex(const char* pool_name)
{
	return g_allocator.getPoolIndex(pool_name);
}

IAllocator* GetAllocator(void)
{
	return &g_allocator;
}

void* ShibbolethAllocate(size_t size, size_t pool_index)
{
	return SHIB_ALLOC(size, pool_index, g_allocator);
}

void* ShibbolethAllocate(size_t size)
{
	return SHIB_ALLOC(size, 0, g_allocator);
}

void ShibbolethFree(void* data)
{
	SHIB_FREE(data, g_allocator);
}

NS_END
