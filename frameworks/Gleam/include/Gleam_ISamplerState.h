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

#pragma once

#include "Gleam_Defines.h"
#include <Gaff_IRefCounted.h>

NS_GLEAM

class IRenderDevice;

class ISamplerState : public Gaff::IRefCounted
{
public:
	enum Wrap
	{
		WRAP_REPEAT = 1,
		WRAP_MIRROR,
		WRAP_CLAMP,
		WRAP_BORDER,
		/*WRAP_MIRROR_ONCE,*/
		WRAP_SIZE = WRAP_BORDER/*WRAP_MIRROR_ONCE*/
	};
	
	enum Filter
	{
		FILTER_NEAREST_NEAREST_NEAREST = 0,
		FILTER_NEAREST_NEAREST_LINEAR,
		FILTER_NEAREST_LINEAR_NEAREST,
		FILTER_NEAREST_LINEAR_LINEAR,
		FILTER_LINEAR_NEAREST_NEAREST,
		FILTER_LINEAR_NEAREST_LINEAR,
		FILTER_LINEAR_LINEAR_NEAREST,
		FILTER_LINEAR_LINEAR_LINEAR,
		FILTER_ANISOTROPIC,
		FILTER_SIZE
	};

	struct SamplerSettings
	{
		Filter filter;
		Wrap wrap_u, wrap_v, wrap_w;
		float min_lod, max_lod;
		float lod_bias;
		int32_t max_anisotropy;
		float border_r, border_g, border_b, border_a;
	};

	ISamplerState(void) {}
	virtual ~ISamplerState(void) {}

	bool init(IRenderDevice& rd, const SamplerSettings& sampler_settings)
	{
		return init(
			rd, sampler_settings.filter, sampler_settings.wrap_u, sampler_settings.wrap_v, sampler_settings.wrap_w,
			sampler_settings.min_lod, sampler_settings.max_lod, sampler_settings.lod_bias, sampler_settings.max_anisotropy,
			sampler_settings.border_r, sampler_settings.border_b, sampler_settings.border_b, sampler_settings.border_a
		);
	}

	virtual bool init(
		IRenderDevice& rd,
		Filter filter, Wrap wrap_u, Wrap wrap_v, Wrap wrap_w,
		float min_lod, float max_lod, float lod_bias,
		int32_t max_anisotropy,
		//IRenderState::COMPARISON_FUNC compare_func,
		float border_r, float border_g, float border_b, float border_a
	) = 0;

	virtual void destroy(void) = 0;

	virtual RendererType getRendererType(void) const = 0;

	GAFF_NO_COPY(ISamplerState);
};

NS_END
