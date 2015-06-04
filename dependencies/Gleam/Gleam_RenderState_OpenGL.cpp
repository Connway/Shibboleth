/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gleam_RenderState_OpenGL.h"
#include "Gleam_RenderDevice_OpenGL.h"
#include <Gaff_IncludeAssert.h>
#include <GL/glew.h>

NS_GLEAM

RenderStateGL::DisableEnable RenderStateGL::Disable_Enable_Funcs[2] = { glDisable, glEnable };

unsigned int RenderStateGL::Stencil_Ops[IRenderState::STENCIL_OP_SIZE] = {
	GL_KEEP,
	GL_ZERO,
	GL_REPLACE,
	GL_INCR,
	GL_DECR,
	GL_INVERT,
	GL_INCR_WRAP,
	GL_DECR_WRAP
};

unsigned int RenderStateGL::Blend_Factors[IRenderState::BLEND_FACTOR_SIZE] = {
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

unsigned int RenderStateGL::Blend_Ops[IRenderState::BLEND_OP_SIZE] = {
	GL_FUNC_ADD,
	GL_FUNC_SUBTRACT,
	GL_FUNC_REVERSE_SUBTRACT,
	GL_MIN,
	GL_MAX
};

unsigned int RenderStateGL::Compare_Funcs[IRenderState::COMPARE_SIZE] = {
	GL_NEVER,
	GL_LESS,
	GL_EQUAL,
	GL_LEQUAL,
	GL_GREATER,
	GL_NOTEQUAL,
	GL_GEQUAL,
	GL_ALWAYS
};


RenderStateGL::RenderStateGL(void):
	_wireframe(false), _depth_test(false), _stencil_test(false),
	_depth_func(COMPARE_ALWAYS), _stencil_ref(0), _stencil_read_mask(0xFF),
	_stencil_write_mask(0xFF), _cull_face_mode(CULL_BACK), _front_face_counter_clockwise(false)
{
}

RenderStateGL::~RenderStateGL(void)
{
}

bool RenderStateGL::init(IRenderDevice&, bool wireframe, bool depth_test, bool stencil_test,
							COMPARISON_FUNC depth_func, StencilData front_face,
							StencilData back_face, unsigned int stencil_ref,
							char stencil_read_mask, char stencil_write_mask,
							CULL_MODE cull_face_mode, bool front_face_counter_clockwise,
							const BlendData* blend_data)
{
	_wireframe = wireframe;
	_depth_test = depth_test;
	_stencil_test = stencil_test;
	_depth_func = depth_func;
	_front_face = front_face;
	_back_face = back_face;
	_stencil_ref = stencil_ref;
	_stencil_read_mask = stencil_read_mask;
	_stencil_write_mask = stencil_write_mask;
	_cull_face_mode = cull_face_mode;
	_front_face_counter_clockwise = front_face_counter_clockwise;

	for (unsigned int i = 0; i < 8; ++i) {
		_blend_data[i] = *(blend_data + i);
	}

	return true;
}

bool RenderStateGL::init(IRenderDevice&, bool wireframe, bool depth_test, bool stencil_test,
							COMPARISON_FUNC depth_func, StencilData front_face,
							StencilData back_face, unsigned int stencil_ref,
							char stencil_read_mask, char stencil_write_mask,
							CULL_MODE cull_face_mode, bool front_face_counter_clockwise,
							BlendData blend_data)
{
	_wireframe = wireframe;
	_depth_test = depth_test;
	_stencil_test = stencil_test;
	_depth_func = depth_func;
	_front_face = front_face;
	_back_face = back_face;
	_stencil_ref = stencil_ref;
	_stencil_read_mask = stencil_read_mask;
	_stencil_write_mask = stencil_write_mask;
	_cull_face_mode = cull_face_mode;
	_front_face_counter_clockwise = front_face_counter_clockwise;
	_blend_data[0] = blend_data;

	for (unsigned int i = 1; i < 8; ++i) {
		_blend_data[i].enable_alpha_blending = false;
	}

	return true;
}

void RenderStateGL::destroy(void)
{
}

void RenderStateGL::set(IRenderDevice& rd) const
{
	assert(!rd.isD3D());
	IRenderDeviceGL& rdgl = reinterpret_cast<IRenderDeviceGL&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	rdgl.setRenderState(this);
}

void RenderStateGL::unset(IRenderDevice& rd) const
{
	rd.resetRenderState();
}

bool RenderStateGL::isD3D(void) const
{
	return false;
}

bool RenderStateGL::isWireframe(void) const
{
	return _wireframe;
}

bool RenderStateGL::isDepthTestEnabled(void) const
{
	return _depth_test;
}

bool RenderStateGL::isStencilTestEnabled(void) const
{
	return _stencil_test;
}

IRenderState::COMPARISON_FUNC RenderStateGL::getDepthFunc(void) const
{
	return _depth_func;
}

const IRenderState::StencilData& RenderStateGL::getFrontFaceStencilData(void) const
{
	return _front_face;
}

const IRenderState::StencilData& RenderStateGL::getBackFaceStencilData(void) const
{
	return _back_face;
}

unsigned int RenderStateGL::getStencilRef(void) const
{
	return _stencil_ref;
}

char RenderStateGL::getStencilReadMask(void) const
{
	return _stencil_read_mask;
}

char RenderStateGL::getStencilWriteMask(void) const
{
	return _stencil_write_mask;
}

IRenderState::CULL_MODE RenderStateGL::getCullFaceMode(void) const
{
	return _cull_face_mode;
}

bool RenderStateGL::isFrontFaceCounterClockwise(void) const
{
	return _front_face_counter_clockwise;
}

const IRenderState::BlendData* RenderStateGL::getBlendData(void) const
{
	return _blend_data;
}

NS_END
