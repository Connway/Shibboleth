/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gleam_ProgramBase.h"

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11DomainShader;
struct ID3D11GeometryShader;
struct ID3D11HullShader;
struct ID3D11ComputeShader;

struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
struct ID3D11Buffer;

NS_GLEAM

class ProgramBuffersD3D : public ProgramBuffersBase
{
public:
	ProgramBuffersD3D(void);
	~ProgramBuffersD3D(void);

	void addConstantBuffer(IShader::SHADER_TYPE type, IBuffer* const_buffer) override;
	void removeConstantBuffer(IShader::SHADER_TYPE type, size_t index) override;
	void popConstantBuffer(IShader::SHADER_TYPE type, size_t count = 1) override;

	void addResourceView(IShader::SHADER_TYPE type, IShaderResourceView* resource_view) override;
	void removeResourceView(IShader::SHADER_TYPE type, size_t index) override;
	void popResourceView(IShader::SHADER_TYPE type, size_t count = 1) override;

	void addSamplerState(IShader::SHADER_TYPE type, ISamplerState* sampler) override;
	void removeSamplerState(IShader::SHADER_TYPE type, size_t index) override;
	void popSamplerState(IShader::SHADER_TYPE type, size_t count = 1) override;

	IProgramBuffers* clone(void) const override;

	void bind(IRenderDevice& rd) override;

	RendererType getRendererType(void) const override;

private:
	GleamArray<ID3D11ShaderResourceView*> _res_views[IShader::SHADER_TYPE_SIZE];
	GleamArray<ID3D11SamplerState*> _samplers[IShader::SHADER_TYPE_SIZE];
	GleamArray<ID3D11Buffer*> _buffers[IShader::SHADER_TYPE_SIZE];

	void cacheResViews(IShader::SHADER_TYPE type);
	void cacheSamplers(IShader::SHADER_TYPE type);
	void cacheBuffers(IShader::SHADER_TYPE type);

	friend class ProgramD3D;
};


class ProgramD3D : public ProgramBase
{
public:
	ProgramD3D(void);
	~ProgramD3D(void);

	bool init(void);

	void attach(IShader* shader);
	void detach(IShader::SHADER_TYPE shader);

	void bind(IRenderDevice& rd);
	void unbind(IRenderDevice& rd);

	ProgramReflection getReflectionData(void) const;

	RendererType getRendererType(void) const;

private:
	ID3D11VertexShader* _shader_vertex;
	ID3D11PixelShader* _shader_pixel;
	ID3D11DomainShader* _shader_domain;
	ID3D11GeometryShader* _shader_geometry;
	ID3D11HullShader* _shader_hull;
	ID3D11ComputeShader* _shader_compute;

	ShaderReflection getShaderReflectionData(const IShader* shader, ProgramReflection& prog_refl) const;
};

NS_END
