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

#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Memory.h>
#include <Gaff_Assert.h>

void* operator new(size_t count) noexcept(false)
{
	return SHIB_ALLOC(count, Shibboleth::GetAllocator());
}

void* operator new[](size_t count) noexcept(false)
{
	return SHIB_ALLOC(count, Shibboleth::GetAllocator());
}

void* operator new(size_t count, const std::nothrow_t&) noexcept
{
	return SHIB_ALLOC(count, Shibboleth::GetAllocator());
}

void* operator new[](size_t count, const std::nothrow_t&) noexcept
{
	return SHIB_ALLOC(count, Shibboleth::GetAllocator());
}

void operator delete(void* ptr) noexcept
{
	SHIB_FREE(ptr, Shibboleth::GetAllocator());
}

void operator delete[](void* ptr) noexcept
{
	SHIB_FREE(ptr, Shibboleth::GetAllocator());
}

#if (__cplusplus >= 201402L || _MSC_VER >= 1916)
void operator delete(void* ptr, size_t size) noexcept
{
	auto& allocator = Shibboleth::GetAllocator();
	GAFF_ASSERT(ptr == NULL || size <= allocator.getUsableSize(ptr));
	SHIB_FREE(ptr, allocator);
}

void operator delete[](void* ptr, size_t size) noexcept
{
	auto& allocator = Shibboleth::GetAllocator();
	GAFF_ASSERT(ptr == NULL || size <= allocator.getUsableSize(ptr));
	SHIB_FREE(ptr, allocator);
}
#endif

#if (__cplusplus > 201402L || defined(__cpp_aligned_new))
void* operator new (size_t count, std::align_val_t al) noexcept(false)
{
	return SHIB_ALLOC_ALIGNED(count, static_cast<size_t>(al), Shibboleth::GetAllocator());
}

void* operator new[](size_t count, std::align_val_t al) noexcept(false)
{
	return SHIB_ALLOC_ALIGNED(count, static_cast<size_t>(al), Shibboleth::GetAllocator());
}

void* operator new(size_t count, std::align_val_t al, const std::nothrow_t&) noexcept
{
	return SHIB_ALLOC_ALIGNED(count, static_cast<size_t>(al), Shibboleth::GetAllocator());
}

void* operator new[](size_t count, std::align_val_t al, const std::nothrow_t&) noexcept
{
	return SHIB_ALLOC_ALIGNED(count, static_cast<size_t>(al), Shibboleth::GetAllocator());
}

void operator delete(void* ptr, std::align_val_t al) noexcept
{
	GAFF_ASSERT(((uintptr_t)ptr % static_cast<size_t>(al)) == 0);
	SHIB_FREE(ptr, Shibboleth::GetAllocator());
}

void operator delete[](void* ptr, std::align_val_t al) noexcept
{
	GAFF_ASSERT(((uintptr_t)ptr % static_cast<size_t>(al)) == 0);
	SHIB_FREE(ptr, Shibboleth::GetAllocator());
}

void operator delete(void* ptr, std::size_t size, std::align_val_t al) noexcept
{
	auto& allocator = Shibboleth::GetAllocator();

	GAFF_ASSERT(((uintptr_t)ptr % static_cast<size_t>(al)) == 0);
	GAFF_ASSERT(ptr == NULL || size <= allocator.getUsableSize(ptr));
	SHIB_FREE(ptr, allocator);
}

void operator delete[](void* ptr, std::size_t size, std::align_val_t al) noexcept
{
	auto& allocator = Shibboleth::GetAllocator();

	GAFF_ASSERT(((uintptr_t)ptr % static_cast<size_t>(al)) == 0);
	GAFF_ASSERT(ptr == NULL || size <= allocator.getUsableSize(ptr));
	SHIB_FREE(ptr, allocator);
}
#endif

void* operator new[](size_t size, const char* /*pName*/, int /*flags*/, unsigned /*debugFlags*/, const char* file, int line)
{
	return Shibboleth::GetAllocator().alloc(size, file, line);
}

void* operator new[](size_t size, size_t alignment, size_t /*alignmentOffset*/, const char* /*pName*/, int /*flags*/, unsigned /*debugFlags*/, const char* file, int line)
{
	return Shibboleth::GetAllocator().alloc(size, alignment, file, line);
}
