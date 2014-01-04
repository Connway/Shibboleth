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
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

// Need to research class linkage
class ShaderD3D : public IShader
{
public:
	ShaderD3D(void);
	~ShaderD3D(void);

	INLINE bool initVertex(const IRenderDevice& rd, const GleamGString& file_path);
	INLINE bool initPixel(const IRenderDevice& rd, const GleamGString& file_path);
	INLINE bool initDomain(const IRenderDevice& rd, const GleamGString& file_path);
	INLINE bool initGeometry(const IRenderDevice& rd, const GleamGString& file_path);
	INLINE bool initHull(const IRenderDevice& rd, const GleamGString& file_path);
	INLINE bool initCompute(const IRenderDevice& rd, const GleamGString& file_path);

	bool initVertex(const IRenderDevice&, const GChar* file_path);
	bool initPixel(const IRenderDevice&, const GChar* file_path);
	bool initDomain(const IRenderDevice&, const GChar* file_path);
	bool initGeometry(const IRenderDevice&, const GChar* file_path);
	bool initHull(const IRenderDevice&, const GChar* file_path);
	bool initCompute(const IRenderDevice&, const GChar* file_path);

	bool initVertexSource(const IRenderDevice& rd, const GleamAString& source);
	bool initPixelSource(const IRenderDevice& rd, const GleamAString& source);
	bool initDomainSource(const IRenderDevice& rd, const GleamAString& source);
	bool initGeometrySource(const IRenderDevice& rd, const GleamAString& source);
	bool initHullSource(const IRenderDevice& rd, const GleamAString& source);
	bool initComputeSource(const IRenderDevice& rd, const GleamAString& source);

	bool initVertexSource(const IRenderDevice& rd, const char* source);
	bool initPixelSource(const IRenderDevice& rd, const char* source);
	bool initDomainSource(const IRenderDevice& rd, const char* source);
	bool initGeometrySource(const IRenderDevice& rd, const char* source);
	bool initHullSource(const IRenderDevice& rd, const char* source);
	bool initComputeSource(const IRenderDevice& rd, const char* source);

	void destroy(void);

	INLINE bool isD3D(void) const;

	INLINE void* getShader(void) const;
	INLINE ID3D11VertexShader* getVertexShader(void) const;
	INLINE ID3D11PixelShader* getPixelShader(void) const;
	INLINE ID3D11DomainShader* getDomainShader(void) const;
	INLINE ID3D11GeometryShader* getGeometryShader(void) const;
	INLINE ID3D11HullShader* getHullShader(void) const;
	INLINE ID3D11ComputeShader* getComputeShader(void) const;

	INLINE ID3DBlob* getByteCodeBuffer(void) const;

private:
	union
	{
		void* _shader;
		ID3D11VertexShader* _shader_vertex;
		ID3D11PixelShader* _shader_pixel;
		ID3D11DomainShader* _shader_domain;
		ID3D11GeometryShader* _shader_geometry;
		ID3D11HullShader* _shader_hull;
		ID3D11ComputeShader* _shader_compute;
	};

	ID3DBlob* _shader_buffer;

	bool loadFile(const GChar* file_path, char*& shader_src, SIZE_T& shader_size) const;
	ID3DBlob* compileShader(const char* shader, SIZE_T shader_size, /*macro, include,*/ LPCSTR entry_point, LPCSTR target);
};

NS_END
