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

#include "Gaff_Image.h"
#include "Gaff_Assert.h"
#include <IL/ilu.h>
#include <IL/il.h>

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

void Image::SetErrorLanguage(ErrorLanguage language)
{
	GAFF_ASSERT(language >= ERR_LANG_ENGLISH && language <= ERR_LANG_FRENCH);
	iluSetLanguage(language);
}

unsigned int Image::GetError(void)
{
	return ilGetError();
}

const char* Image::GetErrorString(unsigned int error)
{
#ifdef PLATFORM_WINDOWS
	static char temp[STRING_CONVERSION_BUFFER_SIZE] = { 0 };

	const wchar_t* str = iluErrorString(error);
	char* temp_begin = temp;
	char* temp_end = temp + STRING_CONVERSION_BUFFER_SIZE;

	eastl::DecodePart(str, str + eastl::CharStrlen(str), temp_begin, temp_end);
	return temp;
#else
	return iluErrorString(error);
#endif
}

Image::Image(const Image& image):
	_image(0), _initialized(false)
{
	*this = image;
}

Image::Image(Image&& image):
	_image(image._image), _initialized(true)
{
	image._image = 0;
}

Image::Image(void):
	_image(0), _initialized(false)
{
}

Image::~Image(void)
{
	destroy();
}

const Image& Image::operator=(const Image& rhs)
{
	// If we haven't initialized yet, do so now
	if (!_initialized && !init()) {
		return *this;
	}

	ilBindImage(_image);
	ilCopyImage(rhs._image);

	return *this;
}

const Image& Image::operator=(Image&& rhs)
{
	GAFF_ASSERT(!_initialized);
	_image = rhs._image;
	rhs._image = 0;
	return *this;
}

bool Image::init(void)
{
	GAFF_ASSERT(!_initialized);
	_image = ilGenImage();
	_initialized = (ilGetError() == IL_NO_ERROR);
	return _initialized;
}

void Image::destroy()
{
	if (_initialized) {
		ilDeleteImage(_image);
		_image = 0;
	}
}

/*!
	\brief Loads an image from memory.
	\param image Image buffer.
	\param image_size Size of \a image in bytes.
	\return Whether the image was successfully loaded.
*/
bool Image::load(const void* image, unsigned int image_size)
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return ilLoadL(IL_TYPE_UNKNOWN, image, image_size) != 0;
}

bool Image::load(const char* filename)
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);

#ifdef PLATFORM_WINDOWS
	CONVERT_STRING(wchar_t, temp, filename);
	return ilLoadImage(temp) != 0;
#else
	return ilLoadImage(filename) != 0;
#endif
}

bool Image::save(const char* filename, bool allow_overwrite)
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);

	// We are allowing overwrites, but we failed to enable
	if (allow_overwrite && !ilEnable(IL_FILE_OVERWRITE)) {
		return false;
	}

#ifdef PLATFORM_WINDOWS
	CONVERT_STRING(wchar_t, temp, filename);
	bool ret = ilSaveImage(temp) != 0;
#else
	bool ret = ilSaveImage(filename) != 0;
#endif

	ilDisable(IL_FILE_OVERWRITE);
	return ret;
}

bool Image::setImageProperties(unsigned int width, unsigned int height, unsigned int depth,
								unsigned char bytes_per_pixel, Format format, Type type)
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return ilTexImage(width, height, depth, bytes_per_pixel, format, type, nullptr) != 0;
}

const unsigned char* Image::getBuffer(void) const
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return ilGetData();
}

bool Image::writeBuffer(void* buffer)
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return ilSetData(buffer) != 0;
}

bool Image::copy(const Image& src, unsigned int src_x, unsigned int src_y, unsigned int src_z,
				unsigned int width, unsigned int height, unsigned int depth,
				unsigned int dest_x, unsigned int dest_y, unsigned int dest_z)
{
	GAFF_ASSERT(_initialized && src._initialized);
	ilBindImage(_image);
	return ilBlit(src._image, dest_x, dest_y, dest_z, src_x, src_y, src_z, width, height, depth) != 0;
}

bool Image::copy(const Image& src, unsigned int dest_x, unsigned int dest_y, unsigned int dest_z)
{
	GAFF_ASSERT(_initialized && src._initialized);
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
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return static_cast<unsigned int>(ilGetInteger(IL_IMAGE_WIDTH));
}

unsigned int Image::getHeight(void) const
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return static_cast<unsigned int>(ilGetInteger(IL_IMAGE_HEIGHT));
}
unsigned int Image::getDepth(void) const
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return static_cast<unsigned int>(ilGetInteger(IL_IMAGE_DEPTH));
}

unsigned int Image::getBytesPerPixel(void) const
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return static_cast<unsigned int>(ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL));
}

unsigned int Image::getBitsPerPixel(void) const
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return static_cast<unsigned int>(ilGetInteger(IL_IMAGE_BITS_PER_PIXEL));
}

unsigned int Image::getNumChannels(void) const
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return static_cast<unsigned int>(ilGetInteger(IL_IMAGE_CHANNELS));
}

Image::Format Image::getFormat(void) const
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return static_cast<Format>(ilGetInteger(IL_IMAGE_FORMAT));
}

Image::Type Image::getType(void) const
{
	GAFF_ASSERT(_initialized);
	ilBindImage(_image);
	return static_cast<Type>(ilGetInteger(IL_IMAGE_TYPE));
}

NS_END