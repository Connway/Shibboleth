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

#include "Gleam_BlendState_OpenGL.h"
#include "Gleam_RenderDevice_OpenGL.h"
#include <GL/glew.h>

NS_GLEAM

unsigned int BlendStateGL::Blend_Factors[IBlendState::BLEND_FACTOR_SIZE] = {
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA_SATURATE,
	0,
	0,
	GL_SRC1_COLOR,
	GL_ONE_MINUS_SRC1_COLOR,
	GL_SRC1_ALPHA,
	GL_ONE_MINUS_SRC1_ALPHA
};

unsigned int BlendStateGL::Blend_Ops[IBlendState::BLEND_OP_SIZE] = {
	GL_FUNC_ADD,
	GL_FUNC_SUBTRACT,
	GL_FUNC_REVERSE_SUBTRACT,
	GL_MIN,
	GL_MAX
};

BlendStateGL::BlendStateGL(void)
{
}

BlendStateGL::~BlendStateGL(void)
{
}

bool BlendStateGL::init(IRenderDevice& rd, const BlendStateSettings& settings)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_OPENGL);
	_blend_settings[0] = settings;
	return true;
}

bool BlendStateGL::init(IRenderDevice& rd, const BlendStateSettings* settings)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_OPENGL);
	memcpy(_blend_settings, settings, sizeof(BlendStateSettings) * 8);
	return true;
}

void BlendStateGL::destroy(void)
{
}

void BlendStateGL::set(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_OPENGL);
	IRenderDeviceGL& rdgl = reinterpret_cast<IRenderDeviceGL&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	rdgl.setBlendState(this);
}

void BlendStateGL::unset(IRenderDevice& rd) const
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_OPENGL);
	IRenderDeviceGL& rdgl = reinterpret_cast<IRenderDeviceGL&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	rdgl.setBlendState(nullptr);
}

RendererType BlendStateGL::getRendererType() const
{
	return RENDERER_OPENGL;
}

const IBlendState::BlendStateSettings* BlendStateGL::getBlendSettings(void) const
{
	return _blend_settings;
}

NS_END
