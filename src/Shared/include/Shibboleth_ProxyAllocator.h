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

#pragma once

#include "Shibboleth_Memory.h"
#include <Gaff_Math.h>
#include <cstring>

NS_SHIBBOLETH

class ProxyAllocator : public Gaff::IAllocator
{
public:
	explicit ProxyAllocator(const char* pool_tag = nullptr, Shibboleth::IAllocator* allocator = GetAllocator()):
		_allocator(allocator), _alloc_tag(0)
	{
		if (pool_tag) {
			_alloc_tag = Gaff::FNV1Hash32(pool_tag, strlen(pool_tag));
			_allocator->createMemoryPool(pool_tag, _alloc_tag);
		}
	}

	ProxyAllocator(const ProxyAllocator& allocator):
		_allocator(allocator._allocator), _alloc_tag(allocator._alloc_tag)
	{
	}

	const ProxyAllocator& operator=(const ProxyAllocator& rhs)
	{
		_allocator = rhs._allocator;
		_alloc_tag = rhs._alloc_tag;
		return *this;
	}

	void* alloc(size_t size_bytes)
	{
		return _allocator->alloc(size_bytes, _alloc_tag);
	}

	void free(void* data)
	{
		_allocator->free(data, _alloc_tag);
	}

private:
	Shibboleth::IAllocator* _allocator;
	unsigned int _alloc_tag;
};

NS_END
