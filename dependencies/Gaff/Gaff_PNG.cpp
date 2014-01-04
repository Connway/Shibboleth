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

#include "Gaff_PNG.h"

NS_GAFF

PNG::PNG(void):
	_width_bytes(0), _bit_depth(0),
	_color_type(0), _width(0),
	_height(0), _png(nullptr),
	_info(nullptr)
{
}

PNG::~PNG(void)
{
	close();
}

bool PNG::open(const wchar_t* file_name, OPEN_MODE mode)
{
	assert(mode == READ || mode == WRITE);
	
	File::OPEN_MODE final_mode = (mode == READ) ? File::READ_BINARY : File::WRITE_BINARY;

	if (!_file.open(file_name, final_mode)) {
		return false;
	}

	return (mode == READ) ? loadReadInfo() : loadWriteInfo();
}

bool PNG::open(const char* file_name, OPEN_MODE mode)
{
	assert(mode == READ || mode == WRITE);

	File::OPEN_MODE final_mode = (mode == READ) ? File::READ_BINARY : File::WRITE_BINARY;

	if (!_file.open(file_name, final_mode)) {
		return false;
	}

	return (mode == READ) ? loadReadInfo() : loadWriteInfo();
}

void PNG::close(void)
{
	_bit_depth = _color_type = 0;
	_width_bytes = _width =
	_height = 0;
	_png = nullptr;
	_info = nullptr;
	_file.close();
}

unsigned char PNG::getColorType(void) const
{
	return _color_type;
}

unsigned char PNG::getBitDepth(void) const
{
	return _bit_depth;
}

unsigned int PNG::getWidthBytes(void) const
{
	return _width_bytes;
}

unsigned int PNG::getWidth(void) const
{
	return _width;
}

unsigned int PNG::getHeight(void) const
{
	return _height;
}

void PNG::setColorType(unsigned char color_type)
{
	_color_type = color_type;
}

void PNG::setBitDepth(unsigned char bit_depth)
{
	_bit_depth = bit_depth;
}

void PNG::setWidth(unsigned int width)
{
	_width = width;
}

void PNG::setHeight(unsigned int height)
{
	_height = height;
}

void PNG::read(unsigned char** buffer) const
{
	png_read_image(_png, buffer);
}

bool PNG::write(unsigned char** buffer, unsigned int width, unsigned int height, unsigned char bit_depth, unsigned char color_type)
{
	assert(width > 0 && height > 0 && bit_depth > 0);

	/* write header */
	if (setjmp(png_jmpbuf(_png))) {
		//abort_("[write_png_file] Error during writing header");
		return false;
	}

	png_set_IHDR(_png, _info, width, height,
				bit_depth, color_type, PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(_png, _info);

	/* write bytes */
	if (setjmp(png_jmpbuf(_png))) {
		//abort_("[write_png_file] Error during writing bytes");
		return false;
	}

	png_write_image(_png, buffer);

	/* end write */
	if (setjmp(png_jmpbuf(_png))) {
		//abort_("[write_png_file] Error during end of write");
		return false;
	}

	png_write_end(_png, nullptr);
	return true;
}

bool PNG::write(unsigned char** buffer)
{
	return write(buffer, _width, _height, _bit_depth, _color_type);
}

bool PNG::loadReadInfo(void)
{
	unsigned char header[8];

	_file.read(header, 1, 8);

	if (png_sig_cmp(header, 0, 8)) {
		// not recognized as png
		return false;
	}

	_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	// until I can figure out how their memory allocation works
	//png_structp png = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL, );

	if (!_png) {
		// failed to create read struct
		return false;
	}

	_info = png_create_info_struct(_png);

	if (!_info) {
		// failed to create info struct
		return false;
	}

	if (setjmp(png_jmpbuf(_png))) {
		// failed to init io
		return false;
	}

	png_init_io(_png, _file.getFile());
	png_set_sig_bytes(_png, 8);
	png_read_info(_png, _info);

	_width_bytes = (unsigned int)png_get_rowbytes(_png, _info);
	_width = png_get_image_width(_png, _info);
	_height = png_get_image_height(_png, _info);
	_color_type = png_get_color_type(_png, _info);
	_bit_depth = png_get_bit_depth(_png, _info);
	png_set_interlace_handling(_png);

	return true;
}

bool PNG::loadWriteInfo(void)
{
	_png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!_png) {
		return false;
	}

	_info = png_create_info_struct(_png);

	if (!_info) {
		return false;
	}

	if (setjmp(png_jmpbuf(_png))) {
		// failed to init io
		return false;
	}

	png_init_io(_png, _file.getFile());

	return true;
}

PNG::PNG(PNG&& rhs):
	_bit_depth(rhs._bit_depth), _color_type(rhs._color_type),
	_width(rhs._width), _height(rhs._height),
	_png(rhs._png), _info(rhs._info),
	_file((File&&)rhs._file)
{
	rhs._bit_depth = rhs._color_type = 0;
	rhs._width_bytes = rhs._width =
	rhs._height = 0;
	rhs._png = nullptr;
	rhs._info = nullptr;
}

const PNG& PNG::operator=(PNG&& rhs)
{
	_width_bytes = rhs._width_bytes;
	_bit_depth = rhs._bit_depth;
	_color_type = rhs._color_type;
	_width = rhs._width;
	_height = rhs._height;
	_file = (File&&) rhs._file;

	rhs._bit_depth = rhs._color_type = 0;
	rhs._width_bytes = rhs._width =
	rhs._height = 0;
	rhs._png = nullptr;
	rhs._info = nullptr;

	return *this;
}

NS_END

