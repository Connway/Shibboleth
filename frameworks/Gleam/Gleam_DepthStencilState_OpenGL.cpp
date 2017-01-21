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

#include "Gleam_DepthStencilState_OpenGL.h"
#include "Gleam_RenderDevice_OpenGL.h"
#include <Gaff_Assert.h>
#include <GL/glew.h>

NS_GLEAM

unsigned int DepthStencilStateGL::Stencil_Ops[IDepthStencilState::STENCIL_OP_SIZE] = {
	GL_KEEP,
	GL_ZERO,
	GL_REPLACE,
	GL_INCR,
	GL_DECR,
	GL_INVERT,
	GL_INCR_WRAP,
	GL_DECR_WRAP
};

unsigned int DepthStencilStateGL::Compare_Funcs[IDepthStencilState::COMPARE_SIZE] = {
	GL_NEVER,
	GL_LESS,
	GL_EQUAL,
	GL_LEQUAL,
	GL_GREATER,
	GL_NOTEQUAL,
	GL_GEQUAL,
	GL_ALWAYS
};

DepthStencilStateGL::DepthStencilStateGL(void)
{
}

DepthStencilStateGL::~DepthStencilStateGL(void)
{
}

bool DepthStencilStateGL::init(IRenderDevice& rd, const DepthStencilStateSettings& settings)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_OPENGL);
	_depth_stencil_settings = settings;
	return true;
}

void DepthStencilStateGL::destroy(void)
{
}

void DepthStencilStateGL::set(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_OPENGL);
	IRenderDeviceGL& rdgl = reinterpret_cast<IRenderDeviceGL&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	rdgl.setDepthStencilState(this);
}

void DepthStencilStateGL::unset(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_OPENGL);
	IRenderDeviceGL& rdgl = reinterpret_cast<IRenderDeviceGL&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	rdgl.setDepthStencilState(nullptr);
}

RendererType DepthStencilStateGL::getRendererType(void) const
{
	return RENDERER_OPENGL;
}

const IDepthStencilState::DepthStencilStateSettings& DepthStencilStateGL::getDepthStencilSettings(void) const
{
	return _depth_stencil_settings;
}

NS_END
