/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gaff_String.h"

NS_GAFF

class Image
{
public:
	enum Format
	{
		FMT_COLOR_INDEX = 0x1900,
		FMT_ALPHA = 0x1906,
		FMT_RGB = 0x1907,
		FMT_RGBA = 0x1908,
		FMT_BGR = 0x80E0,
		FMT_BGRA = 0x80E1,
		FMT_LUMINANCE = 0x1909,
		FMT_LIMUNANCE_ALPHA = 0x190A
	};

	enum Type
	{
		TYPE_BYTE = 0x1400,
		TYPE_UNSIGNED_BYTE = 0x1401,
		TYPE_SHORT = 0x1402,
		TYPE_UNSIGNED_SHORT = 0x1403,
		TYPE_INT = 0x1404,
		TYPE_UNSIGNED_INT = 0x1405,
		TYPE_FLOAT = 0x1406,
		TYPE_DOUBLE = 0x140A,
		TYPE_HALF = 0x140B
	};

	enum Language
	{
		LANG_ENGLISH = 0x0800,
		LANG_ARABIC = 0x0801,
		LANG_DUTCH = 0x0802,
		LANG_JAPANESE = 0x0803,
		LANG_SPANISH = 0x0804,
		LANG_GERMAN = 0x0805,
		LANG_FRENCH = 0x0806
	};

	enum Filter
	{
		FILTER_NEAREST = 0x2601,
		FILTER_LINEAR = 0x2602,
		FILTER_BILINEAR = 0x2603,
		FILTER_BOX = 0x2604,
		FILTER_TRIANGLE = 0x2605,
		FILTER_BELL = 0x2606,
		FILTER_BSPLINE = 0x2607,
		FILTER_LANCZOS = 0x2608, 
		FILTER_MITCHELL = 0x2609
	};

#if defined(_WIN32) || defined(_WIN64)
	#define MEMCB __stdcall
#else
	#define MEMCB
#endif

	typedef void* (MEMCB *AllocFunc)(const size_t);
	typedef void  (MEMCB *FreeFunc) (const void* const);

	template <class T, class Allocator>
	bool load(const String<T, Allocator>& filename)
	{
		assert(filename.size());
		return load(filename.getBuffer());
	}

	static void SysInit(void);
	static void SysShutdown(void);
	static void SetMemoryFunctions(AllocFunc af, FreeFunc ff);
	static const GChar* GetErrorString(unsigned int error);
	static void SetErrorLanguage(Language language);
	static unsigned int GetError(void);

	Image(const Image& image);
	Image(Image&& image);
	Image(void);
	~Image(void);

	const Image& operator=(const Image& rhs);
	const Image& operator=(Image&& rhs);

	bool init(void);
	void destroy();

	bool load(const wchar_t* filename);
	bool load(const char* filename);

	bool save(const wchar_t* filename, bool allow_overwrite = false);
	bool save(const char* filename, bool allow_overwrite = false);

	bool setImageProperties(unsigned int width, unsigned int height, unsigned int depth,
							unsigned char bytes_per_pixel, Format format, Type type);

	const unsigned char* getBuffer(void) const;
	bool writeBuffer(void* data);

	bool copy(const Image& src, unsigned int src_x, unsigned int src_y, unsigned int src_z,
				unsigned int width, unsigned int height, unsigned int depth,
				unsigned int dest_x, unsigned int dest_y, unsigned int dest_z);

	bool copy(const Image& src, unsigned int dest_x, unsigned int dest_y, unsigned int dest_z);

	bool scale(unsigned int width, unsigned int height, unsigned int depth, Filter filter = FILTER_NEAREST);

	unsigned int getWidth(void) const;
	unsigned int getHeight(void) const;
	unsigned int getDepth(void) const;
	unsigned int getBytesPerPixel(void) const;
	unsigned int getBitsPerPixel(void) const;
	unsigned int getNumChannels(void) const;
	Format getFormat(void) const;
	Type getType(void) const;

private:
	unsigned int _image;
};

NS_END
