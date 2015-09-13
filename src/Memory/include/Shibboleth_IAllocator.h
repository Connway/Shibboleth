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

#include <Shibboleth_Defines.h>
#include <Gaff_IAllocator.h>

NS_SHIBBOLETH

class IAllocator : public Gaff::IAllocator
{
public:
	IAllocator(void) {}
	virtual ~IAllocator(void) {}

	virtual void createMemoryPool(const char* pool_name, unsigned int alloc_tag) = 0;
	virtual void* alloc(size_t size_bytes, unsigned int alloc_tag) = 0;
	virtual void* alloc(size_t size_bytes) = 0;
	virtual void free(void* data) = 0;

	GAFF_NO_COPY(IAllocator);
	GAFF_NO_MOVE(IAllocator);
};

NS_END
