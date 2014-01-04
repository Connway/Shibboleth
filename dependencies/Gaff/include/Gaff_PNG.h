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
#include <png.h>

NS_GAFF

class PNG
{
public:
	enum OPEN_MODE {
		READ = 0,
		WRITE
	};

	PNG(void);
	~PNG(void);

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

	INLINE unsigned char getColorType(void) const;
	INLINE unsigned char getBitDepth(void) const;
	INLINE unsigned int getWidthBytes(void) const;
	INLINE unsigned int getWidth(void) const;
	INLINE unsigned int getHeight(void) const;
	INLINE void setColorType(unsigned char color_type);
	INLINE void setBitDepth(unsigned char bit_depth);
	INLINE void setWidth(unsigned int width);
	INLINE void setHeight(unsigned int height);

	INLINE void read(unsigned char** buffer) const;
	bool write(unsigned char** buffer, unsigned int width, unsigned int height, unsigned char bit_depth, unsigned char color_type);
	INLINE bool write(unsigned char** buffer);

	PNG(PNG&& rhs);
	const PNG& operator=(PNG&& rhs);

private:
	unsigned int _width_bytes;
	unsigned char _bit_depth;
	unsigned char _color_type;
	unsigned int _width;
	unsigned int _height;

	png_struct* _png;
	png_info* _info;

	File _file;

	bool loadReadInfo(void);
	bool loadWriteInfo(void);

	GAFF_NO_COPY(PNG);
};

NS_END
