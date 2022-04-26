/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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
#include "Gaff_Utils.h"

NS_GAFF

template <size_t alignment>
class DefaultAlignedAllocator : public IAllocator
{
public:
	DefaultAlignedAllocator(const char* name = nullptr):
		_name(name)
	{
	}

	bool operator==(const DefaultAlignedAllocator& rhs) const
	{
		return _name == rhs._name;
	}

	void* alloc(size_t size_bytes, size_t align, const char* /*file*/, int /*line*/) override
	{
		return allocate(size_bytes, align, 0, 0);
	}

	void* alloc(size_t size_bytes, const char* /*file*/, int /*line*/) override
	{
		return allocate(size_bytes);
	}

	void free(void* data) override
	{
		AlignedFree(data);
	}

	void* realloc(void* old_ptr, size_t new_size, size_t /*alignment*/, const char* /*file*/, int /*line*/) override
	{
		return AlignedRealloc(old_ptr, new_size, alignment);
	}

	void* realloc(void* old_ptr, size_t new_size, const char* /*file*/, int /*line*/) override
	{
		return AlignedRealloc(old_ptr, new_size, alignment);
	}

	void* calloc(size_t num_members, size_t member_size, size_t /*alignment*/, const char* /*file*/, int /*line*/) override
	{
		return AlignedCalloc(num_members, member_size, alignment);
	}

	void* calloc(size_t num_members, size_t member_size, const char* /*file*/, int /*line*/) override
	{
		return AlignedCalloc(num_members, member_size, alignment);
	}

	size_t getUsableSize(const void* data) const override
	{
		return GetUsableSize(const_cast<void*>(data));
	}

	// For EASTL support.
	void* allocate(size_t n, size_t align, size_t, int flags = 0) override
	{
		GAFF_REF(flags);
		return AlignedMalloc(n, align);
	}

	void* allocate(size_t n, int flags = 0) override
	{
		GAFF_REF(flags);
		return AlignedMalloc(n, alignment);
	}

	void deallocate(void* p, size_t) override
	{
		AlignedFree(p);
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
