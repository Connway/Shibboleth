/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Gleam_IRenderTarget.h"
#include "Gleam_IncludeD3D11.h"
#include "Gleam_Vector.h"

struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

NS_GLEAM

class RenderTarget final : public IRenderTarget
{
public:
	RenderTarget(void);
	~RenderTarget(void);

	bool init(void) override;
	void destroy(void) override;

	IVec2 getSize(void) const override;

	bool addTexture(IRenderDevice& rd, ITexture& color_texture, CubeFace face = CubeFace::None) override;
	void popTexture(void) override;

	bool addDepthStencilBuffer(IRenderDevice& rd, ITexture& depth_stencil_texture) override;

	void bind(IRenderDevice& rd) override;
	void unbind(IRenderDevice& rd) override;

	void clear(IRenderDevice& rd, const IRenderTarget::ClearSettings& settings = ClearSettings{}) override;

	bool isComplete(void) const override;

	RendererType getRendererType(void) const override;

	D3D11_VIEWPORT getViewport(void) const;

private:
	Vector<ID3D11RenderTargetView1*> _render_target_views;
	D3D11_VIEWPORT _viewport;
	ID3D11DepthStencilView* _depth_stencil_view = nullptr;

	friend class RenderOutput;
	void setRTV(ID3D11RenderTargetView1* rt, const D3D11_VIEWPORT& viewport);
};

NS_END