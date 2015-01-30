/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gleam_Texture_OpenGL.h"
#include <Gaff_IncludeAssert.h>
#include <Gaff_Math.h>
#include <GL/glew.h>
#include <cmath>

NS_GLEAM

static unsigned int _format_map[ITexture::FORMAT_SIZE] = {
	GL_R8,
	GL_R16,
	GL_RG8,
	GL_RG16,
	//GL_RGB8,
	GL_RGBA8,
	GL_RGBA16,

	GL_R8_SNORM,
	GL_R16_SNORM,
	GL_RG8_SNORM,
	GL_RG16_SNORM,
	//GL_RGB8_SNORM,
	GL_RGB16_SNORM,
	GL_RGBA8_SNORM,

	GL_R8I,
	GL_R16I,
	GL_R32I,
	GL_RG8I,
	GL_RG16I,
	GL_RG32I,
	//GL_RGB8I,
	//GL_RGB16I,
	GL_RGB32I,
	GL_RGBA8I,
	GL_RGBA16I,
	GL_RGBA32I,

	GL_R8UI,
	GL_R16UI,
	GL_R32UI,
	GL_RG8UI,
	GL_RG16UI,
	GL_RG32UI,
	//GL_RGB8UI,
	//GL_RGB16UI,
	GL_RGB32UI,
	GL_RGBA8UI,
	GL_RGBA16UI,
	GL_RGBA32UI,

	GL_R16F,
	GL_R32F,
	GL_RG16F,
	GL_RG32F,
	GL_RGB16F,
	GL_RGB32F,
	GL_RGBA16F,
	GL_RGBA32F,

	GL_R11F_G11F_B10F,
	GL_RGB9_E5,
	GL_RGB10_A2,
	GL_RGB10_A2UI,
	GL_SRGB8_ALPHA8,

	GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT32F,

	GL_DEPTH24_STENCIL8,
	GL_DEPTH32F_STENCIL8
};

static unsigned int _format_size[ITexture::FORMAT_SIZE] = {
	1,
	2,
	2,
	4,
	//3,
	4,
	8,

	1,
	2,
	2,
	4,
	//3,
	6,
	4,

	1,
	2,
	4,
	2,
	4,
	8,
	//3,
	//6,
	12,
	4,
	8,
	16,

	1,
	2,
	4,
	2,
	4,
	8,
	//3,
	//6,
	12,
	4,
	8,
	16,

	2,
	4,
	4,
	8,
	6,
	12,
	8,
	16,

	4,
	4,
	4,
	4,
	4,

	2,
	4,
	4,
	8
};

TextureGL::TextureGL(void):
	_texture(0)
{
}

TextureGL::~TextureGL(void)
{
	destroy();
}

void TextureGL::destroy(void)
{
	if (_texture != 0) {
		glDeleteTextures(1, &_texture);
	}
}

bool TextureGL::init3D(IRenderDevice&, int width, int height, int depth, FORMAT format, int mip_levels, const void* buffer)
{
	assert(width > 0 && height > 0 && depth > 0 && mip_levels > 0);

	_mip_levels = (unsigned int)mip_levels;
	_format = format;
	_type = THREED;
	_width = width;
	_height = height;
	_depth = depth;

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_3D, _texture);

	GLenum gl_format = _format_map[format];

	if (buffer) {
		GLenum gl_channels = determineChannels(format);
		GLenum gl_type = determineType(format);

		// if this is zero, something seriously went wrong
		assert(gl_channels != 0 && gl_type != 0);

		//glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width, height, depth, gl_channels, gl_type, buffer);
		//glTexImage3D(GL_TEXTURE_3D, 0, gl_format, width, height, depth, 0, gl_channels, gl_type, buffer);

		unsigned int byte_size = _format_size[format];

		for (int i = 0; i < mip_levels; ++i) {
			glTexImage3D(GL_TEXTURE_3D, i, gl_format, width, height, depth, 0, gl_channels, gl_type, buffer);
			buffer = ((unsigned char*)buffer) + width * height * depth * byte_size;
			width = Gaff::Max(1, width / 2);
			height = Gaff::Max(1, height / 2);
			depth = Gaff::Max(1, depth / 2);
		}

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, mip_levels);
		glGenerateMipmap(GL_TEXTURE_3D);

	} else {
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, mip_levels);
		glTexStorage3D(GL_TEXTURE_3D, mip_levels, gl_format, width, height, depth);
	}

	glBindTexture(GL_TEXTURE_3D, 0);
	glFinish();

	return glGetError() == GL_NO_ERROR;
}

bool TextureGL::init2D(IRenderDevice&, int width, int height, FORMAT format, int mip_levels, const void* buffer)
{
	assert(width > 0 && height > 0 && mip_levels > 0);

	_mip_levels = (unsigned int)mip_levels;
	_format = format;
	_type = TWOD;
	_width = width;
	_height = height;
	_depth = 0;

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	GLenum gl_format = _format_map[format];

	if (buffer) {
		GLenum gl_channels = determineChannels(format);
		GLenum gl_type = determineType(format);

		// if this is zero, something seriously went wrong
		assert(gl_channels != 0 && gl_type != 0);

		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, gl_channels, gl_type, buffer);
		//glTexImage2D(GL_TEXTURE_2D, 0, gl_format, width, height, 0, gl_channels, gl_type, buffer);

		unsigned int byte_size = _format_size[format];

		for (int i = 0; i < mip_levels; ++i) {
			glTexImage2D(GL_TEXTURE_2D, i, gl_format, width, height, 0, gl_channels, gl_type, buffer);
			buffer = ((unsigned char*)buffer) + width * height * byte_size;
			width = Gaff::Max(1, width / 2);
			height = Gaff::Max(1, height / 2);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_levels);
		glGenerateMipmap(GL_TEXTURE_2D);

	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_levels);
		glTexStorage2D(GL_TEXTURE_2D, mip_levels, gl_format, width, height);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glFinish();

	return glGetError() == GL_NO_ERROR;
}

bool TextureGL::init1D(IRenderDevice&, int width, FORMAT format, int mip_levels, const void* buffer)
{
	assert(width > 0 && mip_levels > 0);

	_mip_levels = (unsigned int)mip_levels;
	_format = format;
	_type = ONED;
	_width = width;
	_height = 0;
	_depth = 0;

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_1D, _texture);

	GLenum gl_format = _format_map[format];

	if (buffer) {
		GLenum gl_channels = determineChannels(format);
		GLenum gl_type = determineType(format);

		// if this is zero, something seriously went wrong
		assert(gl_channels != 0 && gl_type != 0);

		//glTexSubImage1D(GL_TEXTURE_1D, 0, 0, width, gl_channels, gl_type, buffer);
		//glTexImage1D(GL_TEXTURE_1D, 0, gl_format, width, 0, gl_channels, gl_type, buffer);

		unsigned int byte_size = _format_size[format];

		for (int i = 0; i < mip_levels; ++i) {
			glTexImage1D(GL_TEXTURE_1D, i, gl_format, width, 0, gl_channels, gl_type, buffer);
			buffer = ((unsigned char*)buffer) + width * byte_size;
			width = Gaff::Max(1, width / 2);
		}

		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, mip_levels);
		glGenerateMipmap(GL_TEXTURE_1D);

	} else {
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, mip_levels);
		glTexStorage1D(GL_TEXTURE_1D, mip_levels, gl_format, width);
	}

	glBindTexture(GL_TEXTURE_1D, 0);
	glFinish();

	return glGetError() == GL_NO_ERROR;
}

bool TextureGL::initCubemap(IRenderDevice&, int width, int height, FORMAT format, int mip_levels, const void* buffer)
{
	assert(width > 0 && height > 0 && mip_levels > 0);

	_mip_levels = (unsigned int)mip_levels;
	_format = format;
	_type = CUBE;
	_width = width;
	_height = height;
	_depth = 0;

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);

	GLenum gl_format = _format_map[format];

	if (buffer) {
		GLenum gl_channels = determineChannels(format);
		GLenum gl_type = determineType(format);

		// if this is zero, something seriously went wrong
		assert(gl_channels != 0 && gl_type != 0);

		//glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, width, height, gl_channels, gl_type, buf);
		//glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, width, height, gl_channels, gl_type, buf + buf_size);
		//glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, width, height, gl_channels, gl_type, buf + 2 * buf_size);
		//glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, width, height, gl_channels, gl_type, buf + 3 * buf_size);
		//glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, width, height, gl_channels, gl_type, buf + 4 * buf_size);
		//glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, width, height, gl_channels, gl_type, buf + 5 * buf_size);

		//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, gl_format, width, height, 0, gl_channels, gl_type, buf);
		//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, gl_format, width, height, 0, gl_channels, gl_type, buf + buf_size);
		//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, gl_format, width, height, 0, gl_channels, gl_type, buf + 2 * buf_size);
		//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, gl_format, width, height, 0, gl_channels, gl_type, buf + 3 * buf_size);
		//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, gl_format, width, height, 0, gl_channels, gl_type, buf + 4 * buf_size);
		//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, gl_format, width, height, 0, gl_channels, gl_type, buf + 5 * buf_size);

		unsigned int byte_size = _format_size[format];
		unsigned int buf_size = width * height * byte_size;
		const char* buf = (const char*)buffer;

		for (int i = 0; i < mip_levels; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, gl_format, width, height, 0, gl_channels, gl_type, buf);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, gl_format, width, height, 0, gl_channels, gl_type, buf + buf_size);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, gl_format, width, height, 0, gl_channels, gl_type, buf + 2 * buf_size);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, gl_format, width, height, 0, gl_channels, gl_type, buf + 3 * buf_size);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, gl_format, width, height, 0, gl_channels, gl_type, buf + 4 * buf_size);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, gl_format, width, height, 0, gl_channels, gl_type, buf + 5 * buf_size);

			width = Gaff::Max(1, width / 2);
			height = Gaff::Max(1, height / 2);
			buf_size = width * height * byte_size;
			buf += 6 * buf_size;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_levels);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip_levels);

		glTexStorage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, mip_levels, gl_format, width, height);
		glTexStorage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, mip_levels, gl_format, width, height);
		glTexStorage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, mip_levels, gl_format, width, height);
		glTexStorage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, mip_levels, gl_format, width, height);
		glTexStorage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, mip_levels, gl_format, width, height);
		glTexStorage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, mip_levels, gl_format, width, height);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glFinish();

	return glGetError() == GL_NO_ERROR;
}

bool TextureGL::initDepthStencil(IRenderDevice&, int width, int height, FORMAT format)
{
	assert(width > 0 && height > 0);

	_format = format;
	_mip_levels = 1;
	_width = width;
	_height = height;
	_depth = 0;

	switch (format) {
		case DEPTH_16_UNORM:
		case DEPTH_32_F:
			_type = DEPTH;
			break;

		case DEPTH_STENCIL_24_8_UNORM_UI:
		case DEPTH_STENCIL_32_8_F:
			_type = DEPTH_STENCIL;
			break;

		default:
			_type = TYPE_SIZE;
			break;
	}

	assert(_type < TYPE_SIZE);

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glTexStorage2D(GL_TEXTURE_2D, 1, _format_map[format], width, height);

	glBindTexture(GL_TEXTURE_2D, 0);
	glFinish();

	return glGetError() == GL_NO_ERROR;
}

bool TextureGL::isD3D(void) const
{
	return false;
}

unsigned int TextureGL::getTexture(void) const
{
	return _texture;
}

unsigned int TextureGL::getTexType(void) const
{
	switch (_type) {
		case ONED:
			return GL_TEXTURE_1D;

		case TWOD:
			return GL_TEXTURE_2D;

		case THREED:
			return GL_TEXTURE_3D;

		case CUBE:
			return GL_TEXTURE_CUBE_MAP;

		case DEPTH:
		case DEPTH_STENCIL:
			return GL_TEXTURE_2D;

		default:
			break;
	}

	// should never happen
	return 0;
}

unsigned int TextureGL::determineChannels(FORMAT format)
{
	GLenum channels = 0;

	switch (format) {
		case R_8_UNORM:
		case R_16_UNORM:
		case R_8_SNORM:
		case R_16_SNORM:
		case R_8_I:
		case R_16_I:
		case R_32_I:
		case R_8_UI:
		case R_16_UI:
		case R_32_UI:
		case R_16_F:
		case R_32_F:
			channels = GL_RED;
			break;

		case RG_8_UNORM:
		case RG_16_UNORM:
		case RG_8_SNORM:
		case RG_16_SNORM:
		case RG_8_I:
		case RG_16_I:
		case RG_32_I:
		case RG_8_UI:
		case RG_16_UI:
		case RG_32_UI:
		case RG_16_F:
		case RG_32_F:
			channels = GL_RG;
			break;

		//case RGB_8_UNORM:
		//case RGB_8_SNORM:
		case RGB_16_SNORM:
		//case RGB_8_I:
		//case RGB_16_I:
		case RGB_32_I:
		//case RGB_8_UI:
		//case RGB_16_UI:
		case RGB_32_UI:
		case RGB_16_F:
		case RGB_32_F:
		case RGB_11_11_10_F:
		case RGBE_9_9_9_5:
			channels = GL_RGB;
			break;

		case RGBA_8_UNORM:
		case RGBA_16_UNORM:
		case RGBA_8_SNORM:
		case RGBA_8_I:
		case RGBA_16_I:
		case RGBA_32_I:
		case RGBA_8_UI:
		case RGBA_16_UI:
		case RGBA_32_UI:
		case RGBA_16_F:
		case RGBA_32_F:
		case RGBA_10_10_10_2_UNORM:
		case RGBA_10_10_10_2_UI:
		case SRGBA_8_UNORM:
			channels = GL_RGBA;
			break;

		case DEPTH_16_UNORM:
		case DEPTH_32_F:
			channels = GL_DEPTH_COMPONENT;
			break;

		case DEPTH_STENCIL_24_8_UNORM_UI:
		case DEPTH_STENCIL_32_8_F:
			channels = GL_DEPTH_STENCIL;
			break;

		// To get GCC to stop erroring
		case FORMAT_SIZE:
			break;
	}

	return channels;
}

unsigned int TextureGL::determineType(FORMAT format)
{
	GLenum type = 0;

	switch (format) {
		case R_8_UNORM:
		case R_8_UI:
		case RG_8_UNORM:
		case RG_8_UI:
		//case RGB_8_UNORM:
		//case RGB_8_UI:
		case RGBA_8_UNORM:
		case RGBA_8_UI:
			type = GL_UNSIGNED_BYTE;
			break;

		case R_8_SNORM:
		case R_8_I:
		case RG_8_SNORM:
		case RG_8_I:
		//case RGB_8_SNORM:
		//case RGB_8_I:
		case RGBA_8_SNORM:
		case RGBA_8_I:
			type = GL_BYTE;
			break;

		case R_16_UNORM:
		case R_16_UI:
		case RG_16_UNORM:
		case RG_16_UI:
		//case RGB_16_UI:
		case RGBA_16_UNORM:
		case RGBA_16_UI:
			type = GL_UNSIGNED_SHORT;
			break;

		case R_16_SNORM:
		case R_16_I:
		case RG_16_SNORM:
		case RG_16_I:
		case RGB_16_SNORM:
		//case RGB_16_I:
		case RGBA_16_I:
			type = GL_SHORT;
			break;

		case R_32_UI:
		case RG_32_UI:
		case RGB_32_UI:
		case RGBA_32_UI:
			type = GL_UNSIGNED_INT;
			break;

		case R_32_I:
		case RG_32_I:
		case RGB_32_I:
		case RGBA_32_I:
			type = GL_INT;
			break;

		case R_16_F:
		case RG_16_F:
		case RGB_16_F:
		case RGBA_16_F:
			type = GL_HALF_FLOAT;
			break;

		case R_32_F:
		case RG_32_F:
		case RGB_32_F:
		case RGBA_32_F:
			type = GL_FLOAT;
			break;

		case RGB_11_11_10_F:
			type = GL_UNSIGNED_INT_10F_11F_11F_REV;
			break;

		case RGBE_9_9_9_5:
			type = GL_UNSIGNED_INT_5_9_9_9_REV;
			break;

		case RGBA_10_10_10_2_UNORM:
		case RGBA_10_10_10_2_UI:
			type = GL_UNSIGNED_INT_10_10_10_2;
			break;

		case SRGBA_8_UNORM:
			type = GL_UNSIGNED_BYTE;
			break;

		case DEPTH_16_UNORM:
			type = GL_HALF_FLOAT;
			break;

		case DEPTH_32_F:
			type = GL_FLOAT;
			break;

		case DEPTH_STENCIL_24_8_UNORM_UI:
			type = GL_UNSIGNED_INT_24_8;
			break;

		case DEPTH_STENCIL_32_8_F:
			type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
			break;

		// To get GCC to stop erroring
		case FORMAT_SIZE:
			break;
	}

	return type;
}

NS_END
