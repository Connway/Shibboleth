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

/*! \file */

#pragma once

#include "Gaff_DynamicModule.h"
#include "Gaff_SharedPtr.h"
#include "Gaff_HashMap.h"

NS_GAFF

/*!
	\brief Manages and loads dynamic modules. (*.so, *.dll, *.dylib)
	\tparam Allocator The allocator we will use for memory allocations.
*/
template <class Allocator = DefaultAllocator>
class DynamicLoader
{
public:
	/*!
		\brief Iterates over each ModulePtr and calls the callback.
		\tparam
			Callback The callback to use on each ModulePtr.
			Callbacks take the form of: bool CB(const ModulePtr&).
			Returning true will end the loop early.

		\return Returns whether the loop was terminated early.
	*/
	template <class Callback>
	bool forEachModule(Callback&& callback)
	{
		for (typename HMap::Iterator it = _modules.begin(); it != _modules.end(); ++it) {
			if (callback(*it)) {
				return true;
			}
		}

		return false;
	}

	typedef SharedPtr<DynamicModule, Allocator> ModulePtr;

	DynamicLoader(const Allocator& allocator = Allocator());
	~DynamicLoader(void);

	void clear(void);

	ModulePtr loadModule(const char* filename, const char* name);
	ModulePtr getModule(const char* name);

	void removeModule(const char* name);

private:
	typedef AHashString<Allocator> HString;
	typedef HashMap<HString, ModulePtr, Allocator> HMap;

	HMap _modules;
	Allocator _allocator;

	GAFF_NO_COPY(DynamicLoader);
	GAFF_NO_MOVE(DynamicLoader);
};

#include "Gaff_DynamicLoader.inl"

NS_END
