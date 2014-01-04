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

#include "Gaff_JPEG.h"

NS_GAFF

JPEG::JPEG(void):
	_quality(100)
{
}

JPEG::~JPEG(void)
{
	close();
}

bool JPEG::open(const wchar_t* file_name, OPEN_MODE mode)
{
	assert(mode == READ || mode == WRITE);

	File::OPEN_MODE final_mode = (mode == READ) ? File::READ_BINARY : File::WRITE_BINARY;

	if (!_file.open(file_name, final_mode)) {
		return false;
	}

	if ((mode == READ && !openRead()) || (mode == WRITE && !openWrite())) {
		close();
		return false;
	}

	return true;
}

bool JPEG::open(const char* file_name, OPEN_MODE mode)
{
	assert(mode == READ || mode == WRITE);

	File::OPEN_MODE final_mode = (mode == READ) ? File::READ_BINARY : File::WRITE_BINARY;

	if (!_file.open(file_name, final_mode)) {
		return false;
	}

	if ((mode == READ && !openRead()) || (mode == WRITE && !openWrite())) {
		close();
		return false;
	}

	return true;
}

void JPEG::close(void)
{
	if (_file.isOpen()) {
		if (_file.getMode() == File::READ_BINARY) {
			jpeg_destroy_decompress(&_decompress);
		} else {
			jpeg_destroy_compress(&_compress);
		}

		_file.close();
	}
}

J_COLOR_SPACE JPEG::getColorSpace(void) const
{
	return (_file.getMode() == File::READ_BINARY) ? _decompress.out_color_space : _compress.in_color_space;
}

unsigned int JPEG::getWidthBytes(void) const
{
	return (_file.getMode() == File::READ_BINARY) ?
		_decompress.output_components * _decompress.output_width :
		_compress.input_components * _compress.image_width;
}

unsigned int JPEG::getWidth(void) const
{
	return (_file.getMode() == File::READ_BINARY) ? _decompress.output_width : _compress.image_width;
}

unsigned int JPEG::getHeight(void) const
{
	return (_file.getMode() == File::READ_BINARY) ? _decompress.output_height : _compress.image_height;
}

int JPEG::getQuality(void) const
{
	assert(_file.getMode() == File::WRITE_BINARY);
	return _quality;
}

void JPEG::setColorSpace(J_COLOR_SPACE color_space)
{
	assert(_file.getMode() == File::WRITE_BINARY);
	_compress.in_color_space = color_space;

	switch (color_space) {
		case JCS_GRAYSCALE:		/* monochrome */
			_compress.input_components = 1;
			break;

		case JCS_RGB:		/* red/green/blue */
		case JCS_YCbCr:		/* Y/Cb/Cr (also known as YUV) */
			_compress.input_components = 3;
			break;

		case JCS_CMYK:		/* C/M/Y/K */
		case JCS_YCCK:		/* Y/Cb/Cr/K */
			_compress.input_components = 4;
			break;
	}
}

void JPEG::setWidth(unsigned int width)
{
	assert(_file.getMode() == File::WRITE_BINARY);
	_compress.image_width = width;
}

void JPEG::setHeight(unsigned int height)
{
	assert(_file.getMode() == File::WRITE_BINARY);
	_compress.image_height = height;
}

void JPEG::setQuality(int quality)
{
	assert(_file.getMode() == File::WRITE_BINARY);
	_quality = quality;
}

bool JPEG::readLine(unsigned char** buffer) const
{
	bool success = jpeg_read_scanlines(&_decompress, buffer, 1) == 1;

	if (_decompress.output_scanline == _decompress.output_height || !success) {
		jpeg_finish_decompress(&_decompress);
	}

	return success;
}

bool JPEG::readLine(unsigned char* buffer) const
{
	JSAMPROW row_pointer[1];
	row_pointer[0] = buffer + _decompress.output_scanline * _decompress.output_width * _decompress.output_components;

	bool success = jpeg_read_scanlines(&_decompress, row_pointer, 1) == 1;

	if (_decompress.output_scanline == _decompress.output_height || !success) {
		jpeg_finish_decompress(&_decompress);
	}

	return success;
}

unsigned int JPEG::read(unsigned char** buffer) const
{
	while (_decompress.output_scanline < _decompress.output_height) {
		unsigned int lines_read = jpeg_read_scanlines(&_decompress, buffer + _decompress.output_scanline, _decompress.output_height - _decompress.output_scanline);

		if (!lines_read) {
			jpeg_finish_decompress(&_decompress);
			return _decompress.output_scanline;
		}
	}

	jpeg_finish_decompress(&_decompress);

	return _decompress.output_scanline;
}

unsigned int JPEG::read(unsigned char* buffer) const
{
	JSAMPROW row_pointer[1];

	while (_decompress.output_scanline < _decompress.output_height) {
		row_pointer[0] = buffer + _decompress.output_scanline * _decompress.output_width * _decompress.output_components;
		unsigned int lines_read = jpeg_read_scanlines(&_decompress, row_pointer, _decompress.output_height - _decompress.output_scanline);

		if (!lines_read) {
			jpeg_finish_decompress(&_decompress);
			return _decompress.output_scanline;
		}
	}

	jpeg_finish_decompress(&_decompress);

	return _decompress.output_scanline;
}

bool JPEG::write(unsigned char** buffer, unsigned int width, unsigned int height, int num_channels, J_COLOR_SPACE color_space, int quality)
{
	_compress.image_width = width;
	_compress.image_height = height;
	_compress.input_components = num_channels;
	_compress.in_color_space = color_space;

	jpeg_set_defaults(&_compress);
	jpeg_set_quality(&_compress, quality, true);
	jpeg_start_compress(&_compress, true);

	while (_compress.next_scanline < _compress.image_height) {
		unsigned int lines_written = jpeg_write_scanlines(&_compress, buffer + _compress.next_scanline, _compress.image_height - _compress.next_scanline);

		// something went wrong, we didn't write any lines
		if (!lines_written) {
			jpeg_finish_compress(&_compress);
			return false;
		}
	}

	jpeg_finish_compress(&_compress);
	return true;
}

bool JPEG::write(unsigned char* buffer, unsigned int width, unsigned int height, int num_channels, J_COLOR_SPACE color_space, int quality)
{
	_compress.image_width = width;
	_compress.image_height = height;
	_compress.input_components = num_channels;
	_compress.in_color_space = color_space;

	jpeg_set_defaults(&_compress);
	jpeg_set_quality(&_compress, quality, true);
	jpeg_start_compress(&_compress, true);

	JSAMPROW row_pointer[1];

	while (_compress.next_scanline < _compress.image_height) {
		row_pointer[0] = (buffer + _compress.next_scanline * width * num_channels);
		unsigned int lines_written = jpeg_write_scanlines(&_compress, row_pointer, 1);

		// something went wrong, we didn't write any lines
		if (!lines_written) {
			jpeg_finish_compress(&_compress);
			return false;
		}
	}

	jpeg_finish_compress(&_compress);
	return true;
}

bool JPEG::write(unsigned char** buffer)
{
	return write(buffer, _compress.image_width, _compress.image_height, _compress.input_components, _compress.in_color_space, _quality);
}

bool JPEG::write(unsigned char* buffer)
{
	return write(buffer, _compress.image_width, _compress.image_height, _compress.input_components, _compress.in_color_space, _quality);
}

bool JPEG::openRead(void)
{
	_decompress.err = jpeg_std_error(&_jerr);
	jpeg_create_decompress(&_decompress);

	jpeg_stdio_src(&_decompress, (FILE*)_file.getFile());

	if (!jpeg_read_header(&_decompress, true)) {
		return false;
	}

	if (!jpeg_start_decompress(&_decompress)) {
		return false;
	}

	return true;
}

bool JPEG::openWrite(void)
{
	_compress.err = jpeg_std_error(&_jerr);
	jpeg_create_compress(&_compress);

	jpeg_stdio_dest(&_compress, (FILE*)_file.getFile());

	return true;
}

NS_END
