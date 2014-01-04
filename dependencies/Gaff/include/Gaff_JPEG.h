/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gaff_File.h"
#include <jpeglib.h>

NS_GAFF

class JPEG
{
public:
	enum OPEN_MODE {
		READ = 0,
		WRITE
	};

	JPEG(void);
	~JPEG(void);

	template <class Allocator>
	INLINE bool open(const Gaff::String<char, Allocator>& file_name, OPEN_MODE mode = READ)
	{
		assert(file_name.size());
		return open(file_name.getBuffer(), mode);
	}

	template <class Allocator>
	INLINE bool open(const Gaff::String<wchar_t, Allocator>& file_name, OPEN_MODE mode = READ)
	{
		assert(file_name.size());
		return open(file_name.getBuffer(), mode);
	}

	bool open(const wchar_t* file_name, OPEN_MODE mode = READ);
	bool open(const char* file_name, OPEN_MODE mode = READ);
	void close(void);

	INLINE J_COLOR_SPACE getColorSpace(void) const;
	INLINE unsigned int getWidthBytes(void) const;
	INLINE unsigned int getWidth(void) const;
	INLINE unsigned int getHeight(void) const;
	INLINE int getQuality(void) const;
	INLINE void setColorSpace(J_COLOR_SPACE color_space);
	INLINE void setWidth(unsigned int width);
	INLINE void setHeight(unsigned int height);
	INLINE void setQuality(int quality);

	bool readLine(unsigned char** buffer) const;
	bool readLine(unsigned char* buffer) const;
	unsigned int read(unsigned char** buffer) const;
	unsigned int read(unsigned char* buffer) const;
	bool write(unsigned char** buffer, unsigned int width, unsigned int height, int num_channels, J_COLOR_SPACE color_space, int quality);
	bool write(unsigned char* buffer, unsigned int width, unsigned int height, int num_channels, J_COLOR_SPACE color_space, int quality);
	INLINE bool write(unsigned char** buffer);
	INLINE bool write(unsigned char* buffer);

	JPEG(JPEG&& rhs);
	const JPEG& operator=(JPEG&& rhs);

private:
	union
	{
		mutable jpeg_decompress_struct _decompress;
		mutable jpeg_compress_struct _compress;
	};

	jpeg_error_mgr _jerr;
	int _quality;

	File _file;

	bool openRead(void);
	bool openWrite(void);

	GAFF_NO_COPY(JPEG);
};

NS_END
