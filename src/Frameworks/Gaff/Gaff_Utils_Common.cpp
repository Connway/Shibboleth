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

#include "Gaff_Utils.h"

NS_GAFF

void GetCurrentTimeString(char8_t* buff, size_t size, const char8_t* format)
{
	GetCurrentTimeString(reinterpret_cast<char*>(buff), size, reinterpret_cast<const char*>(format));
}

void GetCurrentTimeString(char* buff, size_t size, const char* format)
{
	time_t t = time(nullptr);
	tm* now = localtime(&t);
	strftime(buff, size, format, now);
}

void GetCurrentTimeString(wchar_t* buff, size_t size, const wchar_t* format)
{
	time_t t = time(nullptr);
	tm* now = localtime(&t);
	wcsftime(buff, size, format, now);
}

NS_END
