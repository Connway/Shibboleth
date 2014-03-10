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

#include "Gleam_IShader.h"

NS_GLEAM

class ShaderGL : public IShader
{
public:
	ShaderGL(void);
	~ShaderGL(void);

	INLINE bool initVertex(IRenderDevice&, const GleamGString& file_path);
	INLINE bool initPixel(IRenderDevice&, const GleamGString& file_path);
	INLINE bool initDomain(IRenderDevice&, const GleamGString& file_path);
	INLINE bool initGeometry(IRenderDevice&, const GleamGString& file_path);
	INLINE bool initHull(IRenderDevice&, const GleamGString& file_path);
	INLINE bool initCompute(IRenderDevice&, const GleamGString& file_path);

	INLINE bool initVertex(IRenderDevice&, const GChar* file_path);
	INLINE bool initPixel(IRenderDevice&, const GChar* file_path);
	INLINE bool initDomain(IRenderDevice&, const GChar* file_path);
	INLINE bool initGeometry(IRenderDevice&, const GChar* file_path);
	INLINE bool initHull(IRenderDevice&, const GChar* file_path);
	INLINE bool initCompute(IRenderDevice&, const GChar* file_path);

	bool initVertexSource(IRenderDevice&, const GleamAString& source);
	bool initPixelSource(IRenderDevice&, const GleamAString& source);
	bool initDomainSource(IRenderDevice&, const GleamAString& source);
	bool initGeometrySource(IRenderDevice&, const GleamAString& source);
	bool initHullSource(IRenderDevice&, const GleamAString& source);
	bool initComputeSource(IRenderDevice&, const GleamAString& source);

	bool initVertexSource(IRenderDevice&, const char* source);
	bool initPixelSource(IRenderDevice&, const char* source);
	bool initDomainSource(IRenderDevice&, const char* source);
	bool initGeometrySource(IRenderDevice&, const char* source);
	bool initHullSource(IRenderDevice&, const char* source);
	bool initComputeSource(IRenderDevice&, const char* source);

	void destroy(void);

	INLINE bool isD3D(void) const;

	INLINE unsigned int getShader(void) const;

private:
	unsigned int _shader;

	bool loadFileAndCompileShader(unsigned int shader_type, const GChar* file_path);
	bool compileShader(const char* source, int source_size, unsigned int shader_type);
};

NS_END
