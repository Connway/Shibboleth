/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Gleam_RasterState_Direct3D.h"
#include "Gleam_IRenderDevice_Direct3D.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

RasterStateD3D::RasterStateD3D(void):
	_raster_state(nullptr)
{
}

RasterStateD3D::~RasterStateD3D(void)
{
	destroy();
}

bool RasterStateD3D::init(IRenderDevice& rd, const RasterStateSettings& settings)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D);

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11Device* device = rd3d.getActiveDevice();

	D3D11_RASTERIZER_DESC raster_desc;
	raster_desc.AntialiasedLineEnable = false;
	raster_desc.CullMode = (settings.two_sided) ? D3D11_CULL_NONE : D3D11_CULL_BACK;
	raster_desc.DepthBias = settings.depth_bias;
	raster_desc.DepthBiasClamp = settings.depth_bias_clamp;
	raster_desc.DepthClipEnable = true;
	raster_desc.FillMode = (settings.wireframe) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	raster_desc.FrontCounterClockwise = settings.front_face_counter_clockwise;
	raster_desc.MultisampleEnable = false;
	raster_desc.ScissorEnable = settings.scissor_enabled;
	raster_desc.SlopeScaledDepthBias = settings.slope_scale_depth_bias;

	HRESULT result = device->CreateRasterizerState(&raster_desc, &_raster_state);
	return SUCCEEDED(result);
}

void RasterStateD3D::destroy(void)
{
	SAFERELEASE(_raster_state);
}

void RasterStateD3D::set(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D);
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();

	context->RSSetState(_raster_state);
}

void RasterStateD3D::unset(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D);
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();

	context->RSSetState(NULL);
}

RendererType RasterStateD3D::getRendererType(void) const
{
	return RENDERER_DIRECT3D;
}

NS_END
