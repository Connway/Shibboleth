/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
#include <Gaff_Math.h>

NS_GLEAM

static D3D11_FILTER g_filter_map[static_cast<size_t>(ISamplerState::Filter::Count)] = {
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

bool SamplerStateD3D11::init(IRenderDevice& rd, const Settings& sampler_settings)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11 && Gaff::Between(sampler_settings.max_anisotropy, 1, 16));

	D3D11_SAMPLER_DESC desc;
	desc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(sampler_settings.wrap_u);
	desc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(sampler_settings.wrap_v);
	desc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(sampler_settings.wrap_w);
	desc.BorderColor[0] = sampler_settings.border_color.r;
	desc.BorderColor[1] = sampler_settings.border_color.g;
	desc.BorderColor[2] = sampler_settings.border_color.b;
	desc.BorderColor[3] = sampler_settings.border_color.a;
	desc.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>(sampler_settings.compare_func);
	desc.Filter = g_filter_map[static_cast<size_t>(sampler_settings.filter)];
	desc.MaxAnisotropy = static_cast<UINT>(sampler_settings.max_anisotropy);
	desc.MaxLOD = sampler_settings.max_lod;
	desc.MinLOD = sampler_settings.min_lod;
	desc.MipLODBias = sampler_settings.lod_bias;

	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	const HRESULT result = rd3d.getDevice()->CreateSamplerState(&desc, &_sampler_state);
	return SUCCEEDED(result);
}

void SamplerStateD3D11::destroy(void)
{
	SAFERELEASE(_sampler_state)
}

RendererType SamplerStateD3D11::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

ID3D11SamplerState* SamplerStateD3D11::getSamplerState(void) const
{
	return _sampler_state;
}

NS_END

#endif
