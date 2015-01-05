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

#include "Gleam_RenderState_Direct3D.h"
#include "Gleam_RenderDevice_Direct3D.h"
#include "Gleam_IncludeD3D11.h"
#include <Gaff_IncludeAssert.h>
#include <dxgi.h>

NS_GLEAM

RenderStateD3D::RenderStateD3D(void):
	_depth_stencil_state(nullptr), _raster_state(nullptr),
	_blend_state(nullptr), _stencil_ref(0)
{
}

RenderStateD3D::~RenderStateD3D(void)
{
	destroy();
}

bool RenderStateD3D::init(IRenderDevice& rd, bool wireframe, bool depth_test, bool stencil_test,
							COMPARISON_FUNC depth_func, StencilData front_face,
							StencilData back_face, unsigned int stencil_ref,
							char stencil_read_mask, char stencil_write_mask,
							CULL_MODE cull_face_mode, bool front_face_counter_clockwise,
							const BlendData* blend_data)
{
	assert(rd.isD3D());

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = depth_test;
	depth_stencil_desc.StencilEnable = stencil_test;
	depth_stencil_desc.DepthFunc = (D3D11_COMPARISON_FUNC)depth_func;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	depth_stencil_desc.StencilReadMask = stencil_read_mask;
	depth_stencil_desc.StencilWriteMask = stencil_write_mask;

	depth_stencil_desc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)front_face.comp_func;
	depth_stencil_desc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)front_face.stencil_depth_pass;
	depth_stencil_desc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)front_face.stencil_depth_fail;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)front_face.stencil_pass_depth_fail;

	depth_stencil_desc.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)back_face.comp_func;
	depth_stencil_desc.BackFace.StencilPassOp = (D3D11_STENCIL_OP)back_face.stencil_depth_pass;
	depth_stencil_desc.BackFace.StencilFailOp = (D3D11_STENCIL_OP)back_face.stencil_depth_fail;
	depth_stencil_desc.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)back_face.stencil_pass_depth_fail;

	_stencil_ref = stencil_ref;

	HRESULT result = device->CreateDepthStencilState(&depth_stencil_desc, &_depth_stencil_state);
	RETURNIFFAILED(result)

	D3D11_RASTERIZER_DESC raster_desc;
	raster_desc.AntialiasedLineEnable = false;
	raster_desc.CullMode = (D3D11_CULL_MODE)cull_face_mode;
	raster_desc.DepthBias = 0;
	raster_desc.DepthBiasClamp = 0.0f;
	raster_desc.DepthClipEnable = true;
	raster_desc.FillMode = (wireframe) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	raster_desc.FrontCounterClockwise = front_face_counter_clockwise;
	raster_desc.MultisampleEnable = false;
	raster_desc.ScissorEnable = false;
	raster_desc.SlopeScaledDepthBias = 0.0f;

	result = device->CreateRasterizerState(&raster_desc, &_raster_state);
	if (FAILED(result)) {
		destroy();
		return false;
	}

	D3D11_BLEND_DESC blend_desc;
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = true;

	for (unsigned int i = 0; i < 8; ++i) {
		blend_desc.RenderTarget[i].BlendEnable = blend_data[i].enable_alpha_blending;
		blend_desc.RenderTarget[i].BlendOp = (D3D11_BLEND_OP)blend_data[i].blend_op_color;
		blend_desc.RenderTarget[i].BlendOpAlpha = (D3D11_BLEND_OP)blend_data[i].blend_op_alpha;
		blend_desc.RenderTarget[i].DestBlend = (D3D11_BLEND)blend_data[i].blend_dst_color;
		blend_desc.RenderTarget[i].DestBlendAlpha = (D3D11_BLEND)blend_data[i].blend_dst_alpha;
		blend_desc.RenderTarget[i].RenderTargetWriteMask = blend_data[i].color_write_mask;
		blend_desc.RenderTarget[i].SrcBlend = (D3D11_BLEND)blend_data[i].blend_src_color;
		blend_desc.RenderTarget[i].SrcBlendAlpha = (D3D11_BLEND)blend_data[i].blend_src_alpha;
	}

	result = device->CreateBlendState(&blend_desc, &_blend_state);
	if (FAILED(result)) {
		destroy();
		return false;
	}

	return true;
}

bool RenderStateD3D::init(IRenderDevice& rd, bool wireframe, bool depth_test, bool stencil_test,
							COMPARISON_FUNC depth_func, StencilData front_face,
							StencilData back_face, unsigned int stencil_ref,
							char stencil_read_mask, char stencil_write_mask,
							CULL_MODE cull_face_mode, bool front_face_counter_clockwise,
							BlendData blend_data)
{
	assert(rd.isD3D());

	ID3D11Device* device = ((RenderDeviceD3D&)rd).getActiveDevice();

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = depth_test;
	depth_stencil_desc.StencilEnable = stencil_test;
	depth_stencil_desc.DepthFunc = (D3D11_COMPARISON_FUNC)depth_func;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	depth_stencil_desc.StencilReadMask = stencil_read_mask;
	depth_stencil_desc.StencilWriteMask = stencil_write_mask;

	depth_stencil_desc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)front_face.comp_func;
	depth_stencil_desc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)front_face.stencil_depth_pass;
	depth_stencil_desc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)front_face.stencil_depth_fail;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)front_face.stencil_pass_depth_fail;

	depth_stencil_desc.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)back_face.comp_func;
	depth_stencil_desc.BackFace.StencilPassOp = (D3D11_STENCIL_OP)back_face.stencil_depth_pass;
	depth_stencil_desc.BackFace.StencilFailOp = (D3D11_STENCIL_OP)back_face.stencil_depth_fail;
	depth_stencil_desc.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)back_face.stencil_pass_depth_fail;

	_stencil_ref = stencil_ref;

	HRESULT result = device->CreateDepthStencilState(&depth_stencil_desc, &_depth_stencil_state);
	RETURNIFFAILED(result)

		D3D11_RASTERIZER_DESC raster_desc;
	raster_desc.AntialiasedLineEnable = false;
	raster_desc.CullMode = (D3D11_CULL_MODE)cull_face_mode;
	raster_desc.DepthBias = 0;
	raster_desc.DepthBiasClamp = 0.0f;
	raster_desc.DepthClipEnable = true;
	raster_desc.FillMode = (wireframe) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	raster_desc.FrontCounterClockwise = front_face_counter_clockwise;
	raster_desc.MultisampleEnable = false;
	raster_desc.ScissorEnable = false;
	raster_desc.SlopeScaledDepthBias = 0.0f;

	result = device->CreateRasterizerState(&raster_desc, &_raster_state);
	if (FAILED(result)) {
		destroy();
		return false;
	}

	D3D11_BLEND_DESC blend_desc;
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;

	blend_desc.RenderTarget[0].BlendEnable = blend_data.enable_alpha_blending;
	blend_desc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)blend_data.blend_op_color;
	blend_desc.RenderTarget[0].BlendOpAlpha = (D3D11_BLEND_OP)blend_data.blend_op_alpha;
	blend_desc.RenderTarget[0].DestBlend = (D3D11_BLEND)blend_data.blend_dst_color;
	blend_desc.RenderTarget[0].DestBlendAlpha = (D3D11_BLEND)blend_data.blend_dst_alpha;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = blend_data.color_write_mask;
	blend_desc.RenderTarget[0].SrcBlend = (D3D11_BLEND)blend_data.blend_src_color;
	blend_desc.RenderTarget[0].SrcBlendAlpha = (D3D11_BLEND)blend_data.blend_src_alpha;

	result = device->CreateBlendState(&blend_desc, &_blend_state);
	if (FAILED(result)) {
		destroy();
		return false;
	}

	return true;
}

void RenderStateD3D::destroy(void)
{
	SAFERELEASE(_depth_stencil_state)
	SAFERELEASE(_raster_state)
	SAFERELEASE(_blend_state)
}

void RenderStateD3D::set(IRenderDevice& rd) const
{
	assert(rd.isD3D());
	ID3D11DeviceContext* context = ((RenderDeviceD3D&)rd).getActiveDeviceContext();

	context->OMSetDepthStencilState(_depth_stencil_state, _stencil_ref);
	context->OMSetBlendState(_blend_state, NULL, 0xFFFFFFFF);
	context->RSSetState(_raster_state);
}

void RenderStateD3D::unset(IRenderDevice& rd) const
{
	assert(rd.isD3D());
	rd.resetRenderState();
}

bool RenderStateD3D::isD3D(void) const
{
	return true;
}

NS_END

#endif
