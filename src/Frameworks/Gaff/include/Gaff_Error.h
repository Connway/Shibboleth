/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Gaff_Flags.h"

NS_GAFF

struct Error
{
	enum class Flag
	{
		HasError,
		Fatal,

		Count
	};

	bool hasError(void) const { return flags.testAll(Flag::HasError); }
	bool isFatal(void) const { return flags.testAll(Flag::Fatal); }
	bool isOk(void) const { return flags.empty(); }

	operator bool(void) const { return hasError(); }

	Gaff::Flags<Flag> flags;

	static const Error k_fatal_error;
	static const Error k_simple_error;
	static const Error k_no_error;
};

template <class ErrorData>
struct ErrorWithData final : public Error
{
	ErrorData error;

	static const ErrorWithData<ErrorData> k_no_error;
};

template <class ErrorData>
const ErrorWithData<ErrorData> ErrorWithData<ErrorData>::k_no_error{};

NS_END
