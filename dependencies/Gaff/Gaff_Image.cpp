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

#include "Gaff_Image.h"
#include "Gaff_IncludeAssert.h"
#include <IL/ilu.h>
#include <IL/il.h>
#include <cstdlib>

#include <iostream>

NS_GAFF

void Image::SysInit(void)
{
	ilInit();
	iluInit();
}

void Image::SysShutdown(void)
{
	ilShutDown();
}

void Image::SetMemoryFunctions(AllocFunc af, FreeFunc ff)
{
	ilSetMemory(af, ff);
}

const GChar* Image::GetErrorString(unsigned int error)
{
	return iluErrorString(error);
}

void Image::SetErrorLanguage(Language language)
{
	assert(language >= LANG_ENGLISH && language <= LANG_FRENCH);
	iluSetLanguage(language);
}

unsigned int Image::GetError(void)
{
	return ilGetError();
}

Image::Image(const Image& image):
	_image(0)
{
	*this = image;
}

Image::Image(Image&& image):
	_image(image._image)
{
	image._image = 0;
}

Image::Image(void):
	_image(0)
{
}

Image::~Image(void)
{
	destroy();
}

const Image& Image::operator=(const Image& rhs)
{
	// If we haven't initialized yet, do so now
	if (!_image && !init()) {
		return *this;
	}

	ilBindImage(_image);
	ilCopyImage(rhs._image);

	return *this;
}

const Image& Image::operator=(Image&& rhs)
{
	assert(!_image);
	_image = rhs._image;
	rhs._image = 0;
	return *this;
}

bool Image::init(void)
{
	assert(!_image);
	_image = ilGenImage();
	return _image != 0;
}

void Image::destroy()
{
	if (_image) {
		ilDeleteImage(_image);
		_image = 0;
	}
}

bool Image::load(const wchar_t* filename)
{
	assert(_image);
	ilBindImage(_image);

#ifdef _UNICODE
	return ilLoadImage(filename) != 0;
#else
	char buffer[256];
	wcstombs(buffer, filename, 256);
	return ilLoadImage(buffer) != 0;
#endif
}

bool Image::load(const char* filename)
{
	assert(_image);
	ilBindImage(_image);

#ifdef _UNICODE
	wchar_t buffer[256];
	mbstowcs(buffer, filename, 256);
	return ilLoadImage(buffer) != 0;
#else
	return ilLoadImage(filename) != 0;
#endif
}

bool Image::save(const wchar_t* filename, bool allow_overwrite)
{
	assert(_image);
	ilBindImage(_image);

	// We are allowing overwrites, but we failed to enable
	if (allow_overwrite && ilEnable(IL_FILE_OVERWRITE)) {
		return false;
	}

#ifdef _UNICODE
	bool ret = ilSaveImage(filename) != 0;
#else
	char buffer[256];
	wcstombs(buffer, filename, 256);
	bool ret = ilSaveImage(buffer) != 0;
#endif

	ilDisable(IL_FILE_OVERWRITE);
	return ret;
}

bool Image::save(const char* filename, bool allow_overwrite)
{
	assert(_image);
	ilBindImage(_image);

	// We are allowing overwrites, but we failed to enable
	if (allow_overwrite && !ilEnable(IL_FILE_OVERWRITE)) {
		return false;
	}

#ifdef _UNICODE
	wchar_t buffer[256];
	mbstowcs(buffer, filename, 256);
	bool ret = ilSaveImage(buffer) != 0;
#else
	bool ret = ilSaveImage(filename) != 0;
#endif

	ilDisable(IL_FILE_OVERWRITE);
	return ret;
}

bool Image::setImageProperties(unsigned int width, unsigned int height, unsigned int depth,
								unsigned char bytes_per_pixel, Format format, Type type)
{
	assert(_image);
	ilBindImage(_image);
	return ilTexImage(width, height, depth, bytes_per_pixel, format, type, nullptr) != 0;
}

const unsigned char* Image::getBuffer(void) const
{
	assert(_image);
	ilBindImage(_image);
	return ilGetData();
}

bool Image::writeBuffer(void* buffer)
{
	assert(_image);
	ilBindImage(_image);
	return ilSetData(buffer) != 0;
}

bool Image::copy(const Image& src, unsigned int src_x, unsigned int src_y, unsigned int src_z,
				unsigned int width, unsigned int height, unsigned int depth,
				unsigned int dest_x, unsigned int dest_y, unsigned int dest_z)
{
	assert(_image && src._image);
	ilBindImage(_image);
	return ilBlit(src._image, dest_x, dest_y, dest_z, src_x, src_y, src_z, width, height, depth) != 0;
}

bool Image::copy(const Image& src, unsigned int dest_x, unsigned int dest_y, unsigned int dest_z)
{
	assert(_image && src._image);
	ilBindImage(_image);
	return ilOverlayImage(src._image, dest_x, dest_y, dest_z) != 0;
}

bool Image::scale(unsigned int width, unsigned int height, unsigned int depth, Filter filter)
{
	iluImageParameter(ILU_FILTER, filter);
	return iluScale(width, height, depth) != 0;
}

unsigned int Image::getWidth(void) const
{
	assert(_image);
	ilBindImage(_image);
	return (unsigned int)ilGetInteger(IL_IMAGE_WIDTH);
}

unsigned int Image::getHeight(void) const
{
	assert(_image);
	ilBindImage(_image);
	return (unsigned int)ilGetInteger(IL_IMAGE_HEIGHT);
}
unsigned int Image::getDepth(void) const
{
	assert(_image);
	ilBindImage(_image);
	return (unsigned int)ilGetInteger(IL_IMAGE_DEPTH);
}

unsigned int Image::getBytesPerPixel(void) const
{
	assert(_image);
	ilBindImage(_image);
	return (unsigned int)ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
}

unsigned int Image::getBitsPerPixel(void) const
{
	assert(_image);
	ilBindImage(_image);
	return (unsigned int)ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);
}

unsigned int Image::getNumChannels(void) const
{
	assert(_image);
	ilBindImage(_image);
	return (unsigned int)ilGetInteger(IL_IMAGE_CHANNELS);
}

Image::Format Image::getFormat(void) const
{
	assert(_image);
	ilBindImage(_image);
	return (Format)ilGetInteger(IL_IMAGE_FORMAT);
}

Image::Type Image::getType(void) const
{
	assert(_image);
	ilBindImage(_image);
	return (Type)ilGetInteger(IL_IMAGE_TYPE);
}

NS_END
