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

#include "Gleam_IRenderState.h"

NS_GLEAM

class RenderStateGL : public IRenderState
{
public:
	RenderStateGL(void);
	~RenderStateGL(void);

	bool init(const IRenderDevice&, bool wireframe, bool depth_test, bool stencil_test,
				COMPARISON_FUNC depth_func, StencilData front_face,
				StencilData back_face, unsigned int depth_stencil_ref,
				char stencil_read_mask, char stencil_write_mask,
				CULL_MODE cull_face_mode, bool front_face_counter_clockwise,
				const BlendData* blend_data);

	bool init(const IRenderDevice&, bool wireframe, bool depth_test, bool stencil_test,
				COMPARISON_FUNC depth_func, StencilData front_face,
				StencilData back_face, unsigned int depth_stencil_ref,
				char stencil_read_mask, char stencil_write_mask,
				CULL_MODE cull_face_mode, bool front_face_counter_clockwise,
				BlendData blend_data);

	INLINE void destroy(void);

	void set(IRenderDevice&) const;
	INLINE void unset(IRenderDevice& rd) const;

	INLINE bool isD3D(void) const;

private:
	bool _wireframe;
	bool _depth_test;
	bool _stencil_test;
	COMPARISON_FUNC _depth_func;
	StencilData _front_face;
	StencilData _back_face;
	unsigned int _depth_stencil_ref;
	char _stencil_read_mask;
	char _stencil_write_mask;
	CULL_MODE _cull_face_mode;
	bool _front_face_counter_clockwise;
	BlendData _blend_data[8];
};

NS_END
