/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

typedef void (__stdcall *DisableEnable)(GLenum);
typedef void (__stdcall *DisableEnablei)(GLenum, GLuint);

static DisableEnable _disableEnable[2] = { glDisable, glEnable };

static GLenum _stencil_ops[IRenderState::STENCIL_OP_SIZE] = {
	GL_KEEP,
	GL_ZERO,
	GL_REPLACE,
	GL_INCR,
	GL_DECR,
	GL_INVERT,
	GL_INCR_WRAP,
	GL_DECR_WRAP
};

static GLenum _blend_factors[IRenderState::BLEND_FACTOR_SIZE] = {
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

static GLenum _blend_ops[IRenderState::BLEND_OP_SIZE] = {
	GL_FUNC_ADD,
	GL_FUNC_SUBTRACT,
	GL_FUNC_REVERSE_SUBTRACT,
	GL_MIN,
	GL_MAX
};

static GLenum _compare_funcs[IRenderState::COMPARE_SIZE] = {
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
	_depth_func(COMPARE_ALWAYS), _depth_stencil_ref(0), _stencil_read_mask(0xFF),
	_stencil_write_mask(0xFF), _cull_face_mode(CULL_BACK), _front_face_counter_clockwise(false)
{
}

RenderStateGL::~RenderStateGL(void)
{
}

bool RenderStateGL::init(IRenderDevice&, bool wireframe, bool depth_test, bool stencil_test,
							COMPARISON_FUNC depth_func, StencilData front_face,
							StencilData back_face, unsigned int depth_stencil_ref,
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
	_depth_stencil_ref = depth_stencil_ref;
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
							StencilData back_face, unsigned int depth_stencil_ref,
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
	_depth_stencil_ref = depth_stencil_ref;
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

void RenderStateGL::set(IRenderDevice&) const
{
	glPolygonMode(GL_FRONT_AND_BACK, (_wireframe) ? GL_LINE : GL_FILL);

	_disableEnable[_depth_test](GL_DEPTH_TEST);
	_disableEnable[_stencil_test](GL_STENCIL_TEST);
	glDepthFunc(_compare_funcs[_depth_func - 1]);

	glStencilOpSeparate(GL_FRONT, _stencil_ops[_front_face.stencil_depth_fail - 1],
						_stencil_ops[_front_face.stencil_pass_depth_fail - 1],
						_stencil_ops[_front_face.stencil_depth_pass - 1]);
	glStencilOpSeparate(GL_BACK, _stencil_ops[_back_face.stencil_depth_fail - 1],
						_stencil_ops[_back_face.stencil_pass_depth_fail - 1],
						_stencil_ops[_back_face.stencil_depth_pass - 1]);
	glStencilFuncSeparate(GL_FRONT, _compare_funcs[_front_face.comp_func - 1], _depth_stencil_ref, _stencil_read_mask);
	glStencilFuncSeparate(GL_BACK, _compare_funcs[_back_face.comp_func - 1], _depth_stencil_ref, _stencil_read_mask);
	glStencilMask(_stencil_write_mask);

	_disableEnable[_cull_face_mode != CULL_NONE](GL_CULL_FACE);
	glCullFace(GL_FRONT - 2 + _cull_face_mode);
	glFrontFace((_front_face_counter_clockwise) ? GL_CCW : GL_CW);

	for (unsigned int i = 0; i < 8; ++i) {
		if (_blend_data[i].enable_alpha_blending) {
			glEnablei(GL_BLEND, i);

			glBlendFuncSeparatei(i, _blend_factors[_blend_data[i].blend_src_color - 1], _blend_factors[_blend_data[i].blend_dst_color - 1],
								_blend_factors[_blend_data[i].blend_src_alpha - 1], _blend_factors[_blend_data[i].blend_dst_alpha - 1]);

			glBlendEquationSeparatei(i, _blend_ops[_blend_data[i].blend_op_color - 1], _blend_ops[_blend_data[i].blend_op_alpha - 1]);

		} else {
			glDisablei(GL_BLEND, i);
		}
	}
}

void RenderStateGL::unset(IRenderDevice& rd) const
{
	rd.resetRenderState();
}

bool RenderStateGL::isD3D(void) const
{
	return false;
}

NS_END
