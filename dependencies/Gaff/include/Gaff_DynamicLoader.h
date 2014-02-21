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

#include "Gaff_DynamicModule.h"
#include "Gaff_SharedPtr.h"
#include "Gaff_Array.h"

NS_GAFF

template <class Allocator = DefaultAllocator>
class DynamicLoader
{
public:
	typedef SharedPtr<DynamicModule, Allocator> ModulePtr;

	template <class Allocator>
	ModulePtr loadModule(const AString<Allocator>& filename)
	{
		return loadModule(filename.getBuffer());
	}

	DynamicLoader(const Allocator& allocator = Allocator());
	~DynamicLoader(void);

	ModulePtr loadModule(const char* filename);

#ifdef _UNICODE
	template <class Allocator>
	ModulePtr loadModule(const WString<Allocator>& filename)
	{
		return loadModule(filename.getBuffer());
	}

	ModulePtr loadModule(const wchar_t* filename);
#endif

private:
	Array<ModulePtr, Allocator> _modules;
	Allocator _allocator;
};

#include "Gaff_DynamicLoader.inl"

NS_END
