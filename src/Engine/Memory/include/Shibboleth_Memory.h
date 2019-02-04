/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
#include <Gaff_Defines.h>

#ifdef IS_MEMORY
	#define MEMORY_API DYNAMICEXPORT
#else
	#define MEMORY_API DYNAMICIMPORT
#endif

NS_SHIBBOLETH

class IAllocator;

// GetPoolIndex() is not thread-safe. Applications need to ensure they've created all their pools before threading.
MEMORY_API int32_t GetPoolIndex(const char* pool_name);
MEMORY_API IAllocator& GetAllocator(void);

MEMORY_API void* ShibbolethAllocate(size_t size, size_t alignment, int32_t pool_index);
MEMORY_API void* ShibbolethAllocate(size_t size, int32_t pool_index);

MEMORY_API void* ShibbolethAllocate(size_t size, size_t alignment);
MEMORY_API void* ShibbolethAllocate(size_t size);
MEMORY_API void ShibbolethFree(void* data);

MEMORY_API void SetLogDir(const char* dir);
MEMORY_API void AllocatorThreadInit(void);

NS_END
