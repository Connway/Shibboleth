/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gleam_IRenderTarget.h"
#include "Gleam_Array.h"

NS_GLEAM

class RenderTargetGL : public IRenderTarget
{
public:
	RenderTargetGL(void);
	~RenderTargetGL(void);

	void destroy(void);

	bool addTexture(const IRenderDevice& rd, const ITexture* color_texture, CUBE_FACE face = NONE);
	INLINE void popTexture(void);

	bool addDepthStencilBuffer(const IRenderDevice& rd, const ITexture* depth_stencil_texture);

	INLINE void bind(IRenderDevice& rd);
	INLINE void unbind(const IRenderDevice& rd);

	bool isComplete(void) const;

	bool isD3D(void) const;

private:
	unsigned int _frame_buffer;
	unsigned int _attach_count;

	GleamArray(unsigned int) _draw_buffers;

	INLINE void createFramebuffer(void);
};

NS_END
