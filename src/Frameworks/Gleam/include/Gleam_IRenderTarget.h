/************************************************************************************
Copyright (C) by Nicholas LaCroix

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
#include "Gleam_Vec2.h"
#include <Gaff_Flags.h>

NS_GLEAM

class IRenderDevice;
class ITexture;

class IRenderTarget
{
public:
	enum class CubeFace
	{
		PosX = 0,
		NegX,
		PosY,
		NegY,
		PosZ,
		NegZ,

		None
	};

	struct ClearSettings final
	{
		enum class Flags
		{
			Depth = 0,
			Stencil,
			Color,

			Count,

			All = GAFF_FLAG_BIT(Depth) | GAFF_FLAG_BIT(Stencil) | GAFF_FLAG_BIT(Color)
		};

		Gleam::Color::RGBA color = Gleam::Color::Black;
		Gaff::Flags<Flags> flags = Flags::All;
		float depth = 1.0f;
		uint8_t stencil = 0;
	};

	IRenderTarget(void) {}
	virtual ~IRenderTarget(void) {}

	virtual bool init(void) = 0;
	virtual void destroy(void) = 0;

	virtual IVec2 getSize(void) const = 0;

	virtual bool addTexture(IRenderDevice& rd, ITexture& texture, CubeFace face = CubeFace::None) = 0;
	virtual void popTexture(void) = 0;

	virtual bool addDepthStencilBuffer(IRenderDevice& rd, ITexture& depth_stencil_texture) = 0;

	virtual void bind(IRenderDevice& rd) = 0;
	virtual void unbind(IRenderDevice& rd) = 0;

	virtual void clear(IRenderDevice& rd, const ClearSettings& settings = ClearSettings{}) = 0;

	virtual bool isComplete(void) const = 0;

	virtual RendererType getRendererType(void) const = 0;

	GAFF_NO_COPY(IRenderTarget);
};

NS_END
