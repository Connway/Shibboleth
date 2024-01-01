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

#pragma once

#include "Gleam_Defines.h"

MSVC_DISABLE_WARNING_PUSH(4309)

NS_GLEAM

class IRenderDevice;

class IDepthStencilState
{
public:
	enum class StencilOp
	{
		Keep = 1,
		Zero,
		Replace,
		IncrementClamp,
		DecrementClanp,
		Invert,
		IncrementWrap,
		DecrementWrap
	};

	struct StencilData final
	{
		StencilOp stencil_depth_fail = StencilOp::Keep;
		StencilOp stencil_pass_depth_fail = StencilOp::Keep;
		StencilOp stencil_depth_pass = StencilOp::Keep;
		ComparisonFunc comp_func = ComparisonFunc::Always;
	};

	struct Settings final
	{
		StencilData front_face;
		StencilData back_face;
		unsigned int stencil_ref = 0;
		ComparisonFunc depth_func = ComparisonFunc::Less;
		char stencil_read_mask = 0xFF;
		char stencil_write_mask = 0xFF;
		bool stencil_test = false;
		bool depth_test = true;
	};

	IDepthStencilState(void) {}
	virtual ~IDepthStencilState(void) {}

	virtual bool init(IRenderDevice& rd, const Settings& settings) = 0;
	virtual void destroy(void) = 0;

	virtual void bind(IRenderDevice& rd) const = 0;
	virtual void unbind(IRenderDevice& rd) const = 0;

	virtual RendererType getRendererType(void) const = 0;
};

NS_END

MSVC_DISABLE_WARNING_POP()
