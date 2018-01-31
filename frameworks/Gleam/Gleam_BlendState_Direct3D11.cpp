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

#include "Gleam_BlendState_Direct3D11.h"
#include "Gleam_IRenderDevice_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"
#include <Gaff_Assert.h>

NS_GLEAM

BlendStateD3D11::BlendStateD3D11(void):
	_blend_state(nullptr)
{
}

BlendStateD3D11::~BlendStateD3D11(void)
{
	destroy();
}

bool BlendStateD3D11::init(IRenderDevice& rd, const BlendStateSettings& settings)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11);

	IRenderDeviceD3D11& rd3d = reinterpret_cast<IRenderDeviceD3D11&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11Device* device = rd3d.getDevice();

	D3D11_BLEND_DESC blend_desc;
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;

	blend_desc.RenderTarget[0].BlendEnable = settings.enable_alpha_blending;
	blend_desc.RenderTarget[0].BlendOp = static_cast<D3D11_BLEND_OP>(settings.blend_op_color);
	blend_desc.RenderTarget[0].BlendOpAlpha = static_cast<D3D11_BLEND_OP>(settings.blend_op_alpha);
	blend_desc.RenderTarget[0].DestBlend = static_cast<D3D11_BLEND>(settings.blend_dst_color);
	blend_desc.RenderTarget[0].DestBlendAlpha = static_cast<D3D11_BLEND>(settings.blend_dst_alpha);
	blend_desc.RenderTarget[0].RenderTargetWriteMask = settings.color_write_mask;
	blend_desc.RenderTarget[0].SrcBlend = static_cast<D3D11_BLEND>(settings.blend_src_color);
	blend_desc.RenderTarget[0].SrcBlendAlpha = static_cast<D3D11_BLEND>(settings.blend_src_alpha);

	HRESULT result = device->CreateBlendState(&blend_desc, &_blend_state);
	return SUCCEEDED(result);
}

bool BlendStateD3D11::init(IRenderDevice& rd, const BlendStateSettings* settings)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11);

	IRenderDeviceD3D11& rd3d = reinterpret_cast<IRenderDeviceD3D11&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11Device* device = rd3d.getDevice();

	D3D11_BLEND_DESC blend_desc;
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = true;

	for (unsigned int i = 0; i < 8; ++i) {
		blend_desc.RenderTarget[i].BlendEnable = settings[i].enable_alpha_blending;
		blend_desc.RenderTarget[i].BlendOp = static_cast<D3D11_BLEND_OP>(settings[i].blend_op_color);
		blend_desc.RenderTarget[i].BlendOpAlpha = static_cast<D3D11_BLEND_OP>(settings[i].blend_op_alpha);
		blend_desc.RenderTarget[i].DestBlend = static_cast<D3D11_BLEND>(settings[i].blend_dst_color);
		blend_desc.RenderTarget[i].DestBlendAlpha = static_cast<D3D11_BLEND>(settings[i].blend_dst_alpha);
		blend_desc.RenderTarget[i].RenderTargetWriteMask = settings[i].color_write_mask;
		blend_desc.RenderTarget[i].SrcBlend = static_cast<D3D11_BLEND>(settings[i].blend_src_color);
		blend_desc.RenderTarget[i].SrcBlendAlpha = static_cast<D3D11_BLEND>(settings[i].blend_src_alpha);
	}

	HRESULT result = device->CreateBlendState(&blend_desc, &_blend_state);
	return SUCCEEDED(result);
}

void BlendStateD3D11::destroy(void)
{
	SAFERELEASE(_blend_state);
}

void BlendStateD3D11::set(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11);
	IRenderDeviceD3D11& rd3d = reinterpret_cast<IRenderDeviceD3D11&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getDeviceContext();

	context->OMSetBlendState(_blend_state, NULL, 0xFFFFFFFF);
}

void BlendStateD3D11::unset(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11);
	IRenderDeviceD3D11& rd3d = reinterpret_cast<IRenderDeviceD3D11&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getDeviceContext();

	context->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
}

RendererType BlendStateD3D11::getRendererType(void) const
{
	return RENDERER_DIRECT3D11;
}

NS_END
