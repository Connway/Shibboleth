/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_ProxyAllocator.h"

NS_SHIBBOLETH

static ProxyAllocator g_global_proxy;

ProxyAllocator& ProxyAllocator::GetGlobal(void)
{
	return g_global_proxy;
}

ProxyAllocator::ProxyAllocator(const char* pool_tag):
	_pool_tag(pool_tag)
{
	if (_pool_tag) {
		_pool_index = _allocator.getPoolIndex(_pool_tag);
	}
}

const ProxyAllocator& ProxyAllocator::operator=(const ProxyAllocator& rhs)
{
	_pool_tag = rhs._pool_tag;
	_pool_index = rhs._pool_index;
	return *this;
}

bool ProxyAllocator::operator==(const ProxyAllocator& rhs) const
{
	return _pool_index == rhs._pool_index;
}

// For EASTL support.
void* ProxyAllocator::allocate(size_t n, size_t alignment, size_t, int flags)
{
	GAFF_REF(flags);
	return alloc(n, alignment, __FILE__, __LINE__);
}

void* ProxyAllocator::allocate(size_t n, int flags)
{
	GAFF_REF(flags);
	return alloc(n, __FILE__, __LINE__);
}

void ProxyAllocator::deallocate(void* p, size_t)
{
	free(p);
}

const char* ProxyAllocator::get_name() const
{
	return _pool_tag;
}

void ProxyAllocator::set_name(const char* pName)
{
	_pool_tag = pName;

	if (_pool_tag) {
		_pool_index = _allocator.getPoolIndex(_pool_tag);
	}
}

void* ProxyAllocator::alloc(size_t size_bytes, size_t alignment, const char* file, int line)
{
	return _allocator.alloc(size_bytes, alignment, _pool_index, file, line);
}

void* ProxyAllocator::alloc(size_t size_bytes, const char* file, int line)
{
	return _allocator.alloc(size_bytes, _pool_index, file, line);
}

void ProxyAllocator::free(void* data)
{
	_allocator.free(data);
}

NS_END
