/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

class ProxyAllocator final : public Gaff::IAllocator
{
public:
	static ProxyAllocator& GetGlobal(void);

	explicit ProxyAllocator(const char* pool_tag);

	ProxyAllocator(const ProxyAllocator& allocator) = default;
	ProxyAllocator(void) = default;

	const ProxyAllocator& operator=(const ProxyAllocator& rhs);
	bool operator==(const ProxyAllocator& rhs) const;

	int32_t getPoolIndex(void) const { return _pool_index; }

	// Gaff::IAllocator
	void* alloc(size_t size_bytes, size_t alignment, const char* file, int line) override;
	void* alloc(size_t size_bytes, const char* file, int line) override;
	void free(void* data) override;

	void* realloc(void* old_ptr, size_t new_size, size_t alignment, const char* file, int line) override;
	void* realloc(void* old_ptr, size_t new_size, const char* file, int line) override;

	void* calloc(size_t num_members, size_t member_size, size_t alignment, const char* file, int line) override;
	void* calloc(size_t num_members, size_t member_size, const char* file, int line) override;

	size_t getUsableSize(const void* data) const override;

	// For EASTL support.
	void* allocate(size_t n, size_t alignment, size_t, int flags = 0) override;
	void* allocate(size_t n, int flags = 0) override;
	void deallocate(void* p, size_t) override;

	const char* get_name() const override;
	void set_name(const char* pName) override;

private:
	Shibboleth::IAllocator& _allocator = GetAllocator();
	int32_t _pool_index = 0;
};

NS_END
