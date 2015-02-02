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

#include "Gleam_IRenderTarget.h"
#include "Gleam_IncludeD3D11.h"
#include "Gleam_RefCounted.h"
#include "Gleam_Array.h"

struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

NS_GLEAM

class RenderTargetD3D : public IRenderTarget
{
public:
	RenderTargetD3D(void);
	~RenderTargetD3D(void);

	bool init(void);
	void destroy(void);

	bool addTexture(IRenderDevice& rd, const ITexture* color_texture, CUBE_FACE face = NONE);
	INLINE void popTexture(void);

	bool addDepthStencilBuffer(IRenderDevice& rd, const ITexture* depth_stencil_texture);

	INLINE void bind(IRenderDevice& rd);
	INLINE void unbind(IRenderDevice& rd);

	bool isComplete(void) const;

	bool isD3D(void) const;

private:
	GleamArray<ID3D11RenderTargetView*> _render_target_views;
	D3D11_VIEWPORT _viewport;
	ID3D11DepthStencilView* _depth_stencil_view;

	friend class RenderDeviceD3D;
	void setRTV(ID3D11RenderTargetView* rt, const D3D11_VIEWPORT& viewport);

	GLEAM_REF_COUNTED(RenderTargetD3D);
};

NS_END
