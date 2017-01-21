/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Memory.h>

NS_SHIBBOLETH

class ProxyAllocator : public Gaff::IAllocator
{
public:
	//explicit ProxyAllocator(const char* pool_tag = nullptr, Shibboleth::IAllocator* allocator = GetAllocator()):
	//	_allocator(allocator), _pool_index(0)
	//{
	//	if (pool_tag) {
	//		_pool_index = _allocator->getPoolIndex(pool_tag);
	//	}
	//}

	//ProxyAllocator(const ProxyAllocator& allocator):
	//	_allocator(allocator._allocator), _pool_index(allocator._pool_index)
	//{
	//}

	//const ProxyAllocator& operator=(const ProxyAllocator& rhs)
	//{
	//	_allocator = rhs._allocator;
	//	_pool_index = rhs._pool_index;
	//	return *this;
	//}

	explicit ProxyAllocator(const char* pool_tag):
		/*_allocator(GetAllocator()), */_pool_tag(pool_tag)//,
		//_pool_index(0)
	{
		if (_pool_tag) {
			_pool_index = _allocator->getPoolIndex(_pool_tag);
		}
	}

	ProxyAllocator(void) = default;
	ProxyAllocator(const ProxyAllocator& allocator) = default;

	bool operator==(const ProxyAllocator& rhs) const
	{
		return _pool_index == rhs._pool_index;
	}

	const ProxyAllocator& operator=(const ProxyAllocator& rhs)
	{
		_pool_tag = rhs._pool_tag;
		_pool_index = rhs._pool_index;
		return *this;
	}

	// For EASTL support.
	void* allocate(size_t n, int flags = 0) override
	{
		GAFF_REF(flags);
		return alloc(n, __FILE__, __LINE__);
	}

	void* allocate(size_t n, size_t alignment, size_t, int flags = 0) override
	{
		GAFF_REF(flags);
		return alloc(n, alignment, __FILE__, __LINE__);
	}

	void deallocate(void* p, size_t) override
	{
		free(p);
	}

	const char* get_name() const
	{
		return _pool_tag;
	}

	void set_name(const char* pName)
	{
		_pool_tag = pName;

		if (_pool_tag) {
			_pool_index = _allocator->getPoolIndex(_pool_tag);
		}
	}

	void* alloc(size_t size_bytes, size_t alignment, const char* file, int line) override
	{
		return _allocator->alloc(size_bytes, alignment, _pool_index, file, line);
	}

	void* alloc(size_t size_bytes, const char* file, int line) override
	{
		return _allocator->alloc(size_bytes, _pool_index, file, line);
	}

	void free(void* data) override
	{
		_allocator->free(data);
	}

private:
	Shibboleth::IAllocator* _allocator = GetAllocator();
	const char* _pool_tag = nullptr;
	int32_t _pool_index = 0;
};

NS_END
