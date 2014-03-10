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

#include "Gleam_IProgram.h"

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

class ProgramD3D : public IProgram
{
public:
	ProgramD3D(void);
	~ProgramD3D(void);

	INLINE bool init(void);
	void destroy(void);

	void attach(const IShader* shader);
	void detach(const IShader* shader);
	bool link(void);

	void bind(IRenderDevice& rd);
	void unbind(IRenderDevice& rd);

	bool isD3D(void) const;

	virtual void addConstantBuffer(IShader::SHADER_TYPE type, IBuffer* const_buffer);
	virtual void removeConstantBuffer(IShader::SHADER_TYPE type, unsigned int index);
	virtual void popConstantBuffer(IShader::SHADER_TYPE type);

	virtual void addResourceView(IShader::SHADER_TYPE type, IShaderResourceView* resource_view);
	virtual void removeResourceView(IShader::SHADER_TYPE type, unsigned int index);
	virtual void popResourceView(IShader::SHADER_TYPE type);

	virtual void addSamplerState(IShader::SHADER_TYPE type, ISamplerState* sampler);
	virtual void removeSamplerState(IShader::SHADER_TYPE type, unsigned int index);
	virtual void popSamplerState(IShader::SHADER_TYPE type);

private:
	ID3D11VertexShader* _shader_vertex;
	ID3D11PixelShader* _shader_pixel;
	ID3D11DomainShader* _shader_domain;
	ID3D11GeometryShader* _shader_geometry;
	ID3D11HullShader* _shader_hull;
	ID3D11ComputeShader* _shader_compute;

	GleamArray < GleamArray<ID3D11ShaderResourceView*> > _res_views;
	GleamArray < GleamArray<ID3D11SamplerState*> > _samplers;
	GleamArray < GleamArray<ID3D11Buffer*> > _buffers;

	void cacheResViews(void);
	void cacheSamplers(void);
	void cacheBuffers(void);
};

NS_END
