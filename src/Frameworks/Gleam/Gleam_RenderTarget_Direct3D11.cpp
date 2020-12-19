/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Gleam_RenderTarget_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_Texture_Direct3D11.h"
#include "Gleam_IRenderDevice.h"

NS_GLEAM

RenderTargetD3D11::RenderTargetD3D11(void)
{
}

RenderTargetD3D11::~RenderTargetD3D11(void)
{
	destroy();
}

bool RenderTargetD3D11::init(void)
{
	return true;
}

void RenderTargetD3D11::destroy(void)
{
	for (int32_t i = 0; i < static_cast<int32_t>(_render_target_views.size()); ++i) {
		_render_target_views[i]->Release();
	}

	SAFERELEASE(_depth_stencil_view)

	_render_target_views.clear();
}

glm::ivec2 RenderTargetD3D11::getSize(void) const
{
	return glm::ivec2{
		static_cast<int32_t>(_viewport.Width),
		static_cast<int32_t>(_viewport.Height)
	};
}

bool RenderTargetD3D11::addTexture(IRenderDevice& rd, const ITexture* color_texture, CubeFace face)
{
	GAFF_ASSERT(color_texture && color_texture->getRendererType() == RendererType::DIRECT3D11);
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11);

	ID3D11RenderTargetView1* render_target_view = nullptr;

	D3D11_RENDER_TARGET_VIEW_DESC1 desc;
	desc.Format = TextureD3D11::GetD3DFormat(color_texture->getFormat());

	if (face == CubeFace::None) {
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;
		desc.Texture2D.PlaneSlice = 0;

	} else {
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 1;
		desc.Texture2DArray.MipSlice = 0;
		desc.Texture2DArray.FirstArraySlice = static_cast<UINT>(face);
	}

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);

	const HRESULT result = rd3d.getDevice()->CreateRenderTargetView1(static_cast<const TextureD3D11*>(color_texture)->getTexture2D(), &desc, &render_target_view);
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

	_render_target_views.emplace_back(render_target_view);
	return true;
}

void RenderTargetD3D11::popTexture(void)
{
	_render_target_views.back()->Release();
	_render_target_views.pop_back();
}

bool RenderTargetD3D11::addDepthStencilBuffer(IRenderDevice& rd, const ITexture* depth_stencil_texture)
{
	GAFF_ASSERT(depth_stencil_texture && depth_stencil_texture->getRendererType() == RendererType::DIRECT3D11);
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11);

	SAFERELEASE(_depth_stencil_view)

	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	desc.Flags = 0;
	desc.Format = TextureD3D11::GetD3DFormat(depth_stencil_texture->getFormat());
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	const HRESULT result = rd3d.getDevice()->CreateDepthStencilView(static_cast<const TextureD3D11*>(depth_stencil_texture)->getTexture2D(), &desc, &_depth_stencil_view);

	return SUCCEEDED(result);
}

void RenderTargetD3D11::bind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11);

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->OMSetRenderTargets(static_cast<UINT>(_render_target_views.size()), reinterpret_cast<ID3D11RenderTargetView**>(_render_target_views.data()), _depth_stencil_view);
	context->RSSetViewports(1, &_viewport);
}

void RenderTargetD3D11::unbind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11);
	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	rd3d.getDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);
}

void RenderTargetD3D11::clear(IRenderDevice& rd, uint8_t clear_flags, float clear_depth, uint8_t clear_stencil, const Color::RGBA& clear_color)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11);
	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	if (clear_flags & ClearFlags::Color) {
		for (int32_t i = 0; i < static_cast<int32_t>(_render_target_views.size()); ++i) {
			context->ClearRenderTargetView(_render_target_views[i], clear_color.data.data);
		}
	}

	if (_depth_stencil_view) {
		context->ClearDepthStencilView(_depth_stencil_view, clear_flags, clear_depth, clear_stencil);
	}
}

bool RenderTargetD3D11::isComplete(void) const
{
	return _render_target_views.size() > 0;
}

RendererType RenderTargetD3D11::getRendererType(void) const
{
	return RendererType::DIRECT3D11;
}

D3D11_VIEWPORT RenderTargetD3D11::getViewport(void) const
{
	return _viewport;
}

void RenderTargetD3D11::setRTV(ID3D11RenderTargetView1* rt, const D3D11_VIEWPORT& viewport)
{
	GAFF_ASSERT(rt);
	_render_target_views.emplace_back(rt);
	_viewport = viewport;
	rt->AddRef();
}

NS_END

#endif
