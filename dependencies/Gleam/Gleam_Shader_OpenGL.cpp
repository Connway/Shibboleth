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

#include "Gleam_Shader_OpenGL.h"
#include "Gleam_String.h"
#include <Gaff_File.h>
#include <GL/glew.h>

NS_GLEAM

ShaderGL::ShaderGL(void):
	_shader(0)
{
}

ShaderGL::~ShaderGL(void)
{
	destroy();
}

bool ShaderGL::initVertex(IRenderDevice&, const GleamGString& file_path)
{
	assert(file_path.size());
	_type = SHADER_VERTEX;
	return loadFileAndCompileShader(GL_VERTEX_SHADER, file_path.getBuffer());
}

bool ShaderGL::initPixel(IRenderDevice&, const GleamGString& file_path)
{
	assert(file_path.size());
	_type = SHADER_PIXEL;
	return loadFileAndCompileShader(GL_FRAGMENT_SHADER, file_path.getBuffer());
}

bool ShaderGL::initDomain(IRenderDevice&, const GleamGString& file_path)
{
	assert(file_path.size());
	_type = SHADER_DOMAIN;
	return loadFileAndCompileShader(GL_TESS_EVALUATION_SHADER, file_path.getBuffer());
}

bool ShaderGL::initGeometry(IRenderDevice&, const GleamGString& file_path)
{
	assert(file_path.size());
	_type = SHADER_GEOMETRY;
	return loadFileAndCompileShader(GL_GEOMETRY_SHADER, file_path.getBuffer());
}

bool ShaderGL::initHull(IRenderDevice&, const GleamGString& file_path)
{
	assert(file_path.size());
	_type = SHADER_HULL;
	return loadFileAndCompileShader(GL_TESS_CONTROL_SHADER, file_path.getBuffer());
}

bool ShaderGL::initCompute(IRenderDevice&, const GleamGString& file_path)
{
	assert(file_path.size());
	_type = SHADER_COMPUTE;
	return loadFileAndCompileShader(GL_COMPUTE_SHADER, file_path.getBuffer());
}

bool ShaderGL::initVertex(IRenderDevice&, const GChar* file_path)
{
	assert(file_path);
	_type = SHADER_VERTEX;
	return loadFileAndCompileShader(GL_VERTEX_SHADER, file_path);
}

bool ShaderGL::initPixel(IRenderDevice&, const GChar* file_path)
{
	assert(file_path);
	_type = SHADER_PIXEL;
	return loadFileAndCompileShader(GL_FRAGMENT_SHADER, file_path);
}

bool ShaderGL::initDomain(IRenderDevice&, const GChar* file_path)
{
	assert(file_path);
	_type = SHADER_DOMAIN;
	return loadFileAndCompileShader(GL_TESS_EVALUATION_SHADER, file_path);
}

bool ShaderGL::initGeometry(IRenderDevice&, const GChar* file_path)
{
	assert(file_path);
	_type = SHADER_GEOMETRY;
	return loadFileAndCompileShader(GL_GEOMETRY_SHADER, file_path);
}

bool ShaderGL::initHull(IRenderDevice&, const GChar* file_path)
{
	assert(file_path);
	_type = SHADER_HULL;
	return loadFileAndCompileShader(GL_TESS_CONTROL_SHADER, file_path);
}

bool ShaderGL::initCompute(IRenderDevice&, const GChar* file_path)
{
	assert(file_path);
	_type = SHADER_COMPUTE;
	return loadFileAndCompileShader(GL_COMPUTE_SHADER, file_path);
}

bool ShaderGL::initVertexSource(IRenderDevice&, const GleamAString& source)
{
	assert(source.size());
	_type = SHADER_VERTEX;
	return compileShader(source.getBuffer(), source.size(), GL_VERTEX_SHADER);
}

bool ShaderGL::initPixelSource(IRenderDevice&, const GleamAString& source)
{
	assert(source.size());
	_type = SHADER_PIXEL;
	return compileShader(source.getBuffer(), source.size(), GL_FRAGMENT_SHADER);
}

bool ShaderGL::initDomainSource(IRenderDevice&, const GleamAString& source)
{
	assert(source.size());
	_type = SHADER_DOMAIN;
	return compileShader(source.getBuffer(), source.size(), GL_TESS_EVALUATION_SHADER);
}

bool ShaderGL::initGeometrySource(IRenderDevice&, const GleamAString& source)
{
	assert(source.size());
	_type = SHADER_GEOMETRY;
	return compileShader(source.getBuffer(), source.size(), GL_GEOMETRY_SHADER);
}

bool ShaderGL::initHullSource(IRenderDevice&, const GleamAString& source)
{
	assert(source.size());
	_type = SHADER_HULL;
	return compileShader(source.getBuffer(), source.size(), GL_TESS_CONTROL_SHADER);
}

bool ShaderGL::initComputeSource(IRenderDevice&, const GleamAString& source)
{
	assert(source.size());
	_type = SHADER_COMPUTE;
	return compileShader(source.getBuffer(), source.size(), GL_COMPUTE_SHADER);
}

bool ShaderGL::initVertexSource(IRenderDevice&, const char* source)
{
	assert(source);
	_type = SHADER_VERTEX;
	return compileShader(source, (int)strlen(source), GL_VERTEX_SHADER);
}

bool ShaderGL::initPixelSource(IRenderDevice&, const char* source)
{
	assert(source);
	_type = SHADER_PIXEL;
	return compileShader(source, (int)strlen(source), GL_FRAGMENT_SHADER);
}

bool ShaderGL::initDomainSource(IRenderDevice&, const char* source)
{
	assert(source);
	_type = SHADER_DOMAIN;
	return compileShader(source, (int)strlen(source), GL_TESS_EVALUATION_SHADER);
}

bool ShaderGL::initGeometrySource(IRenderDevice&, const char* source)
{
	assert(source);
	_type = SHADER_GEOMETRY;
	return compileShader(source, (int)strlen(source), GL_GEOMETRY_SHADER);
}

bool ShaderGL::initHullSource(IRenderDevice&, const char* source)
{
	assert(source);
	_type = SHADER_HULL;
	return compileShader(source, (int)strlen(source), GL_TESS_CONTROL_SHADER);
}

bool ShaderGL::initComputeSource(IRenderDevice&, const char* source)
{
	assert(source);
	_type = SHADER_COMPUTE;
	return compileShader(source, (int)strlen(source), GL_COMPUTE_SHADER);
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

bool ShaderGL::loadFileAndCompileShader(unsigned int shader_type, const GChar* file_path)
{
	assert(file_path);

	Gaff::File shader(file_path, Gaff::File::READ_BINARY);

	if (!shader.isOpen()) {
		GleamAString msg("Failed to open shader file: ");

		#ifdef _UNICODE	
			// convert file_path to ASCII
		#else
			msg += file_path;
		#endif

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

		#ifdef _UNICODE
			// convert file_path to ASCII
		#else
			msg += file_path;
		#endif

		WriteMessageToLog(msg.getBuffer(), msg.size(), LOG_ERROR);
		return false;
	}

	bool ret = compileShader(shader_src, shader_size, shader_type);
	GleamFree(shader_src);
	return ret;
}

bool ShaderGL::compileShader(const char* source, int source_size, unsigned int shader_type)
{
	_shader = glCreateShader(shader_type);

	if (!_shader) {
		return false;
	}

	glShaderSource(_shader, 1, (const GLchar**)&source, &source_size);
	glCompileShader(_shader);

	GLint status;
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		GLint log_length;
		glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &log_length);

		if (log_length) {
			GLchar* log_buffer = (GLchar*)GleamAllocate(sizeof(GLchar) * (log_length + 1));

			if (log_buffer) {
				glGetShaderInfoLog(_shader, log_length, NULL, log_buffer);

				WriteMessageToLog(log_buffer, log_length - 1, LOG_ERROR);
				GleamFree(log_buffer);
			}
		}

		glDeleteShader(_shader);
		_shader = 0;
		return false;
	}

	return glGetError() == GL_NO_ERROR;
}

NS_END
