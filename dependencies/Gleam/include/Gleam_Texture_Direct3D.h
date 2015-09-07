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

#pragma once

#include "Gleam_IncludeD3D11.h"
#include "Gleam_RefCounted.h"
#include "Gleam_ITexture.h"

NS_GLEAM

class TextureD3D : public ITexture
{
public:
	TextureD3D(void);
	~TextureD3D(void);

	void destroy(void);

	bool init3D(IRenderDevice& rd, int width, int height, int depth, FORMAT format, int mip_levels = 1, const void* buffer = nullptr);
	bool init2D(IRenderDevice& rd, int width, int height, FORMAT format, int mip_levels = 1, const void* buffer = nullptr);
	bool init1D(IRenderDevice& rd, int width, FORMAT format, int mip_levels = 1, const void* buffer = nullptr);
	bool initCubemap(IRenderDevice& rd, int width, int height, FORMAT format, int mip_levels = 1, const void* buffer = nullptr);
	bool initDepthStencil(IRenderDevice& rd, int width, int height, FORMAT format);

	INLINE bool isD3D(void) const;

	INLINE ID3D11DepthStencilView* getDepthStencilView(void) const;

	INLINE void* getTexture(void) const;
	INLINE ID3D11Texture1D* getTexture1D(void) const;
	INLINE ID3D11Texture2D* getTexture2D(void) const;
	INLINE ID3D11Texture3D* getTexture3D(void) const;

	INLINE static DXGI_FORMAT GetD3DFormat(FORMAT format);
	INLINE static DXGI_FORMAT GetTypedFormat(FORMAT format);

private:
	ID3D11DepthStencilView* _depth_stencil_view;

	union
	{
		void* _texture;
		ID3D11Texture1D* _texture_1d;
		ID3D11Texture2D* _texture_2d;
		ID3D11Texture3D* _texture_3d;
	};

	GLEAM_REF_COUNTED(TextureD3D);
};

NS_END
