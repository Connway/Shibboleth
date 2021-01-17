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

#include "Gaff_Defines.h"

#define GAFF_SCOPE_LINE_CAT2(name, line) name##_line_##line
#define GAFF_SCOPE_LINE_CAT(name, line) GAFF_SCOPE_LINE_CAT2(name, line)
#define GAFF_SCOPE_EXIT(exit_func) auto GAFF_SCOPE_LINE_CAT(exit, __LINE__)(Gaff::CreateExitScope(exit_func))

NS_GAFF

template <class T>
class ScopeExit
{
public:
	explicit ScopeExit(T&& exit_func): _exit_func(exit_func) {}
	ScopeExit(ScopeExit<T>&& scoped_exit): _exit_func(scoped_exit._exit_func) {}

	// This should only be executed once when using the GAFF_SCOPE_EXIT macro.
	~ScopeExit(void) { _exit_func(); }

private:
	T _exit_func;
};

template <class T>
ScopeExit<T> CreateExitScope(T&& exit_func)
{
	return ScopeExit<T>(std::move(exit_func));
}

NS_END
