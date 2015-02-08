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

#include "Gleam_Shader_OpenGL.h"
#include "Gleam_String.h"
#include <Gaff_File.h>
#include <GL/glew.h>

NS_GLEAM

static unsigned int gl_shader_type[IShader::SHADER_TYPE_SIZE] = {
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_GEOMETRY_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_COMPUTE_SHADER
};

ShaderGL::ShaderGL(void):
	_shader(0)
{
}

ShaderGL::~ShaderGL(void)
{
	destroy();
}

bool ShaderGL::initSource(IRenderDevice&, const char* shader_source, size_t source_size, SHADER_TYPE shader_type)
{
	assert(shader_source && source_size && shader_type < SHADER_TYPE_SIZE);
	_type = shader_type;
	return compileShader(shader_source, static_cast<int>(source_size), shader_type);
}

bool ShaderGL::initSource(IRenderDevice&, const char* shader_source, SHADER_TYPE shader_type)
{
	assert(shader_source && shader_type < SHADER_TYPE_SIZE);
	_type = shader_type;
	return compileShader(shader_source, static_cast<int>(strlen(shader_source)), shader_type);
}

bool ShaderGL::init(IRenderDevice&, const char* file_path, SHADER_TYPE shader_type)
{
	assert(file_path && shader_type < SHADER_TYPE_SIZE);
	_type = shader_type;
	return loadFileAndCompileShader(gl_shader_type[shader_type], file_path);
}

bool ShaderGL::initVertex(IRenderDevice&, const char* file_path)
{
	assert(file_path);
	_type = SHADER_VERTEX;
	return loadFileAndCompileShader(GL_VERTEX_SHADER, file_path);
}

bool ShaderGL::initPixel(IRenderDevice&, const char* file_path)
{
	assert(file_path);
	_type = SHADER_PIXEL;
	return loadFileAndCompileShader(GL_FRAGMENT_SHADER, file_path);
}

bool ShaderGL::initDomain(IRenderDevice&, const char* file_path)
{
	assert(file_path);
	_type = SHADER_DOMAIN;
	return loadFileAndCompileShader(GL_TESS_EVALUATION_SHADER, file_path);
}

bool ShaderGL::initGeometry(IRenderDevice&, const char* file_path)
{
	assert(file_path);
	_type = SHADER_GEOMETRY;
	return loadFileAndCompileShader(GL_GEOMETRY_SHADER, file_path);
}

bool ShaderGL::initHull(IRenderDevice&, const char* file_path)
{
	assert(file_path);
	_type = SHADER_HULL;
	return loadFileAndCompileShader(GL_TESS_CONTROL_SHADER, file_path);
}

bool ShaderGL::initCompute(IRenderDevice&, const char* file_path)
{
	assert(file_path);
	_type = SHADER_COMPUTE;
	return loadFileAndCompileShader(GL_COMPUTE_SHADER, file_path);
}

#ifdef _UNICODE
bool ShaderGL::init(IRenderDevice&, const wchar_t* file_path, SHADER_TYPE shader_type)
{
	assert(file_path && shader_type < SHADER_TYPE_SIZE);
	_type = shader_type;
	return loadFileAndCompileShader(gl_shader_type[shader_type], file_path);
}

bool ShaderGL::initVertex(IRenderDevice&, const wchar_t* file_path)
{
	assert(file_path);
	_type = SHADER_VERTEX;
	return loadFileAndCompileShader(GL_VERTEX_SHADER, file_path);
}

bool ShaderGL::initPixel(IRenderDevice&, const wchar_t* file_path)
{
	assert(file_path);
	_type = SHADER_PIXEL;
	return loadFileAndCompileShader(GL_FRAGMENT_SHADER, file_path);
}

bool ShaderGL::initDomain(IRenderDevice&, const wchar_t* file_path)
{
	assert(file_path);
	_type = SHADER_DOMAIN;
	return loadFileAndCompileShader(GL_TESS_EVALUATION_SHADER, file_path);
}

bool ShaderGL::initGeometry(IRenderDevice&, const wchar_t* file_path)
{
	assert(file_path);
	_type = SHADER_GEOMETRY;
	return loadFileAndCompileShader(GL_GEOMETRY_SHADER, file_path);
}

bool ShaderGL::initHull(IRenderDevice&, const wchar_t* file_path)
{
	assert(file_path);
	_type = SHADER_HULL;
	return loadFileAndCompileShader(GL_TESS_CONTROL_SHADER, file_path);
}

bool ShaderGL::initCompute(IRenderDevice&, const wchar_t* file_path)
{
	assert(file_path);
	_type = SHADER_COMPUTE;
	return loadFileAndCompileShader(GL_COMPUTE_SHADER, file_path);
}
#endif

bool ShaderGL::initVertexSource(IRenderDevice&, const char* source, size_t source_size)
{
	assert(source);
	_type = SHADER_VERTEX;

	if (source_size == SIZE_T_FAIL) {
		source_size = strlen(source);
	}

	return compileShader(source, static_cast<int>(source_size), GL_VERTEX_SHADER);
}

bool ShaderGL::initPixelSource(IRenderDevice&, const char* source, size_t source_size)
{
	assert(source);
	_type = SHADER_PIXEL;

	if (source_size == SIZE_T_FAIL) {
		source_size = strlen(source);
	}

	return compileShader(source, static_cast<int>(source_size), GL_FRAGMENT_SHADER);
}

bool ShaderGL::initDomainSource(IRenderDevice&, const char* source, size_t source_size)
{
	assert(source);
	_type = SHADER_DOMAIN;

	if (source_size == SIZE_T_FAIL) {
		source_size = strlen(source);
	}

	return compileShader(source, static_cast<int>(source_size), GL_TESS_EVALUATION_SHADER);
}

bool ShaderGL::initGeometrySource(IRenderDevice&, const char* source, size_t source_size)
{
	assert(source);
	_type = SHADER_GEOMETRY;

	if (source_size == SIZE_T_FAIL) {
		source_size = strlen(source);
	}

	return compileShader(source, static_cast<int>(source_size), GL_GEOMETRY_SHADER);
}

bool ShaderGL::initHullSource(IRenderDevice&, const char* source, size_t source_size)
{
	assert(source);
	_type = SHADER_HULL;

	if (source_size == SIZE_T_FAIL) {
		source_size = strlen(source);
	}

	return compileShader(source, static_cast<int>(source_size), GL_TESS_CONTROL_SHADER);
}

bool ShaderGL::initComputeSource(IRenderDevice&, const char* source, size_t source_size)
{
	assert(source);
	_type = SHADER_COMPUTE;

	if (source_size == SIZE_T_FAIL) {
		source_size = strlen(source);
	}

	return compileShader(source, static_cast<int>(source_size), GL_COMPUTE_SHADER);
}

void ShaderGL::destroy(void)
{
	if (_shader) {
		glDeleteShader(_shader);
		_shader = 0;
	}
}

bool ShaderGL::isD3D(void) const
{
	return false;
}

GLuint ShaderGL::getShader(void) const
{
	return _shader;
}

#ifdef _UNICODE
bool ShaderGL::loadFileAndCompileShader(unsigned int shader_type, const wchar_t* file_path)
{
	assert(file_path);

	Gaff::File shader(file_path, Gaff::File::READ_BINARY);

	if (!shader.isOpen()) {
		GleamAString msg("Failed to open shader file: ");

		// convert file_path to ASCII
		char dest[256] = { 0 };
		wcstombs(dest, file_path, wcsnlen(file_path, 256));
		msg += dest;

		WriteMessageToLog(msg.getBuffer(), msg.size(), LOG_ERROR);
		return false;
	}

	int shader_size = shader.getFileSize();
	char* shader_src = (char*)GleamAllocate(sizeof(char) * shader_size);

	if (!shader_src || shader_size == -1) {
		return false;
	}

	if (!shader.readEntireFile(shader_src)) {
		GleamFree(shader_src);

		GleamAString msg("Failed to read shader file: ");

		// convert file_path to ASCII
		char dest[256] = { 0 };
		wcstombs(dest, file_path, wcsnlen(file_path, 256));
		msg += dest;

		WriteMessageToLog(msg.getBuffer(), msg.size(), LOG_ERROR);
		return false;
	}

	bool ret = compileShader(shader_src, shader_size, shader_type);
	GleamFree(shader_src);
	return ret;
}
#endif

bool ShaderGL::loadFileAndCompileShader(unsigned int shader_type, const char* file_path)
{
	assert(file_path);

	Gaff::File shader(file_path, Gaff::File::READ_BINARY);

	if (!shader.isOpen()) {
		GleamAString msg("Failed to open shader file: ");
		msg += file_path;

		WriteMessageToLog(msg.getBuffer(), msg.size(), LOG_ERROR);
		return false;
	}

	int shader_size = shader.getFileSize();
	char* shader_src = (char*)GleamAllocate(sizeof(char) * shader_size);

	if (!shader_src || shader_size == -1) {
		return false;
	}

	if (!shader.readEntireFile(shader_src)) {
		GleamFree(shader_src);

		GleamAString msg("Failed to read shader file: ");
		msg += file_path;

		WriteMessageToLog(msg.getBuffer(), msg.size(), LOG_ERROR);
		return false;
	}

	bool ret = compileShader(shader_src, shader_size, shader_type);
	GleamFree(shader_src);
	return ret;
}

bool ShaderGL::compileShader(const char* source, int source_size, unsigned int shader_type)
{
	unsigned int shader = glCreateShader(gl_shader_type[shader_type]);

	if (!shader) {
		return false;
	}

	glShaderSource(shader, 1, (const GLchar**)&source, &source_size);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		GLint log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

		if (log_length) {
			GLchar* log_buffer = (GLchar*)GleamAllocate(sizeof(GLchar) * (log_length + 1));

			if (log_buffer) {
				glGetShaderInfoLog(shader, log_length, NULL, log_buffer);

				WriteMessageToLog(log_buffer, log_length - 1, LOG_ERROR);
				GleamFree(log_buffer);
			}
		}

		glDeleteShader(shader);
		return false;
	}

	//_shader = glCreateShaderProgramv(shader_type, 1, &source);
	_shader = glCreateProgram();

	if (!_shader) {
		glDeleteShader(shader);
		return false;
	}

	glProgramParameteri(_shader, GL_PROGRAM_SEPARABLE, GL_TRUE);

	glAttachShader(_shader, shader);
	glLinkProgram(_shader);
	glDetachShader(_shader, shader);
	glDeleteShader(shader);

	glGetProgramiv(_shader, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) {
		GLint log_length;
		glGetProgramiv(_shader, GL_INFO_LOG_LENGTH, &log_length);

		if (log_length) {
			GLchar* log_buffer = (GLchar*)GleamAllocate(sizeof(GLchar) * (log_length + 1));

			if (log_buffer) {
				glGetProgramInfoLog(_shader, log_length, NULL, log_buffer);

				WriteMessageToLog(log_buffer, log_length - 1, LOG_ERROR);
				GleamFree(log_buffer);
			}
		}

		glDeleteProgram(_shader);
		_shader = 0;

		return false;
	}

	glFinish();
	return glGetError() == GL_NO_ERROR;
}

NS_END
