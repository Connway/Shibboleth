/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Shibboleth_Allocator.h"
#include <Gaff_Atomic.h>

#define SHIB_REF_COUNTED(Class) \
public: \
	void addRef(void) const \
	{ \
		AtomicIncrement(&_count); \
	} \
	void release(void) const \
	{ \
		unsigned int new_count = AtomicDecrement(&_count); \
		if (!new_count) { \
			GetAllocator()->freeT(this); \
		} \
	} \
	unsigned int getRefCount(void) const \
	{ \
		return _count; \
	} \
private: \
	mutable volatile unsigned int _count = 0 // Use C++11 in-class initialization so that classes don't have to modify constructors.