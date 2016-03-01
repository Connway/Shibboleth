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
#include "Gaff_Utils.h"
#include <boxer/boxer.h>
#include <cstdarg>

#define FINAL_ASSERT_MSG_LEN 2048

NS_GAFF

static AssertHandler g_assert_handler = DefaultAssertHandler;

void DefaultAssertHandler(const char* msg, const char* expr, const char* file, int line)
{
	char final_msg[FINAL_ASSERT_MSG_LEN] = { 0 };

	if (msg) {
		snprintf(
			final_msg, FINAL_ASSERT_MSG_LEN,
			"Assertion failed!\n\nFile: %s\nLine: %i\n\nExpression: %s\nMessage: %s\n\n(Press Retry to debug the application)",
			file, line, expr, msg
		);
	} else {
		snprintf(
			final_msg, FINAL_ASSERT_MSG_LEN,
			"Assertion failed!\n\nFile: %s\nLine: %i\n\nExpression: %s\n\n(Press Retry to debug the application)",
			file, line, expr
		);
	}

	boxer::Selection selection = boxer::show(final_msg, "Assert Triggered!", boxer::Style::Error, boxer::Buttons::AbortRetryIgnore);

	switch (selection) {
		// Terminate the application.
		case boxer::Selection::Abort:
			exit(-1);
			break;

		// Break into debugger if one is present.
		case boxer::Selection::Retry:
			if (IsDebuggerAttached()) {
				DebugBreak();
			}
			break;

		// Try to continue on.
		case boxer::Selection::Ignore:
		default:
			break;
	}
}

void SetAssertHandler(AssertHandler handler)
{
	g_assert_handler = (handler) ? handler : DefaultAssertHandler;
}

void Assert(const char* msg, const char* expr, const char* file, int line, ...)
{
	if (msg) {
		char temp[FINAL_ASSERT_MSG_LEN / 2] = { 0 };

		va_list vl;
		va_start(vl, line);
		vsnprintf(temp, FINAL_ASSERT_MSG_LEN / 2, msg, vl);
		va_end(vl);

		g_assert_handler(temp, expr, file, line);

	} else {
		g_assert_handler(msg, expr, file, line);
	}
}

NS_END
