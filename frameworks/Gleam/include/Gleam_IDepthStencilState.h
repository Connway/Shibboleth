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

#if defined(_WIN32) || defined(_WIN64)
	#pragma warning(push)
	#pragma warning(disable: 4309)
#endif

NS_GLEAM

class IRenderDevice;

class IDepthStencilState : public Gaff::IRefCounted
{
public:
	enum ComparisonFunc
	{
		COMPARE_NEVER = 1,
		COMPARE_LESS,
		COMPARE_EQUAL,
		COMPARE_LESS_EQUAL,
		COMPARE_GREATER,
		COMPARE_NOT_EQUAL,
		COMPARE_GREATER_EQUAL,
		COMPARE_ALWAYS,
		COMPARE_SIZE
	};

	enum StencilOp
	{
		STENCIL_KEEP = 1,
		STENCIL_ZERO,
		STENCIL_REPLACE,
		STENCIL_INCREMENT_CLAMP,
		STENCIL_DECREMENT_CLAMP,
		STENCIL_INVERT,
		STENCIL_INCREMENT_WRAP,
		STENCIL_DECREMENT_WRAP,
		STENCIL_OP_SIZE = STENCIL_DECREMENT_WRAP
	};

	struct StencilData
	{
		StencilOp stencil_depth_fail = STENCIL_KEEP;
		StencilOp stencil_pass_depth_fail = STENCIL_KEEP;
		StencilOp stencil_depth_pass = STENCIL_KEEP;
		ComparisonFunc comp_func = COMPARE_ALWAYS;
	};

	struct DepthStencilStateSettings
	{
		StencilData front_face, back_face;
		unsigned int stencil_ref = 0;
		ComparisonFunc depth_func = COMPARE_LESS;
		char stencil_read_mask = 0xFF;
		char stencil_write_mask = 0xFF;
		bool stencil_test = false;
		bool depth_test = true;
	};

	IDepthStencilState(void) {}
	virtual ~IDepthStencilState(void) {}

	virtual bool init(IRenderDevice& rd, const DepthStencilStateSettings& settings) = 0;
	virtual void destroy(void) = 0;

	virtual void set(IRenderDevice& rd) const = 0;
	virtual void unset(IRenderDevice& rd) const = 0;

	virtual RendererType getRendererType(void) const = 0;
};

NS_END

#if defined(_WIN32) || defined(_WIN64)
	#pragma warning(pop)
#endif
