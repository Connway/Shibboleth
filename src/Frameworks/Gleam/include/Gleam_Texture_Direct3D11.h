/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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
#include "Gleam_ITexture.h"

NS_GLEAM

class Texture final : public ITexture
{
public:
	~Texture(void);

	void destroy(void) override;

	bool init2DArray(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t num_elements, int32_t mip_levels = 1, const void* buffer = nullptr) override;
	bool init1DArray(IRenderDevice& rd, int32_t width, Format format, int32_t num_elements, int32_t mip_levels = 1, const void* buffer = nullptr) override;
	bool init3D(IRenderDevice& rd, int32_t width, int32_t height, int32_t depth, Format format, int32_t mip_levels = 1, const void* buffer = nullptr) override;
	bool init2D(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t mip_levels = 1, const void* buffer = nullptr) override;
	bool init1D(IRenderDevice& rd, int32_t width, Format format, int32_t mip_levels = 1, const void* buffer = nullptr) override;
	bool initCubemap(IRenderDevice& rd, int32_t width, int32_t height, Format format, int32_t mip_levels = 1, const void* buffer = nullptr) override;
	bool initDepthStencil(IRenderDevice& rd, int32_t width, int32_t height, Format format) override;

	RendererType getRendererType(void) const override;

	const void* getTexture(void) const;
	void* getTexture(void);
	const ID3D11Texture1D* getTexture1D(void) const;
	ID3D11Texture1D* getTexture1D(void);
	const ID3D11Texture2D* getTexture2D(void) const;
	ID3D11Texture2D* getTexture2D(void);
	const ID3D11Texture3D* getTexture3D(void) const;
	ID3D11Texture3D* getTexture3D(void);

	static DXGI_FORMAT GetTypelessD3DFormat(Format format);
	static DXGI_FORMAT GetTypedD3DFormat(Format format);
	static DXGI_FORMAT GetD3DFormat(Format format);

private:
	union
	{
		void* _texture = nullptr;
		ID3D11Texture1D* _texture_1d;
		ID3D11Texture2D* _texture_2d;
		ID3D11Texture3D* _texture_3d;
	};
};

NS_END
