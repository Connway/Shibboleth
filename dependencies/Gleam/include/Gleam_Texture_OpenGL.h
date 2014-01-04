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

#pragma once

#include "Gleam_ITexture.h"

NS_GLEAM

class TextureGL : public ITexture
{
public:
	TextureGL(void);
	~TextureGL(void);

	void destroy(void);

	bool init3D(const IRenderDevice&, int width, int height, int depth, FORMAT format, int mip_levels = 1, const void* buffer = NULLPTR);
	bool init2D(const IRenderDevice&, int width, int height, FORMAT format, int mip_levels = 1, const void* buffer = NULLPTR);
	bool init1D(const IRenderDevice&, int width, FORMAT format, int mip_levels = 1, const void* buffer = NULLPTR);
	bool initCubemap(const IRenderDevice&, int width, int height, FORMAT format, int mip_levels = 1, const void* buffer = NULLPTR);
					//const void* pos_x_buffer = NULLPTR, const void* neg_x_buffer = NULLPTR,
					//const void* pos_y_buffer = NULLPTR, const void* neg_y_buffer = NULLPTR,
					//const void* pos_z_buffer = NULLPTR, const void* neg_z_buffer = NULLPTR);
	bool initDepthStencil(const IRenderDevice& rd, int width, int height, FORMAT format);

	INLINE bool isD3D(void) const;

	INLINE unsigned int getTexture(void) const;
	unsigned int getTexType(void) const;

private:
	unsigned int _texture;

	unsigned int determineChannels(FORMAT format) const;
	unsigned int determineType(FORMAT format) const;
};

NS_END
