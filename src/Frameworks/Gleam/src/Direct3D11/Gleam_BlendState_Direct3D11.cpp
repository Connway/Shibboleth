/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#ifdef GLEAM_USE_D3D11

#include "Direct3D11/Gleam_BlendState_Direct3D11.h"
#include "Direct3D11/Gleam_RenderDevice_Direct3D11.h"
#include "Direct3D11/Gleam_IncludeD3D11.h"
#include "Gleam_IRenderDevice.h"
#include <Gaff_Assert.h>

NS_GLEAM

BlendState::BlendState(void):
	_blend_state(nullptr)
{
}

BlendState::~BlendState(void)
{
	destroy();
}

bool BlendState::init(IRenderDevice& rd, const Settings& settings)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	D3D11_BLEND_DESC blend_desc;
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;

	blend_desc.RenderTarget[0].BlendEnable = settings.enable_alpha_blending;
	blend_desc.RenderTarget[0].BlendOp = static_cast<D3D11_BLEND_OP>(settings.blend_op_color);
	blend_desc.RenderTarget[0].BlendOpAlpha = static_cast<D3D11_BLEND_OP>(settings.blend_op_alpha);
	blend_desc.RenderTarget[0].DestBlend = static_cast<D3D11_BLEND>(settings.blend_dst_color);
	blend_desc.RenderTarget[0].DestBlendAlpha = static_cast<D3D11_BLEND>(settings.blend_dst_alpha);
	blend_desc.RenderTarget[0].SrcBlend = static_cast<D3D11_BLEND>(settings.blend_src_color);
	blend_desc.RenderTarget[0].SrcBlendAlpha = static_cast<D3D11_BLEND>(settings.blend_src_alpha);
	blend_desc.RenderTarget[0].RenderTargetWriteMask = static_cast<UINT8>(settings.color_write_mask);

	const HRESULT result = device->CreateBlendState(&blend_desc, &_blend_state);
	return SUCCEEDED(result);
}

void BlendState::destroy(void)
{
	GAFF_COM_SAFE_RELEASE(_blend_state);
}

void BlendState::bind(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->OMSetBlendState(_blend_state, NULL, 0xFFFFFFFF);
}

void BlendState::unbind(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
}

RendererType BlendState::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

NS_END

#endif
