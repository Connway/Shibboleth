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

#include "Gleam_ShaderProgramManager.h"
#include "Gleam_Program_Direct3D.h"
#include "Gleam_Program_OpenGL.h"
#include "Gleam_Shader_Direct3D.h"
#include "Gleam_Shader_OpenGL.h"
#include "Gleam_IRenderDevice.h"

NS_GLEAM

ShaderProgramManager::ShaderProgramManager(void)
{
}

ShaderProgramManager::~ShaderProgramManager(void)
{
	destroy();
}

void ShaderProgramManager::destroy(void)
{
	_program_map.clear();
	_shader_map.clear();

	for (unsigned int i = 0; i < _programs.size(); ++i) {
		_programs[i]->release();
	}

	for (unsigned int i = 0; i < _shaders.size(); ++i) {
		_shaders[i]->release();
	}

	_programs.clear();
	_shaders.clear();
}

IProgram* ShaderProgramManager::createProgram(const GleamAString& name)
{
#ifdef USE_DX
	IProgram* program = GleamClassAllocate(ProgramD3D);
#else
	IProgram* program = GleamClassAllocate(ProgramGL);
#endif

	if (!program) {
		return NULLPTR;
	}

	_programs.push((IProgram* const)program);
	addProgramEntry(name);
	program->addRef();
	program->init();

	return program;
}

IShader* ShaderProgramManager::createShader(const IRenderDevice& rd, const GleamGString& file_path, IShader::SHADER_TYPE shader_type, const GleamAString& name)
{
#ifdef USE_DX
	IShader* shader = GleamClassAllocate(ShaderD3D);
#else
	IShader* shader = GleamClassAllocate(ShaderGL);
#endif

	if (!shader) {
		return NULLPTR;
	}

	bool result = false;

	switch (shader_type) {
		case IShader::SHADER_VERTEX:
			result = shader->initVertex(rd, file_path);
			break;

		case IShader::SHADER_PIXEL:
			result = shader->initPixel(rd, file_path);
			break;

		case IShader::SHADER_DOMAIN:
			result = shader->initDomain(rd, file_path);
			break;

		case IShader::SHADER_GEOMETRY:
			result = shader->initGeometry(rd, file_path);
			break;

		case IShader::SHADER_HULL:
			result = shader->initHull(rd, file_path);
			break;

		case IShader::SHADER_COMPUTE:
			result = shader->initCompute(rd, file_path);
			break;
	}

	if (!result) {
		GleamFree(shader);
		return NULLPTR;
	}

	_shaders.push((IShader* const)shader);
	addShaderEntry(name);
	shader->addRef();

	return shader;
}

int ShaderProgramManager::addProgram(IProgram* program, const GleamAString& name)
{
	assert(program);

	_programs.push(program);
	addProgramEntry(name);
	program->addRef();

	return _programs.size() - 1;
}

int ShaderProgramManager::addShader(IShader* shader, const GleamAString& name)
{
	assert(shader);

	_shaders.push(shader);
	addShaderEntry(name);
	shader->addRef();

	return _shaders.size() - 1;
}

bool ShaderProgramManager::removeProgram(const IProgram* program)
{
	assert(program);
	int index = _programs.linearFind((IProgram* const)program);

	if (index > -1) {
		_programs.erase(index);
		program->release();
		return true;
	}

	return false;
}

void ShaderProgramManager::removeProgram(int index)
{
	assert(index > -1 && (unsigned int)index < _programs.size());
	_programs[index]->release();
	_programs.erase(index);
}

bool ShaderProgramManager::removeShader(const IShader* shader)
{
	assert(shader);
	int index = _shaders.linearFind((IShader* const)shader);

	if (index > -1) {
		_shaders.erase(index);
		shader->release();
		return true;
	}

	return false;
}

void ShaderProgramManager::removeShader(int index)
{
	assert(index > -1 && (unsigned int)index < _shaders.size());
	_shaders[index]->release();
	_shaders.erase(index);
}

int ShaderProgramManager::getProgramIndex(const GleamAString& name) const
{
	assert(name.size());
	return _program_map[name];
}

int ShaderProgramManager::getShaderIndex(const GleamAString& name) const
{
	assert(name.size());
	return _shader_map[name];
}

int ShaderProgramManager::getIndex(const IProgram* program) const
{
	assert(program);
	return _programs.linearFind((IProgram* const)program);
}

int ShaderProgramManager::getIndex(const IShader* shader) const
{
	assert(shader);
	return _shaders.linearFind((IShader* const)shader);
}

const IProgram* ShaderProgramManager::getProgram(int index) const
{
	assert(index > -1 && (unsigned int)index < _programs.size());
	return _programs[index];
}

IProgram* ShaderProgramManager::getProgram(int index)
{
	assert(index > -1 && (unsigned int)index < _programs.size());
	return _programs[index];
}

const IShader* ShaderProgramManager::getShader(int index) const
{
	assert(index > -1 && (unsigned int)index < _shaders.size());
	return _shaders[index];
}

IShader* ShaderProgramManager::getShader(int index)
{
	assert(index > -1 && (unsigned int)index < _shaders.size());
	return _shaders[index];
}

void ShaderProgramManager::addProgramEntry(const GleamAString& name)
{
	if (!name.size()) {
		return;
	}

	int index = _programs.size() - 1;
	assert(_program_map.indexOf(name) == -1);
	_program_map[name] = index;
}

void ShaderProgramManager::addShaderEntry(const GleamAString& name)
{
	if (!name.size()) {
		return;
	}

	int index = _shaders.size() - 1;
	assert(_shader_map.indexOf(name) == -1);
	_shader_map[name] = index;
}

NS_END
