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

#include "Gaff_Defines.h"

#define GAFF_DEFER_LINE_CAT(name, line) name##line
#define GAFF_DEFER(defer_func, ...) const auto GAFF_DEFER_LINE_CAT(DEFER, __LINE__)(Gaff::CreateDefer(defer_func))

NS_GAFF

template <class T>
class Defer final
{
public:
	explicit Defer(T&& defer_func): _defer_func(defer_func) {}
	Defer(Defer<T>&& scoped_exit) = default;

	// This should only be executed once when using the GAFF_DEFER macro.
	~Defer(void) { _defer_func(); }

private:
	T _defer_func;
};

template <class T>
Defer<T> CreateDefer(T&& defer_func)
{
	return Defer<T>(std::move(defer_func));
}

NS_END
