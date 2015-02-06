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

#pragma once

#include "Gleam_Defines.h"
#include <Gaff_IncludeAssert.h>
#include <Gaff_IRefCounted.h>

NS_GLEAM

class IRenderDevice;

class IRenderState : public Gaff::IRefCounted
{
public:
	enum BLEND_FACTOR
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

	enum BLEND_OP
	{
		BLEND_OP_ADD = 1,
		BLEND_OP_SUBTRACT,
		BLEND_OP_REVERSE_SUBTRACT,
		BLEND_OP_MIN,
		BLEND_OP_MAX,
		BLEND_OP_SIZE = BLEND_OP_MAX
	};

	enum CULL_MODE
	{
		CULL_NONE = 1,
		CULL_FRONT,
		CULL_BACK
	};

	enum COMPARISON_FUNC
	{
		COMPARE_NEVER = 1,
		COMPARE_LESS,
		COMPARE_EQUAL,
		COMPARE_LESS_EQUAL,
		COMPARE_GREATER,
		COMPARE_NOT_EQUAL,
		COMPARE_GREATER_EQUAL,
		COMPARE_ALWAYS,
		COMPARE_SIZE
	};

	enum STENCIL_OP
	{
		STENCIL_KEEP = 1,
		STENCIL_ZERO,
		STENCIL_REPLACE,
		STENCIL_INCREMENT_CLAMP,
		STENCIL_DECREMENT_CLAMP,
		STENCIL_INVERT,
		STENCIL_INCREMENT_WRAP,
		STENCIL_DECREMENT_WRAP,
		STENCIL_OP_SIZE = STENCIL_DECREMENT_WRAP
	};

	enum COLOR_MASK
	{
		COLOR_RED = 1,
		COLOR_GREEN = 2,
		COLOR_BLUE = 4,
		COLOR_ALPHA = 8,
		COLOR_ALL = COLOR_RED | COLOR_GREEN | COLOR_BLUE | COLOR_ALPHA
	};

	struct StencilData
	{
		STENCIL_OP stencil_depth_fail;
		STENCIL_OP stencil_pass_depth_fail;
		STENCIL_OP stencil_depth_pass;
		COMPARISON_FUNC comp_func;
	};

	struct BlendData
	{
		BLEND_FACTOR blend_src_color;
		BLEND_FACTOR blend_dst_color;
		BLEND_OP blend_op_color;
		BLEND_FACTOR blend_src_alpha;
		BLEND_FACTOR blend_dst_alpha;
		BLEND_OP blend_op_alpha;
		char color_write_mask;
		bool enable_alpha_blending;
	};

	struct RenderStateSettings
	{
		BlendData blend_data[8];
		StencilData front_face, back_face;
		unsigned int stencil_ref;
		COMPARISON_FUNC depth_func;
		CULL_MODE cull_face_mode;
		char stencil_read_mask, stencil_write_mask;
		bool wireframe, depth_test, stencil_test;
		bool front_face_counter_clockwise;
		unsigned char blend_data_size;
	};

	IRenderState(void) {}
	virtual ~IRenderState(void) {}

	INLINE bool init(IRenderDevice& rd, const RenderStateSettings& render_state_settings)
	{
		assert(render_state_settings.blend_data_size > 0 && render_state_settings.blend_data_size <= 8);

		if (render_state_settings.blend_data_size == 1) {
			return init(
				rd, render_state_settings.wireframe, render_state_settings.depth_test,
				render_state_settings.stencil_test, render_state_settings.depth_func,
				render_state_settings.front_face, render_state_settings.back_face,
				render_state_settings.stencil_ref, render_state_settings.stencil_read_mask,
				render_state_settings.stencil_write_mask, render_state_settings.cull_face_mode,
				render_state_settings.front_face_counter_clockwise,
				render_state_settings.blend_data[0]
			);
		} else {
			return init(
				rd, render_state_settings.wireframe, render_state_settings.depth_test,
				render_state_settings.stencil_test, render_state_settings.depth_func,
				render_state_settings.front_face, render_state_settings.back_face,
				render_state_settings.stencil_ref, render_state_settings.stencil_read_mask,
				render_state_settings.stencil_write_mask, render_state_settings.cull_face_mode,
				render_state_settings.front_face_counter_clockwise,
				render_state_settings.blend_data
			);
		}
	}

	virtual bool init(IRenderDevice& rd, bool wireframe, bool depth_test, bool stencil_test,
						COMPARISON_FUNC depth_func, StencilData front_face,
						StencilData back_face, unsigned int stencil_ref,
						char stencil_read_mask, char stencil_write_mask,
						CULL_MODE cull_face_mode, bool front_face_counter_clockwise,
						const BlendData* blend_data) = 0;

	virtual bool init(IRenderDevice& rd, bool wireframe, bool depth_test, bool stencil_test,
						COMPARISON_FUNC depth_func, StencilData front_face,
						StencilData back_face, unsigned int stencil_ref,
						char stencil_read_mask, char stencil_write_mask,
						CULL_MODE cull_face_mode, bool front_face_counter_clockwise,
						BlendData blend_data) = 0;

	virtual void destroy(void) = 0;

	virtual void set(IRenderDevice& rd) const = 0;
	virtual void unset(IRenderDevice& rd) const = 0;

	virtual bool isD3D(void) const = 0;

	GAFF_NO_COPY(IRenderState);
};

NS_END
