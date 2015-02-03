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

#include "Gleam_Layout_OpenGL.h"
#include "Gleam_RenderDevice_OpenGL.h"
#include "Gleam_Texture_OpenGL.h"
#include "Gleam_Shader_OpenGL.h"
#include "Gleam_Buffer_OpenGL.h"
#include "Gleam_IMesh.h"
#include <GL/glew.h>

NS_GLEAM

struct SizeType
{
	GLint size;
	GLenum type;
	bool normalized;
	unsigned int size_bytes;
};

static SizeType _format_sizes[ITexture::FORMAT_SIZE] = {
		{ 1, GL_UNSIGNED_BYTE, true, 1 }, //R_8_UNORM,
		{ 1, GL_UNSIGNED_SHORT, true, 2 }, //R_16_UNORM,
		{ 2, GL_UNSIGNED_BYTE, true, 2 }, //RG_8_UNORM,
		{ 2, GL_UNSIGNED_SHORT, true, 4 }, //RG_16_UNORM,
		//{ 3, GL_UNSIGNED_BYTE, true, 3 }, //RGB_8_UNORM,
		{ 4, GL_UNSIGNED_BYTE, true, 4 }, //RGBA_8_UNORM,
		{ 4, GL_UNSIGNED_SHORT, true, 8 }, //RGBA_16_UNORM,

		{ 1, GL_BYTE, true, 1 }, //R_8_SNORM,
		{ 1, GL_SHORT, true, 2 }, //R_16_SNORM,
		{ 2, GL_BYTE, true, 2 }, //RG_8_SNORM,
		{ 2, GL_SHORT, true, 4 }, //RG_16_SNORM,
		//{ 3, GL_BYTE, true, 3 }, //RGB_8_SNORM,
		{ 3, GL_SHORT, true, 6 }, //RGB_16_SNORM,
		{ 4, GL_BYTE, true, 4 }, //RGBA_8_SNORM,

		{ 1, GL_BYTE, false, 1 }, //R_8_I,
		{ 1, GL_SHORT, false, 2 }, //R_16_I,
		{ 1, GL_INT, false, 4 }, //R_32_I,
		{ 2, GL_BYTE, false, 2 }, //RG_8_I,
		{ 2, GL_SHORT, false, 4 }, //RG_16_I,
		{ 2, GL_INT, false, 8 }, //RG_32_I,
		//{ 3, GL_BYTE, false, 3 }, //RGB_8_I,
		//{ 3, GL_SHORT, false, 6 }, //RGB_16_I,
		{ 3, GL_INT, false, 12 }, //RGB_32_I,
		{ 4, GL_BYTE, false, 4 }, //RGBA_8_I,
		{ 4, GL_SHORT, false, 8 }, //RGBA_16_I,
		{ 4, GL_INT, false, 16 }, //RGBA_32_I,

		{ 1, GL_UNSIGNED_BYTE, false, 1 }, //R_8_UI,
		{ 1, GL_UNSIGNED_SHORT, false, 2 }, //R_16_UI,
		{ 1, GL_UNSIGNED_INT, false, 4 }, //R_32_UI,
		{ 2, GL_UNSIGNED_BYTE, false, 2 }, //RG_8_UI,
		{ 2, GL_UNSIGNED_SHORT, false, 4 }, //RG_16_UI,
		{ 2, GL_UNSIGNED_INT, false, 8 }, //RG_32_UI,
		//{ 3, GL_UNSIGNED_BYTE, false, 3 }, //RGB_8_UI,
		//{ 3, GL_UNSIGNED_SHORT, false, 6 }, //RGB_16_UI,
		{ 3, GL_UNSIGNED_INT, false, 12 }, //RGB_32_UI,
		{ 4, GL_UNSIGNED_BYTE, false, 4 }, //RGBA_8_UI,
		{ 4, GL_UNSIGNED_SHORT, false, 8 }, //RGBA_16_UI,
		{ 4, GL_UNSIGNED_INT, false, 16 }, //RGBA_32_UI,

		{ 1, GL_HALF_FLOAT, false, 2 }, //R_16_F,
		{ 1, GL_FLOAT, false, 4 }, //R_32_F,
		{ 2, GL_HALF_FLOAT, false, 4 }, //RG_16_F,
		{ 2, GL_FLOAT, false, 8 }, //RG_32_F,
		{ 3, GL_HALF_FLOAT, false, 6 }, //RGB_16_F,
		{ 3, GL_FLOAT, false, 12 }, //RGB_32_F,
		{ 4, GL_HALF_FLOAT, false, 8 }, //RGBA_16_F,
		{ 4, GL_FLOAT, false, 16 }, //RGBA_32_F,

		{ 1, GL_UNSIGNED_INT_10F_11F_11F_REV, false, 4 }, //RGB_11_11_10_F,
		{ 1, GL_INT, false, 4 }, //RGBE_9_9_9_5,
		{ 1, GL_UNSIGNED_INT_2_10_10_10_REV, true, 4 }, //RGBA_10_10_10_2_UNORM,
		{ 1, GL_UNSIGNED_INT_2_10_10_10_REV, false, 4 }, //RGBA_10_10_10_2_UI,
		{ 4, GL_BYTE, true, 4 }, //SRGBA_8_UNORM,

		{ 1, GL_HALF_FLOAT, true, 2 }, //DEPTH_16_UNORM,
		{ 1, GL_FLOAT, false, 4 }, //DEPTH_32_F,
		{ 1, GL_UNSIGNED_INT, true, 4 }, //DEPTH_STENCIL_24_8_UNORM_UI,
		{ 2, GL_FLOAT, false, 8 } //DEPTH_STENCIL_32_8_F
};

LayoutGL::LayoutGL(void)
{
}

LayoutGL::~LayoutGL(void)
{
	destroy();
}

// Ugh, this function is so ugly looking
bool LayoutGL::init(IRenderDevice&, const LayoutDescription* layout_desc, unsigned int layout_desc_size, const IShader*)
{
	LayoutData layout_data;
	unsigned int input_slot = 0;

	for (unsigned int i = 0; i < layout_desc_size; ++i) {
		input_slot = layout_desc[i].input_slot;

		if (_layout_descs.size() <= input_slot) {
			_layout_descs.push(GleamArray<LayoutData>());
		}

		layout_data.size = _format_sizes[layout_desc[i].format].size;
		layout_data.type = _format_sizes[layout_desc[i].format].type;
		layout_data.normalized = _format_sizes[layout_desc[i].format].normalized;

		if (layout_desc[i].aligned_byte_offset == APPEND_ALIGNED) {
			assert(i > 0);
			//layout_data.aligned_byte_offset = _format_sizes[layout_desc[i - 1].format].size_bytes;
			layout_data.aligned_byte_offset = _format_sizes[layout_desc[i - 1].format].size_bytes + _layout_descs[input_slot].last().aligned_byte_offset;

		} else {
			layout_data.aligned_byte_offset = layout_desc[i].aligned_byte_offset;
		}

		_layout_descs[input_slot].push(layout_data);
	}

	return true;
}

void LayoutGL::destroy(void)
{
	_layout_descs.clear();
}

void LayoutGL::setLayout(IRenderDevice& rd, const IMesh* mesh)
{
	assert(!mesh->isD3D() && !rd.isD3D());
	IRenderDeviceGL& rdgl = (IRenderDeviceGL&)*(((const char*)&rd) + sizeof(IRenderDevice));
	rdgl.setLayout(this, mesh);
}

void LayoutGL::unsetLayout(IRenderDevice& rd)
{
	assert(!rd.isD3D());
	IRenderDeviceGL& rdgl = (IRenderDeviceGL&)*(((const char*)&rd) + sizeof(IRenderDevice));
	rdgl.unsetLayout(this);
}

const GleamArray< GleamArray<LayoutGL::LayoutData> >& LayoutGL::GetLayoutDescriptors(void) const
{
	return _layout_descs;
}

NS_END
