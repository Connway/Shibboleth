/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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
#include <Gaff_RefCounted.h>

NS_GLEAM

class IRenderDevice;

class ITexture : public Gaff::IRefCounted
{
public:
	enum TYPE { ONED = 0, TWOD, THREED, CUBE, DEPTH, DEPTH_STENCIL, TYPE_SIZE };
	enum FORMAT
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

		FORMAT_SIZE
	};

	ITexture(void) {}
	virtual ~ITexture(void) {}

	INLINE unsigned int getMipLevels(void) const { return _mip_levels; }
	INLINE FORMAT getFormat(void) const { return _format; }
	INLINE TYPE getType(void) const { return _type; }
	INLINE int getWidth(void) const { return _width; }
	INLINE int getHeight(void) const { return _height; }
	INLINE int getDepth(void) const { return _depth; }

	virtual void destroy(void) = 0;

	virtual bool init3D(IRenderDevice& rd, int width, int height, int depth, FORMAT format,
						int mip_levels = 1, const void* buffer = nullptr) = 0;
	virtual bool init2D(IRenderDevice& rd, int width, int height, FORMAT format,
						int mip_levels = 1, const void* buffer = nullptr) = 0;
	virtual bool init1D(IRenderDevice& rd, int width, FORMAT format,
						int mip_levels = 1, const void* buffer = nullptr) = 0;
	virtual bool initCubemap(IRenderDevice& rd, int width, int height, FORMAT format, int mip_levels = 1, const void* buffer = nullptr) = 0;
	virtual bool initDepthStencil(IRenderDevice& rd, int width, int height, FORMAT format) = 0;

	virtual RendererType getRendererType(void) const = 0;

protected:
	unsigned int _mip_levels;
	int _width;
	int _height;
	int _depth;
	FORMAT _format;
	TYPE _type;

	GAFF_NO_COPY(ITexture);
};

NS_END
