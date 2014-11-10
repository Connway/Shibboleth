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

#pragma once

#include "Gleam_RefCounted.h"
#include "Gleam_IShader.h"

NS_GLEAM

class ShaderGL : public IShader
{
public:
	ShaderGL(void);
	~ShaderGL(void);

	bool initSource(IRenderDevice&, const char* shader_source, unsigned int source_size, SHADER_TYPE shader_type);
	bool initSource(IRenderDevice&, const char* shader_source, SHADER_TYPE shader_type);
	bool init(IRenderDevice&, const char* file_path, SHADER_TYPE shader_type);

	bool initVertex(IRenderDevice&, const char* file_path);
	bool initPixel(IRenderDevice&, const char* file_path);
	bool initDomain(IRenderDevice&, const char* file_path);
	bool initGeometry(IRenderDevice&, const char* file_path);
	bool initHull(IRenderDevice&, const char* file_path);
	bool initCompute(IRenderDevice&, const char* file_path);

#ifdef _UNICODE
	bool init(IRenderDevice&, const wchar_t* file_path, SHADER_TYPE shader_type);

	bool initVertex(IRenderDevice&, const wchar_t* file_path);
	bool initPixel(IRenderDevice&, const wchar_t* file_path);
	bool initDomain(IRenderDevice&, const wchar_t* file_path);
	bool initGeometry(IRenderDevice&, const wchar_t* file_path);
	bool initHull(IRenderDevice&, const wchar_t* file_path);
	bool initCompute(IRenderDevice&, const wchar_t* file_path);
#endif

	bool initVertexSource(IRenderDevice&, const char* source, unsigned int source_size = UINT_FAIL);
	bool initPixelSource(IRenderDevice&, const char* source, unsigned int source_size = UINT_FAIL);
	bool initDomainSource(IRenderDevice&, const char* source, unsigned int source_size = UINT_FAIL);
	bool initGeometrySource(IRenderDevice&, const char* source, unsigned int source_size = UINT_FAIL);
	bool initHullSource(IRenderDevice&, const char* source, unsigned int source_size = UINT_FAIL);
	bool initComputeSource(IRenderDevice&, const char* source, unsigned int source_size = UINT_FAIL);

	void destroy(void);

	bool isD3D(void) const;

	INLINE unsigned int getShader(void) const;

private:
	unsigned int _shader;

#ifdef _UNICODE
	bool loadFileAndCompileShader(unsigned int shader_type, const wchar_t* file_path);
#endif

	bool loadFileAndCompileShader(unsigned int shader_type, const char* file_path);
	bool compileShader(const char* source, int source_size, unsigned int shader_type);

	GLEAM_REF_COUNTED(ShaderGL);
};

NS_END
