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

#include "Gaff_DynamicModule.h"
#include "Gaff_HashString.h"
#include "Gaff_SmartPtrs.h"
#include "Gaff_VectorMap.h"

NS_GAFF

template <class Allocator = DefaultAllocator>
class DynamicLoader
{
public:
	template <class Callback>
	bool forEachModule(Callback&& callback) const
	{
		for (auto it = _modules.begin(); it != _modules.end(); ++it) {
			if (callback(it->first, it->second)) {
				return true;
			}
		}

		return false;
	}

	template <class Callback>
	bool forEachModule(Callback&& callback)
	{
		for (auto it = _modules.begin(); it != _modules.end(); ++it) {
			if (callback(it->first, it->second)) {
				return true;
			}
		}

		return false;
	}

	using ModulePtr = SharedPtr<DynamicModule>;

	DynamicLoader(const Allocator& allocator = Allocator());
	~DynamicLoader(void);

	void clear(void);

	ModulePtr loadModule(const char8_t* filename, const char8_t* name);
	ModulePtr getModule(const char8_t* name);
	void removeModule(const char8_t* name);

private:
	using HString = HashString32<Allocator>;
	using HMap = VectorMap<HString, ModulePtr, Allocator>;

	HMap _modules;
	Allocator _allocator;

	GAFF_NO_COPY(DynamicLoader);
	GAFF_NO_MOVE(DynamicLoader);
};

#include "Gaff_DynamicLoader.inl"

NS_END
