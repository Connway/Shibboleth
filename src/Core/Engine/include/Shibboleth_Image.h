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

#include "Shibboleth_Vector.h"

NS_SHIBBOLETH

class Image final
{
public:
	int32_t getWidth(void) const;
	int32_t getHeight(void) const;
	int32_t getBitDepth(void) const;
	int32_t getNumChannels(void) const;
	const uint8_t* getBuffer(void) const;
	uint8_t* getBuffer(void);

	bool load(const void* buffer, size_t size, const char8_t* file_ext);
	bool load(const void* buffer, size_t size, const char* file_ext);
	bool loadTIFF(const void* buffer, size_t size);
	bool loadPNG(const void* buffer, size_t size);

private:
	int32_t _width = 0;
	int32_t _height = 0;
	int32_t _bit_depth = 0;
	int32_t _num_channels = 0;

	Vector<uint8_t> _image{ ProxyAllocator("Image") };
};

NS_END
