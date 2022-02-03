/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

class ProgramBuffersD3D11 : public ProgramBuffersBase
{
public:
	ProgramBuffersD3D11(void);
	~ProgramBuffersD3D11(void);

	void addConstantBuffer(IShader::Type type, IBuffer* const_buffer) override;
	void removeConstantBuffer(IShader::Type type, int32_t index) override;
	void popConstantBuffer(IShader::Type type, int32_t count = 1) override;

	void addResourceView(IShader::Type type, IShaderResourceView* resource_view) override;
	void removeResourceView(IShader::Type type, int32_t index) override;
	void popResourceView(IShader::Type type, int32_t count = 1) override;
	void setResourceView(IShader::Type type, int32_t index, IShaderResourceView* resource_view) override;

	void addSamplerState(IShader::Type type, ISamplerState* sampler) override;
	void removeSamplerState(IShader::Type type, int32_t index) override;
	void popSamplerState(IShader::Type type, int32_t count = 1) override;

	IProgramBuffers* clone(void) const override;

	void clearResourceViews(void) override;
	void clear(void) override;

	void bind(IRenderDevice& rd, int32_t res_view_offset = 0, int32_t sampler_offset = 0, int32_t buffer_offset = 0) override;

	RendererType getRendererType(void) const override;

private:
	Vector<ID3D11ShaderResourceView*> _res_views[static_cast<size_t>(IShader::Type::Count)];
	Vector<ID3D11SamplerState*> _samplers[static_cast<size_t>(IShader::Type::Count)];
	Vector<ID3D11Buffer*> _buffers[static_cast<size_t>(IShader::Type::Count)];

	friend class ProgramD3D11;
};


class ProgramD3D11 : public ProgramBase
{
public:
	ProgramD3D11(void);
	~ProgramD3D11(void);

	void attach(IShader* shader);
	void detach(IShader::Type shader);

	void bind(IRenderDevice& rd);
	void unbind(IRenderDevice& rd);

	RendererType getRendererType(void) const;

private:
	ID3D11VertexShader* _shader_vertex;
	ID3D11PixelShader* _shader_pixel;
	ID3D11DomainShader* _shader_domain;
	ID3D11GeometryShader* _shader_geometry;
	ID3D11HullShader* _shader_hull;
	ID3D11ComputeShader* _shader_compute;
};

NS_END
