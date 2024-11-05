/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Gaff_Defines.h"
#include <new>

GCC_CLANG_DISABLE_WARNING_PUSH("-Wgnu-zero-variadic-macro-arguments")
#define GAFF_ALLOC_ARRAYT_ALIGNED(Class, alignment, count, allocator, ...) (allocator).template allocArrayT<Class>(alignment, count, __FILE__, __LINE__, ##__VA_ARGS__)
#define GAFF_ALLOC_ARRAYT(Class, count, allocator, ...) (allocator).template allocArrayT<Class>(count, __FILE__, __LINE__, ##__VA_ARGS__)
#define GAFF_ALLOCT_ALIGNED(Class, alignment, allocator, ...) (allocator).template allocT<Class>(alignment, __FILE__, __LINE__, ##__VA_ARGS__)
#define GAFF_ALLOCT(Class, allocator, ...) (allocator).template allocT<Class>(__FILE__, __LINE__, ##__VA_ARGS__)
#define GAFF_ALLOC_ALIGNED(size, alignment, allocator) (allocator).alloc(size, alignment, __FILE__, __LINE__)
#define GAFF_ALLOC(size, allocator) (allocator).alloc(size, __FILE__, __LINE__)
#define GAFF_FREE_ARRAYT(ptr, size, allocator) (allocator).freeArrayT(ptr, size)
#define GAFF_FREET(ptr, allocator) (allocator).freeT(ptr)
#define GAFF_FREE(ptr, allocator) (allocator).free(ptr)
#define GAFF_CALLOC_ALIGNED(num, size, alignment, allocator) (allocator).calloc(num, size, alignment, __FILE__, __LINE__)
#define GAFF_CALLOC(num, size, allocator) (allocator).calloc(num, size, __FILE__, __LINE__)
#define GAFF_REALLOC_ALIGNED(old_ptr, new_size, alignment, allocator) (allocator).realloc(old_ptr, new_size, alignment, __FILE__, __LINE__)
#define GAFF_REALLOC(old_ptr, new_size, allocator) (allocator).realloc(old_ptr, new_size, __FILE__, __LINE__)

#define GAFF_ALLOC_CAST_ALIGNED(type, size, alignment, allocator) reinterpret_cast<type>(GAFF_ALLOC_ALIGNED(size, alignment, allocator))
#define GAFF_ALLOC_CAST(type, size, allocator) reinterpret_cast<type>(GAFF_ALLOC(size, allocator))
GCC_CLANG_DISABLE_WARNING_POP()

NS_GAFF

template <class T, class... Args>
T* ConstructExact(T* data, Args... args)
{
	return new (data) T(args...);
}

template <class T, class... Args>
T* Construct(T* data, Args&&... args)
{
	return new (data) T(std::forward<Args>(args)...);
}

// This is giving a warning saying data is unreferenced ... what?
template <class T>
void Deconstruct(T* data)
{
	// To get rid of the stupid unreferenced parameter warning in VS.
#ifdef PLATFORM_WINDOWS
	GAFF_REF(data);
#endif

	data->~T();
}

class IAllocator
{
public:
	template <class T, class... Args>
	T* allocArrayT(size_t alignment, size_t count, const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count, alignment, file, line));

		for (size_t i = 0; i < count; ++i) {
			Construct(data + i, std::forward<Args>(args)...);
		}

		return data;
	}

	template <class T, class... Args>
	T* allocArrayT(size_t count, const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count, file, line));

		for (size_t i = 0; i < count; ++i) {
			Construct(data + i, std::forward<Args>(args)...);
		}

		return data;
	}

	template <class T, class... Args>
	T* allocT(size_t alignment, const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T), alignment, file, line));
		return Construct(data, std::forward<Args>(args)...);
	}

	template <class T, class... Args>
	T* allocT(const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T), file, line));
		return Construct(data, std::forward<Args>(args)...);
	}

	template <class T>
	void freeArrayT(T* data, size_t count)
	{
		for (size_t i = 0; i < count; ++i) {
			Deconstruct(data + i);
		}

		free((void*)(data));
	}

	template <class T>
	void freeT(T* data)
	{
		Deconstruct(data);
		free((void*)(data));
	}

	virtual ~IAllocator(void) {}

	virtual void* alloc(size_t size_bytes, size_t alignment, const char* file, int line) = 0;
	virtual void* alloc(size_t size_bytes, const char* file, int line) = 0;
	virtual void free(void* data) = 0;

	virtual void* realloc(void* old_ptr, size_t new_size, size_t alignment, const char* file, int line) = 0;
	virtual void* realloc(void* old_ptr, size_t new_size, const char* file, int line) = 0;

	virtual void* calloc(size_t num_members, size_t member_size, size_t alignment, const char* file, int line) = 0;
	virtual void* calloc(size_t num_members, size_t member_size, const char* file, int line) = 0;

	virtual size_t getUsableSize(const void* data) const = 0;

	// For EASTL support.
	virtual void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) = 0;
	virtual void* allocate(size_t n, int flags = 0) = 0;
	virtual void deallocate(void* p, size_t n) = 0;

	virtual const char* get_name() const = 0;
	virtual void set_name(const char* pName) = 0;
};

NS_END
