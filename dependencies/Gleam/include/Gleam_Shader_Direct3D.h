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
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

// Need to research class linkage
class ShaderD3D : public IShader
{
public:
	ShaderD3D(void);
	~ShaderD3D(void);

	bool initSource(IRenderDevice& rd, const char* shader_source, unsigned int source_size, SHADER_TYPE shader_type);
	bool initSource(IRenderDevice& rd, const char* shader_source, SHADER_TYPE shader_type);
	bool init(IRenderDevice& rd, const char* file_path, SHADER_TYPE shader_type);

	bool initVertex(IRenderDevice& rd, const char* file_path);
	bool initPixel(IRenderDevice& rd, const char* file_path);
	bool initDomain(IRenderDevice& rd, const char* file_path);
	bool initGeometry(IRenderDevice& rd, const char* file_path);
	bool initHull(IRenderDevice& rd, const char* file_path);
	bool initCompute(IRenderDevice& rd, const char* file_path);

#ifdef _UNICODE
	bool init(IRenderDevice& rd, const wchar_t* file_path, SHADER_TYPE shader_type);

	bool initVertex(IRenderDevice& rd, const wchar_t* file_path);
	bool initPixel(IRenderDevice& rd, const wchar_t* file_path);
	bool initDomain(IRenderDevice& rd, const wchar_t* file_path);
	bool initGeometry(IRenderDevice& rd, const wchar_t* file_path);
	bool initHull(IRenderDevice& rd, const wchar_t* file_path);
	bool initCompute(IRenderDevice& rd, const wchar_t* file_path);
#endif

	bool initVertexSource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL);
	bool initPixelSource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL);
	bool initDomainSource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL);
	bool initGeometrySource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL);
	bool initHullSource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL);
	bool initComputeSource(IRenderDevice& rd, const char* source, unsigned int source_size = UINT_FAIL);

	void destroy(void);

	bool isD3D(void) const;

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

#ifdef _UNICODE
	bool loadFile(const wchar_t* file_path, char*& shader_src, SIZE_T& shader_size) const;
#endif

	bool loadFile(const char* file_path, char*& shader_src, SIZE_T& shader_size) const;
	ID3DBlob* compileShader(const char* shader, SIZE_T shader_size, /*macro, include,*/ LPCSTR entry_point, LPCSTR target);
};

NS_END
