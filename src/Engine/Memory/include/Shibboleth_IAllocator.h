/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include <Shibboleth_Defines.h>
#include <Gaff_IAllocator.h>

#define SHIB_ALLOC_ARRAYT_ALIGNED_POOL(type, alignment, count, pool_index, allocator, ...) (allocator).template allocArrayT<type>(alignment, pool_index, ##__VA_ARGS__)
#define SHIB_ALLOC_ARRAYT_ALIGNED(Class, alignment, count, allocator, ...) (allocator).template allocArrayT<Class>(alignment, count, __FILE__, __LINE__, ##__VA_ARGS__)
#define SHIB_ALLOC_ARRAYT_POOL(type, count, pool_index, allocator, ...) (allocator).template allocArrayT<type>(pool_index, __FILE__, __LINE__, ##__VA_ARGS__)
#define SHIB_ALLOC_ARRAYT(Class, count, allocator, ...) (allocator).template allocArrayT<Class>(alignment, __FILE__, __LINE__, ##__VA_ARGS__)

#define SHIB_ALLOCT_ALIGNED_POOL(type, alignment, pool_index, allocator, ...) (allocator).template allocT<type>(alignment, pool_index, __FILE__, __LINE__, ##__VA_ARGS__)
#define SHIB_ALLOCT_ALIGNED(Class, alignment, allocator, ...) (allocator).template allocT<Class>(alignment, __FILE__, __LINE__, ##__VA_ARGS__)
#define SHIB_ALLOCT_POOL(type, pool_index, allocator, ...) (allocator).template allocT<type>(pool_index, __FILE__, __LINE__, ##__VA_ARGS__)
#define SHIB_ALLOCT GAFF_ALLOCT

#define SHIB_ALLOC_ALIGNED_POOL(size, alignment, pool_index, allocator) (allocator).alloc(size, alignment, pool_index, __FILE__, __LINE__)
#define SHIB_ALLOC_ALIGNED GAFF_ALLOC_ALIGNED
#define SHIB_ALLOC_POOL(size, pool_index, allocator) (allocator).alloc(size, pool_index, __FILE__, __LINE__)
#define SHIB_ALLOC GAFF_ALLOC

#define SHIB_CALLOC_ALIGNED_POOL(num, size, alignment, pool_index, allocator) (allocator).calloc(num, size, alignment, pool_index, __FILE__, __LINE__)
#define SHIB_CALLOC_ALIGNED(num, size, alignment, allocator) (allocator).calloc(num, size, alignment, __FILE__, __LINE__)
#define SHIB_CALLOC_POOL(num, size, pool_index, allocator) (allocator).calloc(num, size, pool_index, __FILE__, __LINE__)
#define SHIB_CALLOC(num, size, allocator) (allocator).calloc(num, size, __FILE__, __LINE__)

#define SHIB_REALLOC_ALIGNED_POOL(old_ptr, new_size, alignment, pool_index, allocator) (allocator).realloc(old_ptr, new_size, alignment, pool_index, __FILE__, __LINE__)
#define SHIB_REALLOC_ALIGNED(old_ptr, new_size, alignment, allocator) (allocator).realloc(old_ptr, new_size, alignment, __FILE__, __LINE__)
#define SHIB_REALLOC_POOL(old_ptr, new_size, pool_index, allocator) (allocator).realloc(old_ptr, new_size, pool_index, __FILE__, __LINE__)
#define SHIB_REALLOC(old_ptr, new_size, allocator) (allocator).realloc(old_ptr, new_size, __FILE__, __LINE__)

#define SHIB_FREE_ARRAYT GAFF_FREE_ARRAYT
#define SHIB_FREET GAFF_FREET
#define SHIB_FREE GAFF_FREE

#define SHIB_ALLOC_CAST_ALIGNED_POOL(type, size, alignment, pool_index, allocator) reinterpret_cast<type>(SHIB_ALLOC_ALIGNED_POOL(size, alignment, pool_index, allocator))
#define SHIB_ALLOC_CAST_ALIGNED(type, size, alignment, allocator) reinterpret_cast<type>(SHIB_ALLOC_ALIGNED(size, alignment, allocator))
#define SHIB_ALLOC_CAST_POOL(type, size, pool_index, allocator) reinterpret_cast<type>(SHIB_ALLOC_POOL(size, pool_index, allocator))
#define SHIB_ALLOC_CAST(type, size, allocator) reinterpret_cast<type>(SHIB_ALLOC(size, allocator))

NS_SHIBBOLETH

class IAllocator
{
public:
	// Pool versions.
	template <class T, class... Args>
	T* allocArrayT(size_t alignment, size_t count, int32_t pool_index, const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count, alignment, pool_index, file, line));

		for (size_t i = 0; i < count; ++i) {
			Gaff::Construct(data + i, std::forward<Args>(args)...);
		}

		return data;
	}

	template <class T, class... Args>
	T* allocArrayT(size_t count, int32_t pool_index, const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count, pool_index, file, line));

		for (size_t i = 0; i < count; ++i) {
			Gaff::Construct(data + i, std::forward<Args>(args)...);
		}

		return data;
	}

	template <class T, class... Args>
	T* allocT(size_t alignment, int32_t pool_index, const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T), alignment, pool_index, file, line));
		return Gaff::Construct(data, std::forward<Args>(args)...);
	}

	template <class T, class... Args>
	T* allocT(int32_t pool_index, const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T), pool_index, file, line));
		return Gaff::Construct(data, std::forward<Args>(args)...);
	}

	// Regular versions.
	template <class T, class... Args>
	T* allocArrayT(size_t alignment, size_t count, const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count, alignment, file, line));

		for (size_t i = 0; i < count; ++i) {
			Gaff::Construct(data + i, std::forward<Args>(args)...);
		}

		return data;
	}

	template <class T, class... Args>
	T* allocArrayT(size_t count, const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count, file, line));

		for (size_t i = 0; i < count; ++i) {
			Gaff::Construct(data + i, std::forward<Args>(args)...);
		}

		return data;
	}

	template <class T, class... Args>
	T* allocT(size_t alignment, const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T), alignment, file, line));
		return Gaff::Construct(data, std::forward<Args>(args)...);
	}

	template <class T, class... Args>
	T* allocT(const char* file, int line, Args&&... args)
	{
		T* data = reinterpret_cast<T*>(alloc(sizeof(T), file, line));
		return Gaff::Construct(data, std::forward<Args>(args)...);
	}

	template <class T>
	void freeArrayT(T* data, size_t count)
	{
		for (size_t i = 0; i < count; ++i) {
			Gaff::Deconstruct(data + i);
		}

		free((void*)(data));
	}

	template <class T>
	void freeT(T* data)
	{
		Gaff::Deconstruct(data);
		free((void*)(data));
	}

	using OnFreeCallback = void (*)(void*);

	IAllocator(void) {}
	virtual ~IAllocator(void) {}

	virtual void addOnFreeCallback(OnFreeCallback callback, void* data) = 0;
	virtual void removeOnFreeCallback(OnFreeCallback callback, void* data) = 0;

	virtual int32_t getPoolIndex(const char* pool_name) = 0;
	virtual size_t getUsableSize(const void* data) const = 0;

	virtual void* alloc(size_t size_bytes, size_t alignment, int32_t pool_index, const char* file, int line) = 0;
	virtual void* alloc(size_t size_bytes, int32_t pool_index, const char* file, int line) = 0;

	virtual void* alloc(size_t size_bytes, size_t alignment, const char* file, int line) = 0;
	virtual void* alloc(size_t size_bytes, const char* file, int line) = 0;
	virtual void free(void* data) = 0;

	virtual void* calloc(size_t num_members, size_t member_size, size_t alignment, int32_t pool_index, const char* file, int line) = 0;
	virtual void* calloc(size_t num_members, size_t member_size, size_t alignment, const char* file, int line) = 0;
	virtual void* calloc(size_t num_members, size_t member_size, int32_t pool_index, const char* file, int line) = 0;
	virtual void* calloc(size_t num_members, size_t member_size, const char* file, int line) = 0;

	virtual void* realloc(void* old_ptr, size_t new_size, size_t alignment, int32_t pool_index, const char* file, int line) = 0;
	virtual void* realloc(void* old_ptr, size_t new_size, size_t alignment, const char* file, int line) = 0;

	virtual void* realloc(void* old_ptr, size_t new_size, int32_t pool_index, const char* file, int line) = 0;
	virtual void* realloc(void* old_ptr, size_t new_size, const char* file, int line) = 0;

	// Gaff::IAllocator stuff.
	// For EASTL support.
	virtual void* allocate(size_t n, int flags = 0) = 0;
	virtual void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0) = 0;
	virtual void deallocate(void* p, size_t n) = 0;

	virtual const char* get_name() const = 0;
	virtual void set_name(const char* pName) = 0;

	GAFF_NO_COPY(IAllocator);
	GAFF_NO_MOVE(IAllocator);
};

NS_END
