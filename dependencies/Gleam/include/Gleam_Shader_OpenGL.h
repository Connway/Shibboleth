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

#pragma once

#include "Gleam_IShader.h"

NS_GLEAM

class ShaderGL : public IShader
{
public:
	ShaderGL(void);
	~ShaderGL(void);

	INLINE bool initVertex(const IRenderDevice&, const GleamGString& file_path);
	INLINE bool initPixel(const IRenderDevice&, const GleamGString& file_path);
	INLINE bool initDomain(const IRenderDevice&, const GleamGString& file_path);
	INLINE bool initGeometry(const IRenderDevice&, const GleamGString& file_path);
	INLINE bool initHull(const IRenderDevice&, const GleamGString& file_path);
	INLINE bool initCompute(const IRenderDevice&, const GleamGString& file_path);

	INLINE bool initVertex(const IRenderDevice&, const GChar* file_path);
	INLINE bool initPixel(const IRenderDevice&, const GChar* file_path);
	INLINE bool initDomain(const IRenderDevice&, const GChar* file_path);
	INLINE bool initGeometry(const IRenderDevice&, const GChar* file_path);
	INLINE bool initHull(const IRenderDevice&, const GChar* file_path);
	INLINE bool initCompute(const IRenderDevice&, const GChar* file_path);

	bool initVertexSource(const IRenderDevice&, const GleamAString& source);
	bool initPixelSource(const IRenderDevice&, const GleamAString& source);
	bool initDomainSource(const IRenderDevice&, const GleamAString& source);
	bool initGeometrySource(const IRenderDevice&, const GleamAString& source);
	bool initHullSource(const IRenderDevice&, const GleamAString& source);
	bool initComputeSource(const IRenderDevice&, const GleamAString& source);

	bool initVertexSource(const IRenderDevice&, const char* source);
	bool initPixelSource(const IRenderDevice&, const char* source);
	bool initDomainSource(const IRenderDevice&, const char* source);
	bool initGeometrySource(const IRenderDevice&, const char* source);
	bool initHullSource(const IRenderDevice&, const char* source);
	bool initComputeSource(const IRenderDevice&, const char* source);

	void destroy(void);

	INLINE bool isD3D(void) const;

	INLINE unsigned int getShader(void) const;

private:
	unsigned int _shader;

	bool loadFileAndCompileShader(unsigned int shader_type, const GChar* file_path);
	bool compileShader(const char* source, int source_size, unsigned int shader_type);
};

NS_END
