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

#if defined(_WIN32) || defined(_WIN64)

#include "Gaff_DynamicModule_Windows.h"

NS_GAFF

DynamicModule::DynamicModule(void):
	_module(nullptr)
{
}

DynamicModule::~DynamicModule(void)
{
	destroy();
}

bool DynamicModule::load(const wchar_t* filename)
{
	_module = LoadLibraryW(filename);
	return _module != nullptr;
}

bool DynamicModule::load(const char* filename)
{
	_module = LoadLibraryA(filename);
	return _module != nullptr;
}

bool DynamicModule::destroy(void)
{
	if (_module) {
		return FreeLibrary(_module) != 0;
	}

	return false;
}

void* DynamicModule::GetAddress(const char* name) const
{
	return (void*)GetProcAddress(_module, name);
}

NS_END

#endif
