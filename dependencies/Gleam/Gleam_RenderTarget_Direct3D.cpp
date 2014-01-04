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

#include "Gleam_RenderTarget_Direct3D.h"
#include "Gleam_RenderDevice_Direct3D.h"
#include "Gleam_Texture_Direct3D.h"
#include "Gaff_IncludeAssert.h"
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

RenderTargetD3D::RenderTargetD3D(void):
	_depth_stencil_view(NULLPTR)
{
}

RenderTargetD3D::~RenderTargetD3D(void)
{
	destroy();
}

void RenderTargetD3D::destroy(void)
{
	for (unsigned int i = 0; i < _render_target_views.size(); ++i) {
		_render_target_views[i]->Release();
	}

	SAFERELEASE(_depth_stencil_view)

	_render_target_views.clear();
}

bool RenderTargetD3D::addTexture(const IRenderDevice& rd, const ITexture* color_texture, CUBE_FACE face)
{
	assert(color_texture && color_texture->isD3D());
	assert(rd.isD3D());

	ID3D11RenderTargetView* render_target_view = NULLPTR;

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

	HRESULT result = ((const RenderDeviceD3D&)rd).getDevice()->CreateRenderTargetView(((const TextureD3D*)color_texture)->getTexture2D(), &desc, &render_target_view);
	RETURNIFFAILED(result)

	_render_target_views.push(render_target_view);
	return true;
}

void RenderTargetD3D::popTexture(void)
{
	_render_target_views.last()->Release();
	_render_target_views.pop();
}

bool RenderTargetD3D::addDepthStencilBuffer(const IRenderDevice& rd, const ITexture* depth_stencil_texture)
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

	HRESULT result = ((const RenderDeviceD3D&)rd).getDevice()->CreateDepthStencilView(((const TextureD3D*)depth_stencil_texture)->getTexture2D(), &desc, &_depth_stencil_view);
	return SUCCEEDED(result);
}

void RenderTargetD3D::bind(IRenderDevice& rd)
{
	assert(rd.isD3D());

	float clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	for (unsigned int i = 0; i < _render_target_views.size(); ++i) {
		((RenderDeviceD3D&)rd).getDeviceContext()->ClearRenderTargetView(_render_target_views[i], clear_color);
	}

	if (_depth_stencil_view) {
		((RenderDeviceD3D&)rd).getDeviceContext()->ClearDepthStencilView(_depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	((RenderDeviceD3D&)rd).getDeviceContext()->OMSetRenderTargets(_render_target_views.size(), _render_target_views.getArray(), _depth_stencil_view);
	setPrevRenderTarget(rd);
}

void RenderTargetD3D::unbind(const IRenderDevice& rd)
{
	assert(rd.isD3D());
	((RenderDeviceD3D&)rd).unbindRenderTarget();
}

bool RenderTargetD3D::isComplete(void) const
{
	return _render_target_views.size() > 0;
}

bool RenderTargetD3D::isD3D(void) const
{
	return true;
}

NS_END
