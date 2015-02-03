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
#include "Gleam_IRenderDevice_OpenGL.h"
#include "Gleam_SamplerState_OpenGL.h"
#include "Gleam_IRenderDevice.h"
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

	assert(!rd.isD3D() && !program_buffers->isD3D());
	IRenderDeviceGL& rdgl = (IRenderDeviceGL&)*(((const char*)&rd) + sizeof(IRenderDevice));
	rdgl.bindShader(this, (ProgramBuffersGL*)program_buffers);
}

void ProgramGL::unbind(IRenderDevice& rd)
{
	assert(!rd.isD3D());
	IRenderDeviceGL& rdgl = (IRenderDeviceGL&)*(((const char*)&rd) + sizeof(IRenderDevice));
	rdgl.unbindShader();
}

bool ProgramGL::isD3D(void) const
{
	return false;
}

unsigned int ProgramGL::getProgram(void) const
{
	return _program;
}

NS_END
