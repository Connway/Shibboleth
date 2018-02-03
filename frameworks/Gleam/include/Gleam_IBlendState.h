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

class IBlendState : public Gaff::IRefCounted
{
public:
	enum BlendFactor
	{
		BLEND_FACTOR_ZERO = 1,
		BLEND_FACTOR_ONE,
		BLEND_FACTOR_SRC_COLOR,
		BLEND_FACTOR_INV_SRC_COLOR,
		BLEND_FACTOR_SRC_ALPHA,
		BLEND_FACTOR_INV_SRC_ALPHA,
		BLEND_FACTOR_DEST_ALPHA,
		BLEND_FACTOR_INV_DEST_ALPHA,
		BLEND_FACTOR_DEST_COLOR,
		BLEND_FACTOR_INV_DEST_COLOR,
		BLEND_FACTOR_SRC_ALPHA_SATURATE,
		//FACTOR,
		//INV_BLEND_FACTOR,
		BLEND_FACTOR_SRC1_COLOR = 14,
		BLEND_FACTOR_INV_SRC1_COLOR,
		BLEND_FACTOR_SRC1_ALPHA,
		BLEND_FACTOR_INV_SRC1_ALPHA,
		BLEND_FACTOR_SIZE = BLEND_FACTOR_INV_SRC1_ALPHA
	};

	enum BlendOp
	{
		BLEND_OP_ADD = 1,
		BLEND_OP_SUBTRACT,
		BLEND_OP_REVERSE_SUBTRACT,
		BLEND_OP_MIN,
		BLEND_OP_MAX,
		BLEND_OP_SIZE = BLEND_OP_MAX
	};

	enum ColorMask
	{
		COLOR_RED = 1,
		COLOR_GREEN = 2,
		COLOR_BLUE = 4,
		COLOR_ALPHA = 8,
		COLOR_ALL = COLOR_RED | COLOR_GREEN | COLOR_BLUE | COLOR_ALPHA
	};

	struct BlendStateSettings
	{
		BlendFactor blend_src_color = BLEND_FACTOR_SRC_COLOR;
		BlendFactor blend_dst_color = BLEND_FACTOR_DEST_COLOR;
		BlendOp blend_op_color = BLEND_OP_ADD;
		BlendFactor blend_src_alpha = BLEND_FACTOR_SRC_ALPHA;
		BlendFactor blend_dst_alpha = BLEND_FACTOR_DEST_ALPHA;
		BlendOp blend_op_alpha = BLEND_OP_ADD;
		char color_write_mask = COLOR_ALL;
		bool enable_alpha_blending = false;
	};

	IBlendState(void) {}
	virtual ~IBlendState(void) {}

	virtual bool init(IRenderDevice& rd, const BlendStateSettings& settings) = 0;
	virtual bool init(IRenderDevice& rd, const BlendStateSettings* settings) = 0;
	virtual void destroy(void) = 0;

	virtual void set(IRenderDevice& rd) const = 0;
	virtual void unset(IRenderDevice& rd) const = 0;

	virtual RendererType getRendererType(void) const = 0;
};

NS_END
