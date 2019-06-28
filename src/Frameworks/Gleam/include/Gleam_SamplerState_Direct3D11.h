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

#include "Gleam_ISamplerState.h"

struct ID3D11SamplerState;

NS_GLEAM

class SamplerStateD3D11 : public ISamplerState
{
public:
	SamplerStateD3D11(void);
	~SamplerStateD3D11(void);

	bool init(
		IRenderDevice& rd, Filter filter,
		Wrap wrap_u, Wrap wrap_v, Wrap wrap_w,
		float min_lod, float max_lod, float lod_bias,
		int32_t max_anisotropy,
		//IRenderState::COMPARISON_FUNC compare_func,
		float border_r, float border_g, float border_b, float border_a
	);

	void destroy(void);

	RendererType getRendererType(void) const;

	ID3D11SamplerState* getSamplerState(void) const;

private:
	ID3D11SamplerState* _sampler_state;
};

NS_END
