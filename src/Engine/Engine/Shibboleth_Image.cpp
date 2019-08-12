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
#include "Shibboleth_LogManager.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_String.h"
#include "Shibboleth_Vector.h"
#include "Shibboleth_IApp.h"
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

bool Image::load(const void* buffer, size_t size, const char* file_ext, const FinishReadingCallback finish_reading_callback, void* callback_data)
{
	if (Gaff::EndsWith(file_ext, ".png")) {
		return loadPNG(buffer, size, finish_reading_callback, callback_data);
	} else if (Gaff::EndsWith(file_ext, ".tiff") || Gaff::EndsWith(file_ext, ".tif")) {
		//return loadTIFF(buffer, size, finish_reading_callback, callback_data);
	}

	return false;
}

bool Image::loadPNG(const void* buffer, size_t size, const FinishReadingCallback finish_reading_callback, void* callback_data)
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
		return false;
	}

	// Give calling code a chance to free up memory, as we no longer need the buffer.
	if (finish_reading_callback) {
		finish_reading_callback(callback_data);
	}

	//const png_byte num_channels = png_get_channels(png_ptr, info_ptr);

	switch (color_type) {
		case PNG_COLOR_TYPE_RGB:
			//_image.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * (static_cast<size_t>(bit_depth) / 8) * 4);
			break;

		case PNG_COLOR_TYPE_RGBA:
			//_image.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * (static_cast<size_t>(bit_depth) / 8) * 4);
			break;

		case PNG_COLOR_TYPE_GRAY:
			// Not supported for now.
			return false;
			break;

		case PNG_COLOR_TYPE_GA:
			// Not supported for now.
			return false;
			break;
	}

	png_destroy_read_struct(&png_ptr, nullptr, nullptr);
	return true;
}

NS_END
