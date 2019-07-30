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

#include "Gleam_Defines.h"

NS_GLEAM

class IRenderDevice;

class IRasterState
{
public:
	struct RasterSettings final
	{
		float slope_scale_depth_bias = 0.0f;
		float depth_bias_clamp = 0.0f;
		int depth_bias = 0;
		bool depth_clip_enabled = true;
		bool front_face_counter_clockwise = false;
		bool scissor_enabled = false;
		bool two_sided = false;
		bool wireframe = false;
	};

	IRasterState(void) {}
	virtual ~IRasterState(void) {}

	virtual bool init(IRenderDevice& rd, const RasterSettings& settings) = 0;
	virtual void destroy(void) = 0;

	virtual void set(IRenderDevice& rd) const = 0;
	virtual void unset(IRenderDevice& rd) const = 0;

	virtual RendererType getRendererType(void) const = 0;
};

NS_END
