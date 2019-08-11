/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_Image.h"
#include "Shibboleth_String.h"

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable : 4267 4100 4611)
#endif

#define __STDC_LIB_EXT1__
#include <png.hpp>

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif

NS_SHIBBOLETH

class MemoryBuffer final : public std::streambuf
{
public:
	MemoryBuffer(const void* buffer, size_t size)
	{
		char* const start = reinterpret_cast<char*>(const_cast<void*>(buffer));
		char* const last = start + size;
		setg(start, start, last);
		setp(start, last);
	}
};

bool Image::load(const void* buffer, size_t size, const char* file_ext)
{
	if (Gaff::EndsWith(file_ext, ".png")) {
		return loadPNG(buffer, size);
	} else if (Gaff::EndsWith(file_ext, ".tiff") || Gaff::EndsWith(file_ext, ".tif")) {
		//return loadTIFF(buffer, size);
	}

	return false;
}

bool Image::loadPNG(const void* buffer, size_t size)
{
	MemoryBuffer mem_buf(buffer, size);
	std::istream stream(&mem_buf, false);

	png::image<png::rgba_pixel> image(stream);

	return false;
}

NS_END
