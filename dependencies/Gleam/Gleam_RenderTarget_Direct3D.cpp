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

#if defined(_WIN32) || defined(_WIN64)

#include "Gleam_RenderTarget_Direct3D.h"
#include "Gleam_IRenderDevice_Direct3D.h"
#include "Gleam_Texture_Direct3D.h"
#include "Gleam_IRenderDevice.h"
#include <Gaff_IncludeAssert.h>

NS_GLEAM

RenderTargetD3D::RenderTargetD3D(void):
	_depth_stencil_view(nullptr)
{
}

RenderTargetD3D::~RenderTargetD3D(void)
{
	destroy();
}

bool RenderTargetD3D::init(void)
{
	return true;
}

void RenderTargetD3D::destroy(void)
{
	for (unsigned int i = 0; i < _render_target_views.size(); ++i) {
		_render_target_views[i]->Release();
	}

	SAFERELEASE(_depth_stencil_view)

	_render_target_views.clear();
}

bool RenderTargetD3D::addTexture(IRenderDevice& rd, const ITexture* color_texture, CUBE_FACE face)
{
	assert(color_texture && color_texture->isD3D());
	assert(rd.isD3D());

	ID3D11RenderTargetView* render_target_view = nullptr;

	D3D11_RENDER_TARGET_VIEW_DESC desc;
	desc.Format = TextureD3D::getD3DFormat(color_texture->getFormat());

	if (face == NONE) {
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

	} else {
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 1;
		desc.Texture2DArray.MipSlice = 0;
		desc.Texture2DArray.FirstArraySlice = face;
	}

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	HRESULT result = rd3d.getActiveDevice()->CreateRenderTargetView(reinterpret_cast<const TextureD3D*>(color_texture)->getTexture2D(), &desc, &render_target_view);
	RETURNIFFAILED(result)

	// This is the first one, use this texture's width/height
	if (_render_target_views.empty()) {
		_viewport.TopLeftX = 0.0f;
		_viewport.TopLeftY = 0.0f;
		_viewport.Width = static_cast<float>(color_texture->getWidth());
		_viewport.Height = static_cast<float>(color_texture->getHeight());
		_viewport.MinDepth = 0.0f;
		_viewport.MaxDepth = 1.0f;
	}

	_render_target_views.push(render_target_view);
	return true;
}

void RenderTargetD3D::popTexture(void)
{
	_render_target_views.last()->Release();
	_render_target_views.pop();
}

bool RenderTargetD3D::addDepthStencilBuffer(IRenderDevice& rd, const ITexture* depth_stencil_texture)
{
	assert(depth_stencil_texture && depth_stencil_texture->isD3D());
	assert(rd.isD3D());

	if (_depth_stencil_view) {
		_depth_stencil_view->Release();
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	desc.Flags = 0;
	desc.Format = TextureD3D::getD3DFormat(depth_stencil_texture->getFormat());
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	HRESULT result = rd3d.getActiveDevice()->CreateDepthStencilView(reinterpret_cast<const TextureD3D*>(depth_stencil_texture)->getTexture2D(), &desc, &_depth_stencil_view);
	return SUCCEEDED(result);
}

void RenderTargetD3D::bind(IRenderDevice& rd)
{
	assert(rd.isD3D());

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	rd3d.getActiveDeviceContext()->OMSetRenderTargets(static_cast<UINT>(_render_target_views.size()), _render_target_views.getArray(), _depth_stencil_view);
	rd3d.getActiveDeviceContext()->RSSetViewports(1, &_viewport);
}

void RenderTargetD3D::unbind(IRenderDevice& rd)
{
	assert(rd.isD3D());
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	rd3d.getActiveDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);
}

void RenderTargetD3D::clear(IRenderDevice& rd, unsigned int clear_flags, float clear_depth, unsigned char clear_stencil, float* clear_color)
{
	assert(rd.isD3D());
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));

	if (clear_flags | CLEAR_COLOR) {
		for (size_t i = 0; i < _render_target_views.size(); ++i) {
			rd3d.getActiveDeviceContext()->ClearRenderTargetView(_render_target_views[i], (clear_color) ? clear_color : rd3d.getClearColor());
		}
	}

	if (_depth_stencil_view) {
		rd3d.getActiveDeviceContext()->ClearDepthStencilView(_depth_stencil_view, clear_flags, clear_depth, clear_stencil);
	}
}

bool RenderTargetD3D::isComplete(void) const
{
	return _render_target_views.size() > 0;
}

bool RenderTargetD3D::isD3D(void) const
{
	return true;
}

void RenderTargetD3D::setRTV(ID3D11RenderTargetView* rt, const D3D11_VIEWPORT& viewport)
{
	assert(rt);
	_render_target_views.push(rt);
	_viewport = viewport;
	rt->AddRef();
}

NS_END

#endif
