/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Gleam_IncludeD3D11.h"
#include "Gleam_IShader.h"

NS_GLEAM

class ShaderD3D11 final : public IShader
{
public:
	ShaderD3D11(void);
	~ShaderD3D11(void);

	bool initSource(IRenderDevice& rd, const char* shader_source, size_t source_size, Type shader_type);
	bool initSource(IRenderDevice& rd, const char* shader_source, Type shader_type);
	bool init(IRenderDevice& rd, const char8_t* file_path, Type shader_type);

	bool initVertex(IRenderDevice& rd, const char8_t* file_path);
	bool initPixel(IRenderDevice& rd, const char8_t* file_path);
	bool initDomain(IRenderDevice& rd, const char8_t* file_path);
	bool initGeometry(IRenderDevice& rd, const char8_t* file_path);
	bool initHull(IRenderDevice& rd, const char8_t* file_path);
	bool initCompute(IRenderDevice& rd, const char8_t* file_path);

	bool initVertexSource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL);
	bool initPixelSource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL);
	bool initDomainSource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL);
	bool initGeometrySource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL);
	bool initHullSource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL);
	bool initComputeSource(IRenderDevice& rd, const char* source, size_t source_size = GAFF_SIZE_T_FAIL);

	void destroy(void);

	ShaderReflection getReflectionData(void) const override;

	RendererType getRendererType(void) const override;

	void* getShader(void) const;
	ID3D11VertexShader* getVertexShader(void) const;
	ID3D11PixelShader* getPixelShader(void) const;
	ID3D11DomainShader* getDomainShader(void) const;
	ID3D11GeometryShader* getGeometryShader(void) const;
	ID3D11HullShader* getHullShader(void) const;
	ID3D11ComputeShader* getComputeShader(void) const;

	ID3DBlob* getByteCodeBuffer(void) const;

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
};

NS_END
