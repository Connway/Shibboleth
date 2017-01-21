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

#include "Gleam_IRenderTarget.h"
#include "Gleam_RefCounted.h"
#include "Gleam_Array.h"
#include <Gaff_Pair.h>

NS_GLEAM

class RenderTargetGL : public IRenderTarget
{
public:
	RenderTargetGL(void);
	~RenderTargetGL(void);

	bool init(void);
	void destroy(void);

	bool addTexture(IRenderDevice& rd, const ITexture* color_texture, CUBE_FACE face = NONE);
	void popTexture(void);

	bool addDepthStencilBuffer(IRenderDevice& rd, const ITexture* depth_stencil_texture);

	void bind(IRenderDevice& rd);
	void unbind(IRenderDevice& rd);

	void clear(IRenderDevice& rd, unsigned int clear_flags = CLEAR_DEPTH | CLEAR_STENCIL | CLEAR_COLOR, float clear_depth = 1.0f, unsigned char clear_stencil = 0, float* clear_color = nullptr);

	bool isComplete(void) const;

	RendererType getRendererType(void) const;

private:
#ifdef OPENGL_MULTITHREAD
	GleamArray< Gaff::Pair<const ITexture*, CUBE_FACE> > _color_textures;
	const ITexture* _depth_stencil_texture;
#endif

	GleamArray<unsigned int> _draw_buffers;

	unsigned int _frame_buffer;
	unsigned int _attach_count;

	int _viewport_width;
	int _viewport_height;

	friend class RenderDeviceGL;
	void setViewport(int viewport_width, int viewport_height);

	GLEAM_REF_COUNTED(RenderTargetGL);
};

NS_END
