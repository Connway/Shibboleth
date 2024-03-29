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

#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Memory.h>
#include <Gaff_IncludeEASTLAtomic.h>
#include <Gaff_IRefCounted.h>

#define SHIB_REF_COUNTED() \
public: \
	void addRef(void) const \
	{ \
		++_count; \
	} \
	void release(void) const \
	{ \
		int32_t new_count = --_count; \
		if (!new_count) { \
			SHIB_FREET(this, Shibboleth::GetAllocator()); \
		} \
	} \
	int32_t getRefCount(void) const \
	{ \
		return _count; \
	} \
private: \
	mutable eastl::atomic<int32_t> _count = 0

NS_SHIBBOLETH

class RefCounted : public Gaff::IRefCounted
{
public:
	RefCounted(void) {}

	void addRef(void) const
	{
		++_count;
	}

	void release(void) const
	{
		int32_t new_count = --_count;

		if (!new_count) {
			SHIB_FREET(this, GetAllocator());
		}
	}

	int32_t getRefCount(void) const
	{
		return _count;
	}

private:
	mutable eastl::atomic<int32_t> _count = 0;

	GAFF_NO_COPY(RefCounted);
	GAFF_NO_MOVE(RefCounted);
};

NS_END
