/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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
#include <Gaff_RefPtr.h>

NS_GLEAM

class IRenderDevice;
class ITexture;

class IRenderTarget : public Gaff::IRefCounted
{
public:
	enum CLEAR_FLAGS
	{
		CLEAR_DEPTH = 1,
		CLEAR_STENCIL,
		CLEAR_COLOR,
		CLEAR_ALL = CLEAR_DEPTH | CLEAR_STENCIL | CLEAR_COLOR
	};

	enum CUBE_FACE { POS_X = 0, NEG_X, POS_Y, NEG_Y, POS_Z, NEG_Z, NONE };

	IRenderTarget(void) {}
	virtual ~IRenderTarget(void) {}

	virtual bool init(void) = 0;
	virtual void destroy(void) = 0;

	virtual bool addTexture(IRenderDevice& rd, const ITexture* texture, CUBE_FACE face = NONE) = 0;
	virtual void popTexture(void) = 0;

	virtual bool addDepthStencilBuffer(IRenderDevice& rd, const ITexture* depth_stencil_texture) = 0;

	virtual void bind(IRenderDevice& rd) = 0;
	virtual void unbind(IRenderDevice& rd) = 0;

	virtual void clear(IRenderDevice& rd, uint32_t clear_flags = CLEAR_DEPTH | CLEAR_STENCIL, float clear_depth = 1.0f, uint8_t clear_stencil = 0, float* clear_color = nullptr) = 0;

	virtual bool isComplete(void) const = 0;

	virtual RendererType getRendererType(void) const = 0;

	GAFF_NO_COPY(IRenderTarget);
};

using IRenderTargetPtr = Gaff::RefPtr<IRenderTarget>;

NS_END
