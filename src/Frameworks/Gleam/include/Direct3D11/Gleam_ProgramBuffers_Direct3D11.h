/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Gleam_IProgramBuffers.h"

struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
struct ID3D11Buffer;

NS_GLEAM

class ProgramBuffers final : public IProgramBuffers
{
public:
	const Vector<IBuffer*>& getConstantBuffers(IShader::Type type) const override;
	const IBuffer* getConstantBuffer(IShader::Type type, int32_t index) const override;
	IBuffer* getConstantBuffer(IShader::Type type, int32_t index) override;
	void addConstantBuffer(IShader::Type type, IBuffer& const_buffer) override;
	void removeConstantBuffer(IShader::Type type, int32_t index) override;
	void popConstantBuffer(IShader::Type type, int32_t count = 1) override;

	int32_t getConstantBufferCount(IShader::Type type) const override;
	int32_t getConstantBufferCount(void) const override;

	const Vector<IShaderResourceView*>& getResourceViews(IShader::Type type) const override;
	const IShaderResourceView* getResourceView(IShader::Type type, int32_t index) const override;
	IShaderResourceView* getResourceView(IShader::Type type, int32_t index) override;
	void addResourceView(IShader::Type type, IShaderResourceView& resource_view) override;
	void removeResourceView(IShader::Type type, int32_t index) override;
	void popResourceView(IShader::Type type, int32_t count = 1) override;
	void setResourceView(IShader::Type type, int32_t index, IShaderResourceView& resource_view) override;

	int32_t getResourceViewCount(IShader::Type type) const override;
	int32_t getResourceViewCount(void) const override;

	const Vector<const ISamplerState*>& getSamplerStates(IShader::Type type) const override;
	const ISamplerState* getSamplerState(IShader::Type type, int32_t index) const override;
	void addSamplerState(IShader::Type type, const ISamplerState& sampler) override;
	void removeSamplerState(IShader::Type type, int32_t index) override;
	void popSamplerState(IShader::Type type, int32_t count = 1) override;

	int32_t getSamplerCount(IShader::Type type) const override;
	int32_t getSamplerCount(void) const override;

	IProgramBuffers* clone(void) const override;

	void clearResourceViews(void) override;
	void clear(void) override;

	void bind(IRenderDevice& rd, int32_t res_view_offset = 0, int32_t sampler_offset = 0, int32_t buffer_offset = 0) override;

	RendererType getRendererType(void) const override;

private:
	Vector<IShaderResourceView*> _resource_views[static_cast<size_t>(IShader::Type::Count)];
	Vector<const ISamplerState*> _sampler_states[static_cast<size_t>(IShader::Type::Count)];
	Vector<IBuffer*> _constant_buffers[static_cast<size_t>(IShader::Type::Count)];

	Vector<ID3D11ShaderResourceView*> _d3d_res_views[static_cast<size_t>(IShader::Type::Count)];
	Vector<ID3D11SamplerState*> _d3d_samplers[static_cast<size_t>(IShader::Type::Count)];
	Vector<ID3D11Buffer*> _d3d_buffers[static_cast<size_t>(IShader::Type::Count)];
};

NS_END
