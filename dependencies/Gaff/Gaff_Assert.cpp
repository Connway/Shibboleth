/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Gaff_Assert.h"
#include <cassert>
#include <cstdarg>

NS_GAFF

void DefaultAssertHandler(const char*, const char*, const char*, int)
{
	assert(false);
}

static AssertHandler g_assert_handler = DefaultAssertHandler;

void SetAssertHandler(AssertHandler handler)
{
	g_assert_handler = (handler) ? handler : DefaultAssertHandler;
}

void Assert(const char* msg, const char* expr, const char* file, int line, ...)
{
	if (msg) {
		char temp[256] = { 0 };

		va_list vl;
		va_start(vl, line);
		vsnprintf(temp, 256, msg, vl);
		va_end(vl);

		g_assert_handler(temp, expr, file, line);

	} else {
		g_assert_handler(msg, expr, file, line);
	}
}

NS_END
