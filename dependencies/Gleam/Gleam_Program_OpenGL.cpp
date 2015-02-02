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

#include "Gleam_Program_OpenGL.h"
#include "Gleam_ShaderResourceView_OpenGL.h"
#include "Gleam_SamplerState_OpenGL.h"
#include "Gleam_Shader_OpenGL.h"
#include "Gleam_Buffer_OpenGL.h"
#include <GL/glew.h>

NS_GLEAM

unsigned int ProgramGL::_stages[IShader::SHADER_TYPE_SIZE] = {
	GL_VERTEX_SHADER_BIT,
	GL_FRAGMENT_SHADER_BIT,
	GL_TESS_EVALUATION_SHADER_BIT,
	GL_GEOMETRY_SHADER_BIT,
	GL_TESS_CONTROL_SHADER_BIT,
	GL_COMPUTE_SHADER_BIT
};

ProgramGL::ProgramGL(void):
	_program(0)
{
}

ProgramGL::~ProgramGL(void)
{
	destroy();
	ProgramBase::destroy();
}

bool ProgramGL::init(void)
{
#ifndef OPENGL_MULTITHREAD
	glGenProgramPipelines(1, &_program);
	return _program != 0;
#else
	return true;
#endif
}

void ProgramGL::destroy(void)
{
	if (_program) {
		glDeleteProgramPipelines(1, &_program);
		_program = 0;
	}
}

void ProgramGL::attach(IShader* shader)
{
	assert(!shader->isD3D() && shader->getType() >= IShader::SHADER_VERTEX && shader->getType() < IShader::SHADER_TYPE_SIZE);

#ifdef OPENGL_MULTITHREAD
	if (_program) {
		glUseProgramStages(_program, _stages[shader->getType()], ((const ShaderGL*)shader)->getShader());
	}
#else
	glUseProgramStages(_program, _stages[shader->getType()], ((const ShaderGL*)shader)->getShader());
#endif

	_attached_shaders[shader->getType()] = shader;
}

void ProgramGL::detach(IShader::SHADER_TYPE shader)
{
	assert(shader >= IShader::SHADER_VERTEX && shader < IShader::SHADER_TYPE_SIZE);

#ifdef OPENGL_MULTITHREAD
	if (_program) {
		glUseProgramStages(_program, _stages[shader], 0);
	}
#else
	glUseProgramStages(_program, _stages[shader], 0);
#endif

	_attached_shaders[shader] = nullptr;
}

void ProgramGL::bind(IRenderDevice& rd, IProgramBuffers* program_buffers)
{
	// If we are doing multi-threaded resource loading, then we can't create the program until we use it.
	// We are assuming the first time we use it, we are in the main thread (or thread that is solely going to use it).
#ifdef OPENGL_MULTITHREAD
	if (!_program) {
		glGenProgramPipelines(1, &_program);
		if (_program) {
			for (unsigned int i = IShader::SHADER_VERTEX; i < IShader::SHADER_TYPE_SIZE; ++i) {
				if (_attached_shaders[i]) {
					glUseProgramStages(_program, _stages[_attached_shaders[i]->getType()], ((const ShaderGL*)_attached_shaders[i].get())->getShader());
				}
			}
		}
	}
#endif

	glBindProgramPipeline(_program);

	if (program_buffers) {
		assert(!program_buffers->isD3D());
		unsigned int texture_count = 0;
		unsigned int count = 0;

		for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
			const GleamArray<IShaderResourceView*>& resource_views = program_buffers->getResourceViews((Gleam::IShader::SHADER_TYPE)i);
			const GleamArray<ISamplerState*>& sampler_states = program_buffers->getSamplerStates((Gleam::IShader::SHADER_TYPE)i);
			const GleamArray<IBuffer*>& const_bufs = program_buffers->getConstantBuffers((Gleam::IShader::SHADER_TYPE)i);

			assert(sampler_states.size() <= resource_views.size());
			unsigned int sampler_count = 0;

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
}

void ProgramGL::unbind(IRenderDevice&)
{
	glBindProgramPipeline(0);
}

bool ProgramGL::isD3D(void) const
{
	return false;
}

NS_END
