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

#pragma once

#include "Gleam_Defines.h"
#include <Gaff_Defines.h>

NS_GLEAM

class IRenderDevice;

class ITexture
{
public:
	enum class Type
	{
		ONE_D = 0,
		TWO_D,
		THREE_D,
		CUBE,
		DEPTH,
		DEPTH_STENCIL,

		TWO_D_ARRAY,
		ONE_D_ARRAY,

		SIZE
	};

	enum class Format
	{
		R_8_UNORM = 0,
		R_16_UNORM,
		RG_8_UNORM,
		RG_16_UNORM,
		//RGB_8_UNORM,
		RGBA_8_UNORM,
		RGBA_16_UNORM,

		R_8_SNORM,
		R_16_SNORM,
		RG_8_SNORM,
		RG_16_SNORM,
		//RGB_8_SNORM,
		RGB_16_SNORM,
		RGBA_8_SNORM,

		R_8_I,
		R_16_I,
		R_32_I,
		RG_8_I,
		RG_16_I,
		RG_32_I,
		//RGB_8_I,
		//RGB_16_I,
		RGB_32_I,
		RGBA_8_I,
		RGBA_16_I,
		RGBA_32_I,

		R_8_UI,
		R_16_UI,
		R_32_UI,
		RG_8_UI,
		RG_16_UI,
		RG_32_UI,
		//RGB_8_UI,
		//RGB_16_UI,
		RGB_32_UI,
		RGBA_8_UI,
		RGBA_16_UI,
		RGBA_32_UI,

		R_16_F,
		R_32_F,
		RG_16_F,
		RG_32_F,
		RGB_16_F,
		RGB_32_F,
		RGBA_16_F,
		RGBA_32_F,

		RGB_11_11_10_F,
		RGBE_9_9_9_5,
		RGBA_10_10_10_2_UNORM,
		RGBA_10_10_10_2_UI,
		SRGBA_8_UNORM,

		DEPTH_16_UNORM,
		DEPTH_32_F,
		DEPTH_STENCIL_24_8_UNORM_UI,
		DEPTH_STENCIL_32_8_F,

		SIZE
	};

	ITexture(void) {}
	virtual ~ITexture(void) {}

	int32_t getMipLevels(void) const { return _mip_levels; }
	Format getFormat(void) const { return _format; }
	Type getType(void) const { return _type; }
	int32_t getWidth(void) const { return _width; }
	int32_t getHeight(void) const { return _height; }
	int32_t getDepth(void) const { return _depth; }
	int32_t getArraySize(void) const { return _array_size; }

	virtual void destroy(void) = 0;

	virtual bool init2DArray(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t num_elements, int32_t mip_levels = 1, const void* buffer = nullptr) = 0;
	virtual bool init1DArray(IRenderDevice& rd, int32_t width, Format format, int32_t num_elements, int32_t mip_levels = 1, const void* buffer = nullptr) = 0;
	virtual bool init3D(IRenderDevice& rd, int32_t width, int32_t height, int32_t depth, Format format, int32_t mip_levels = 1, const void* buffer = nullptr) = 0;
	virtual bool init2D(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t mip_levels = 1, const void* buffer = nullptr) = 0;
	virtual bool init1D(IRenderDevice& rd, int32_t width, Format format, int32_t mip_levels = 1, const void* buffer = nullptr) = 0;
	virtual bool initCubemap(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t mip_levels = 1, const void* buffer = nullptr) = 0;
	virtual bool initDepthStencil(IRenderDevice& rd, int32_t width, int32_t height, Format format) = 0;

	virtual RendererType getRendererType(void) const = 0;

protected:
	int32_t _array_size = 0;
	int32_t _mip_levels;
	int32_t _width;
	int32_t _height;
	int32_t _depth;
	Format _format;
	Type _type;

	GAFF_NO_COPY(ITexture);
};

NS_END
