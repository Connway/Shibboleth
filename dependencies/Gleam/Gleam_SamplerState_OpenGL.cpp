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

#include "Gleam_SamplerState_OpenGL.h"
#include <Gaff_IncludeAssert.h>
#include <GL/glew.h>

NS_GLEAM

static GLenum _min_map[ISamplerState::FILTER_SIZE] = {
	GL_NEAREST_MIPMAP_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR,
	GL_NEAREST_MIPMAP_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR
};

static GLenum _mag_map[ISamplerState::FILTER_SIZE] = {
	GL_NEAREST,
	GL_NEAREST,
	GL_LINEAR,
	GL_LINEAR,
	GL_NEAREST,
	GL_NEAREST,
	GL_LINEAR,
	GL_LINEAR,
	GL_LINEAR
};

static GLenum _wrap_map[ISamplerState::WRAP_SIZE] = {
	GL_REPEAT,
	GL_MIRRORED_REPEAT,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_BORDER//,
	//GL_MIRROR_CLAMP_TO_EDGE
};

SamplerStateGL::SamplerStateGL(void):
	_sampler_state(0)
{
}

SamplerStateGL::~SamplerStateGL(void)
{
	destroy();
}

bool SamplerStateGL::init(
	IRenderDevice&,
	FILTER filter, WRAP u, WRAP v, WRAP w,
	float min_lod, float max_lod, float lod_bias,
	unsigned int max_anisotropy,
	//IRenderState::COMPARISON_FUNC compare_func,
	float border_r, float border_g, float border_b, float border_a
	)
{
#ifdef _DEBUG
	GLint ma;
	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ma);
	assert(max_anisotropy <= (unsigned int)ma);
#endif

	float border_color[4] = { border_r, border_g, border_b, border_a };

	glGenSamplers(1, &_sampler_state);
	glSamplerParameteri(_sampler_state, GL_TEXTURE_WRAP_R, _wrap_map[u - 1]);
	glSamplerParameteri(_sampler_state, GL_TEXTURE_WRAP_S, _wrap_map[v - 1]);
	glSamplerParameteri(_sampler_state, GL_TEXTURE_WRAP_T, _wrap_map[w - 1]);
	glSamplerParameteri(_sampler_state, GL_TEXTURE_MIN_FILTER, _min_map[filter]);
	glSamplerParameteri(_sampler_state, GL_TEXTURE_MAG_FILTER, _mag_map[filter]);
	glSamplerParameterf(_sampler_state, GL_TEXTURE_MIN_LOD, min_lod);
	glSamplerParameterf(_sampler_state, GL_TEXTURE_MAX_LOD, max_lod);
	glSamplerParameterf(_sampler_state, GL_TEXTURE_LOD_BIAS, lod_bias);
	glSamplerParameterfv(_sampler_state, GL_TEXTURE_BORDER_COLOR, border_color);

	if (filter == FILTER_ANISOTROPIC) {
		glSamplerParameterf(_sampler_state, GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)max_anisotropy);
	}

	return glGetError() == GL_NO_ERROR;
}

void SamplerStateGL::destroy(void)
{
	if (_sampler_state) {
		glDeleteSamplers(1, &_sampler_state);
		_sampler_state = 0;
	}
}

bool SamplerStateGL::isD3D(void) const
{
	return false;
}

unsigned int SamplerStateGL::getSamplerState(void) const
{
	return _sampler_state;
}

NS_END
