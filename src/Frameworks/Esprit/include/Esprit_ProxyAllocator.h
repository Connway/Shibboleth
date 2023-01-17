/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Esprit_Global.h"

NS_ESPRIT

class ProxyAllocator : public Gaff::IAllocator
{
public:
	GAFF_STRUCTORS_DEFAULT(ProxyAllocator);
	GAFF_COPY_DEFAULT(ProxyAllocator);
	GAFF_MOVE_DEFAULT(ProxyAllocator);

	explicit ProxyAllocator(const char* name):
		_name(name)
	{
	}

	bool operator==(const ProxyAllocator& rhs) const
	{
		return _name == rhs._name;
	}

	void* alloc(size_t size_bytes, size_t alignment, const char* file, int line) override
	{
		return GetAllocator()->alloc(size_bytes, alignment, file, line);
	}

	void* alloc(size_t size_bytes, const char* file, int line) override
	{
		return GetAllocator()->alloc(size_bytes, file, line);
	}

	void free(void* data) override
	{
		GetAllocator()->free(data);
	}

	void* realloc(void* old_ptr, size_t new_size, size_t alignment, const char* file, int line) override
	{
		return GetAllocator()->realloc(old_ptr, new_size, alignment, file, line);
	}

	void* realloc(void* old_ptr, size_t new_size, const char* file, int line) override
	{
		return GetAllocator()->realloc(old_ptr, new_size, file, line);
	}

	void* calloc(size_t num_members, size_t member_size, size_t alignment, const char* file, int line) override
	{
		return GetAllocator()->calloc(num_members, member_size, alignment, file, line);
	}

	void* calloc(size_t num_members, size_t member_size, const char* file, int line) override
	{
		return GetAllocator()->calloc(num_members, member_size, file, line);
	}

	size_t getUsableSize(const void* data) const override
	{
		return GetAllocator()->getUsableSize(data);
	}

	// For EASTL support.
	void* allocate(size_t n, int flags = 0) override
	{
		return GetAllocator()->allocate(n, flags);
	}

	void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) override
	{
		return GetAllocator()->allocate(n, alignment, offset, flags);
	}

	void deallocate(void* p, size_t n) override
	{
		return GetAllocator()->deallocate(p, n);
	}

	const char* get_name() const override
	{
		return _name;
	}

	void set_name(const char* pName) override
	{
		_name = pName;
	}

private:
	const char* _name = nullptr;
};

NS_END
