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

#pragma once

#include "Gleam_Color.h"
#include <Gaff_Defines.h>

NS_GLEAM

class IRenderDevice;

class ISamplerState
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
		Color border_color = COLOR_BLACK;
		ComparisonFunc compare_func = ComparisonFunc::NEVER;
	};

	ISamplerState(void) {}
	virtual ~ISamplerState(void) {}

	virtual bool init(IRenderDevice& rd, const SamplerSettings& sampler_settings) = 0;
	virtual void destroy(void) = 0;

	virtual RendererType getRendererType(void) const = 0;

	GAFF_NO_COPY(ISamplerState);
};

NS_END
