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

#ifdef GLEAM_USE_D3D11

#include "Gleam_RasterState_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

RasterState::RasterState(void):
	_raster_state(nullptr)
{
}

RasterState::~RasterState(void)
{
	destroy();
}

bool RasterState::init(IRenderDevice& rd, const Settings& settings)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);

	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11Device5* const device = rd3d.getDevice();

	D3D11_RASTERIZER_DESC raster_desc;
	raster_desc.AntialiasedLineEnable = false;
	raster_desc.CullMode = (settings.two_sided) ? D3D11_CULL_NONE : D3D11_CULL_BACK;
	raster_desc.DepthBias = settings.depth_bias;
	raster_desc.DepthBiasClamp = settings.depth_bias_clamp;
	raster_desc.DepthClipEnable = settings.depth_clip_enabled;
	raster_desc.FillMode = (settings.wireframe) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	raster_desc.FrontCounterClockwise = settings.front_face_counter_clockwise;
	raster_desc.MultisampleEnable = false;
	raster_desc.ScissorEnable = settings.scissor_enabled;
	raster_desc.SlopeScaledDepthBias = settings.slope_scale_depth_bias;

	const HRESULT result = device->CreateRasterizerState(&raster_desc, &_raster_state);
	return SUCCEEDED(result);
}

void RasterState::destroy(void)
{
	GAFF_COM_SAFE_RELEASE(_raster_state);
}

void RasterState::bind(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->RSSetState(_raster_state);
}

void RasterState::unbind(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->RSSetState(NULL);
}

RendererType RasterState::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

NS_END

#endif
