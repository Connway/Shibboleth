/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Gaff_IncludeWindows.h"
#include "Gaff_Defines.h"

NS_GAFF

class DynamicModule
{
public:
	DynamicModule(void);
	~DynamicModule(void);

	template <class Func>
	Func getFunc(const char* name) const
	{
		return reinterpret_cast<Func>(getAddress(name));
	}

	template <class T>
	T* getVariable(const char* name) const
	{
		return reinterpret_cast<T*>(getAddress(name));
	}

	bool load(const char8_t* filename);

	bool destroy(void);

	void* getAddress(const char8_t* name) const;
	void* getAddress(const char* name) const;

	static const char* GetErrorString(void);

private:
	HMODULE _module;

	GAFF_NO_COPY(DynamicModule);
	GAFF_NO_MOVE(DynamicModule);
};

NS_END
