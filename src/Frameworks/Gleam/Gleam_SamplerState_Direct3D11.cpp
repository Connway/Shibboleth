/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Gleam_SamplerState_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

static D3D11_FILTER g_filter_map[ISamplerState::FILTER_SIZE] = {
	D3D11_FILTER_MIN_MAG_MIP_POINT,
	D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
	D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
	D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,
	D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,
	D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
	D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
	D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	D3D11_FILTER_ANISOTROPIC
};

SamplerStateD3D11::SamplerStateD3D11(void):
	_sampler_state(nullptr)
{
}

SamplerStateD3D11::~SamplerStateD3D11(void)
{
	destroy();
}

bool SamplerStateD3D11::init(
	IRenderDevice& rd, Filter filter,
	Wrap wrap_u, Wrap wrap_v, Wrap wrap_w,
	float min_lod, float max_lod, float lod_bias,
	int32_t max_anisotropy,
	//IRenderState::COMPARISON_FUNC compare_func,
	float border_r, float border_g, float border_b, float border_a)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D11 && max_anisotropy <= 16);

	D3D11_SAMPLER_DESC desc;
	desc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(wrap_u);
	desc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(wrap_v);
	desc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(wrap_w);
	desc.BorderColor[0] = border_r;
	desc.BorderColor[1] = border_g;
	desc.BorderColor[2] = border_b;
	desc.BorderColor[3] = border_a;
	//desc.ComparisonFunc = (D3D11_COMPARISON_FUNC)compare_func;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.Filter = g_filter_map[filter];
	desc.MaxAnisotropy = static_cast<UINT>(max_anisotropy);
	desc.MaxLOD = max_lod;
	desc.MinLOD = min_lod;
	desc.MipLODBias = lod_bias;

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	HRESULT result = rd3d.getDevice()->CreateSamplerState(&desc, &_sampler_state);
	return SUCCEEDED(result);
}

void SamplerStateD3D11::destroy(void)
{
	SAFERELEASE(_sampler_state)
}

RendererType SamplerStateD3D11::getRendererType(void) const
{
	return RENDERER_DIRECT3D11;
}

ID3D11SamplerState* SamplerStateD3D11::getSamplerState(void) const
{
	return _sampler_state;
}

NS_END

#endif
