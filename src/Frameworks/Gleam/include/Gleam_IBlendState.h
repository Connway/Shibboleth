/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

class IBlendState
{
public:
	enum class BlendFactor
	{
		Zero = 1,
		One,
		SourceColor,
		InverseSourceColor,
		SourceAlpha,
		InverseSourceAlpha,
		DestAlpha,
		InverseDestAlpha,
		DestColor,
		InverseDestColor,
		SourceAlphaSaturate,
		// Factor,
		// InverseFactor,
		FactorSource1Color,
		FactorInverseSource1Color,
		FactorSource1Alpha,
		FactorInverseSource1Alpha
	};

	enum class BlendOp
	{
		Add = 1,
		Subtract,
		ReverseSubract,
		Min,
		Max
	};

	enum class ColorMask
	{
		Red		= 1 << 0,
		Green	= 1 << 1,
		Blue	= 1 << 2,
		Alpha	= 1 << 3,
		All 	= Red | Green| Blue | Alpha
	};

	struct Settings final
	{
		BlendFactor blend_src_color = BlendFactor::SourceColor;
		BlendFactor blend_dst_color = BlendFactor::DestColor;
		BlendOp blend_op_color = BlendOp::Add;
		BlendFactor blend_src_alpha = BlendFactor::SourceAlpha;
		BlendFactor blend_dst_alpha = BlendFactor::DestAlpha;
		BlendOp blend_op_alpha = BlendOp::Add;
		ColorMask color_write_mask = ColorMask::All;
		bool enable_alpha_blending = false;
	};

	IBlendState(void) {}
	virtual ~IBlendState(void) {}

	virtual bool init(IRenderDevice& rd, const Settings& settings) = 0;
	virtual void destroy(void) = 0;

	virtual void bind(IRenderDevice& rd) const = 0;
	virtual void unbind(IRenderDevice& rd) const = 0;

	virtual RendererType getRendererType(void) const = 0;
};

NS_END
