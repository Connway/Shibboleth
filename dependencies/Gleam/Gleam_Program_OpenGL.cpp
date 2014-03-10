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

#include "Gleam_Program_OpenGL.h"
#include "Gleam_ShaderResourceView_OpenGL.h"
#include "Gleam_SamplerState_OpenGL.h"
#include "Gleam_Shader_OpenGL.h"
#include "Gleam_Buffer_OpenGL.h"
#include <GL/glew.h>

NS_GLEAM

ProgramGL::ProgramGL(void):
	_program(0)
{
}

ProgramGL::~ProgramGL(void)
{
	destroy();
	IProgram::destroy();
}

bool ProgramGL::init(void)
{
	_program = glCreateProgram();
	return _program != 0;
}

void ProgramGL::destroy(void)
{
	if (_program) {
		glDeleteProgram(_program);
		_program = 0;
	}
}

void ProgramGL::attach(const IShader* shader)
{
	assert(!shader->isD3D());
	glAttachShader(_program, ((const ShaderGL*)shader)->getShader());
}

void ProgramGL::detach(const IShader* shader)
{
	assert(!shader->isD3D());
	glDetachShader(_program, ((const ShaderGL*)shader)->getShader());
}

bool ProgramGL::link(void)
{
	glLinkProgram(_program);

	GLint linked;
	glGetProgramiv(_program, GL_LINK_STATUS, &linked);

	if (linked == GL_FALSE) {
		GLint log_length;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &log_length);

		if (log_length) {
			GLchar* log_buffer = (GLchar*)GleamAllocate(sizeof(GLchar) * (log_length + 1));

			if (log_buffer) {
				glGetProgramInfoLog(_program, log_length, NULL, log_buffer);

				WriteMessageToLog(log_buffer, log_length - 1, LOG_ERROR);
				GleamFree(log_buffer);
			}
		}

		glDeleteProgram(_program);

		return false;
	}

	return glGetError() == GL_NO_ERROR;
}

void ProgramGL::bind(IRenderDevice&)
{
	glUseProgram(_program);

	unsigned int texture_count = 0;
	unsigned int count = 0;

	for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		assert(_sampler_states[i].size() <= _resource_views[i].size());
		unsigned int sampler_count = 0;

		GleamArray<IShaderResourceView*>& resource_views = _resource_views[i];
		GleamArray<ISamplerState*>& sampler_states = _sampler_states[i];
		GleamArray<IBuffer*>& const_bufs = _constant_buffers[i];

		for (unsigned int j = 0; j < const_bufs.size(); ++j) {
			glBindBufferBase(GL_UNIFORM_BUFFER, count, ((const BufferGL*)const_bufs[j])->getBuffer());
			++count;
		}

		for (unsigned int j = 0; j < resource_views.size(); ++j) {
			ShaderResourceViewGL* rv = (ShaderResourceViewGL*)resource_views[j];

			if (resource_views[j]->getViewType() == IShaderResourceView::VIEW_TEXTURE) {
				// should probably assert that texture_count isn't higher than the supported number of textures

				SamplerStateGL* st = (SamplerStateGL*)sampler_states[sampler_count];

				glActiveTexture(GL_TEXTURE0 + texture_count);
				glBindTexture(rv->getTarget(), rv->getResourceView());
				glBindSampler(texture_count, st->getSamplerState());

				++texture_count;
				++sampler_count;

			} else {
				assert(0 && "How is your ShaderResourceView not a texture? That's the only type we have implemented ...");
			}
		}
	}
}

void ProgramGL::unbind(IRenderDevice&)
{
	glUseProgram(0);
}

bool ProgramGL::isD3D(void) const
{
	return false;
}

NS_END
