/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
#include "Shibboleth_LogManager.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_String.h"
#include "Shibboleth_Vector.h"
#include "Shibboleth_IApp.h"
#include <Gaff_Math.h>
#include <tiffio.h>
#include <png.h>

NS_SHIBBOLETH

static ProxyAllocator g_image_allocator("Image");

static void* ImageMalloc(png_structp, png_alloc_size_t size)
{
	return SHIB_ALLOC(size, g_image_allocator);
}

static void ImageFree(png_structp, void* ptr)
{
	SHIB_FREE(ptr, g_image_allocator);
}

static void ImageWarning(png_structp, png_const_charp message)
{
	LogWarningDefault("%s", message);
}

static void ImageError(png_structp, png_const_charp message)
{
	LogErrorDefault("%s", message);
}

struct BufferData final
{
	const void* buffer;
	const size_t size;
	size_t curr_byte_offset;
};

static void PNGRead(png_structp png_ptr, png_bytep out_buffer, png_size_t out_size)
{
	const png_voidp io_ptr = png_get_io_ptr(png_ptr);

	if (!io_ptr) {
		return;
	}

	BufferData* const buffer_data = reinterpret_cast<BufferData*>(io_ptr);
	const size_t read_size = Gaff::Min(buffer_data->size - buffer_data->curr_byte_offset, out_size);

	memcpy(out_buffer, reinterpret_cast<const int8_t*>(buffer_data->buffer) + buffer_data->curr_byte_offset, read_size);
	buffer_data->curr_byte_offset += read_size;
}

static tsize_t TIFFRead(thandle_t st, tdata_t buffer, tsize_t size)
{
	const BufferData* const data = reinterpret_cast<const BufferData*>(st);
	const size_t bytes_read = Gaff::Min(static_cast<size_t>(size), data->size - data->curr_byte_offset);

	memcpy(buffer, data->buffer, bytes_read);
	return bytes_read;
}

static tsize_t TIFFWrite(thandle_t st, tdata_t buffer, tsize_t size)
{
	//BufferData* const data = reinterpret_cast<BufferData*>(st);
	//const size_t bytes_written = Gaff::Min(static_cast<size_t>(size), data->size - data->curr_byte_offset);

	//memcpy(reinterpret_cast<int8_t*>(data->buffer) + data->curr_byte_offset, buffer, bytes_written);
	//return bytes_written;

	GAFF_REF(st, buffer, size);
	return 0;
}

static int TIFFClose(thandle_t)
{
	return 0;
}

static toff_t TIFFSeek(thandle_t st, toff_t pos, int whence)
{
	if (pos == 0xFFFFFFFF) {
		return 0xFFFFFFFF;
	}

	BufferData* const data = reinterpret_cast<BufferData*>(st);

	switch (whence) {
		case SEEK_SET:
			GAFF_ASSERT(pos < data->size);
			data->curr_byte_offset = pos;
			break;

		case SEEK_CUR:
			GAFF_ASSERT((data->curr_byte_offset + pos) < data->size);
			data->curr_byte_offset += pos;
			break;

		case SEEK_END:
			// Unsupported.
			break;
	}

	return data->curr_byte_offset;
}

static toff_t TIFFSize(thandle_t st)
{
	return reinterpret_cast<const BufferData*>(st)->size;
}

static int TIFFMap(thandle_t st, tdata_t* buffer, toff_t* size)
{
	BufferData* const data = reinterpret_cast<BufferData*>(st);
	*buffer = const_cast<void*>(data->buffer);
	*size = data->size;

	return 1;
}

static void TIFFError(const char* module, const char* format, va_list va)
{
	CONVERT_STRING(char8_t, temp_module, module);
	CONVERT_STRING(char8_t, temp_format, format);

	U8String format_string;
	format_string.sprintf_va_list(temp_format, va);

	LogErrorDefault("TIFF [%s]: %s", temp_module, temp_format);
}

static void TIFFWarning(const char* module, const char* format, va_list va)
{
	CONVERT_STRING(char8_t, temp_module, module);
	CONVERT_STRING(char8_t, temp_format, format);

	U8String format_string;
	format_string.sprintf_va_list(temp_format, va);

	LogWarningDefault("TIFF [%s]: %s", temp_module, temp_format);
}


static void TIFFUnmap(thandle_t, tdata_t, toff_t)
{
}


int32_t Image::getWidth(void) const
{
	return _width;
}

int32_t Image::getHeight(void) const
{
	return _height;
}

int32_t Image::getBitDepth(void) const
{
	return _bit_depth;
}

int32_t Image::getNumChannels(void) const
{
	return _num_channels;
}

const uint8_t* Image::getBuffer(void) const
{
	return _image.data();
}

uint8_t* Image::getBuffer(void)
{
	return _image.data();
}

bool Image::load(const void* buffer, size_t size, const char8_t* file_ext)
{
	if (Gaff::EndsWith(file_ext, u8".png")) {
		return loadPNG(buffer, size);
	} else if (Gaff::EndsWith(file_ext, u8".tiff") || Gaff::EndsWith(file_ext, u8".tif")) {
		return loadTIFF(buffer, size);
	}

	return false;
}

bool Image::load(const void* buffer, size_t size, const char* file_ext)
{
	CONVERT_STRING(char8_t, temp_file_ext, file_ext);
	return load(buffer, size, temp_file_ext);
}

bool Image::loadTIFF(const void* buffer, size_t size)
{
	TIFFSetWarningHandler(TIFFWarning);
	TIFFSetErrorHandler(TIFFError);

	BufferData data = { buffer, size, 0 };
	TIFF* const tiff = TIFFClientOpen(
		"Memory",
		"r",
		&data,
		TIFFRead,
		TIFFWrite,
		TIFFSeek,
		TIFFClose,
		TIFFSize,
		TIFFMap,
		TIFFUnmap
	);

	uint32_t width;
	uint32_t height;

	TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);

	_image.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * sizeof(uint32_t));

	const bool success = TIFFReadRGBAImageOriented(tiff, width, height, reinterpret_cast<uint32_t*>(_image.data()), ORIENTATION_TOPLEFT);
	TIFFClose(tiff);

	if (success) {
		_width = static_cast<int32_t>(width);
		_height = static_cast<int32_t>(height);
		_bit_depth = 8;
		_num_channels = 4;
	}
	
	return success;
}

bool Image::loadPNG(const void* buffer, size_t size)
{
	constexpr size_t PNG_SIG_SIZE = 8;

	if (!png_check_sig(reinterpret_cast<png_const_bytep>(buffer), PNG_SIG_SIZE)) {
		return false;
	}

	png_structp png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, nullptr, ImageError, ImageWarning, nullptr, ImageMalloc, ImageFree);

	if (!png_ptr) {
		// $TODO: Log error.
		return false;
	}

	const png_infop info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr) {
		// $TODO: Log error.
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		return false;
	}

	BufferData data = { buffer, size, PNG_SIG_SIZE };
	png_set_read_fn(png_ptr, &data, PNGRead);

	// tell libpng we already read the signature
	png_set_sig_bytes(png_ptr, PNG_SIG_SIZE);

	png_read_info(png_ptr, info_ptr);

	png_uint_32 width = 0;
	png_uint_32 height = 0;
	int bit_depth = 0;
	int color_type = -1;

	const png_uint_32 retval = png_get_IHDR(
		png_ptr,
		info_ptr,
		&width,
		&height,
		&bit_depth,
		&color_type,
		nullptr,
		nullptr,
		nullptr
	);

	if (retval != 1) {
		// $TODO: Log error
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		return false;
	}

	const png_byte num_channels = png_get_channels(png_ptr, info_ptr);
	const size_t bytes_per_row = png_get_rowbytes(png_ptr, info_ptr);

	_image.resize(
		static_cast<size_t>(width) *
		static_cast<size_t>(height) *
		(static_cast<size_t>(bit_depth) / 8) *
		static_cast<size_t>(num_channels)
	);

	uint8_t* const start = _image.data();

	for (int32_t i = 0; i < static_cast<int32_t>(height); ++i) {
		const size_t byte_offset = static_cast<size_t>(i) * bytes_per_row;
		png_read_row(png_ptr, start + byte_offset, nullptr);
	}

	png_destroy_read_struct(&png_ptr, nullptr, nullptr);

	_width = static_cast<int32_t>(width);
	_height = static_cast<int32_t>(height);
	_bit_depth = static_cast<int32_t>(bit_depth);
	_num_channels = static_cast<int32_t>(num_channels);

	return true;
}

NS_END
