/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

NS_GLEAM

class ProgramBuffersBase : public IProgramBuffers
{
public:
	ProgramBuffersBase(void);
	~ProgramBuffersBase(void);

	const Vector<IBuffer*>& getConstantBuffers(IShader::Type type) const override;
	const IBuffer* getConstantBuffer(IShader::Type type, int32_t index) const override;
	IBuffer* getConstantBuffer(IShader::Type type, int32_t index) override;
	void addConstantBuffer(IShader::Type type, IBuffer* const_buffer) override;
	void removeConstantBuffer(IShader::Type type, int32_t index) override;
	void popConstantBuffer(IShader::Type type, int32_t count = 1) override;

	int32_t getConstantBufferCount(IShader::Type type) const override;
	int32_t getConstantBufferCount(void) const override;

	const Vector<IShaderResourceView*>& getResourceViews(IShader::Type type) const override;
	const IShaderResourceView* getResourceView(IShader::Type type, int32_t index) const override;
	IShaderResourceView* getResourceView(IShader::Type type, int32_t index) override;
	void addResourceView(IShader::Type type, IShaderResourceView* resource_view) override;
	void removeResourceView(IShader::Type type, int32_t index) override;
	void popResourceView(IShader::Type type, int32_t count = 1) override;
	void setResourceView(IShader::Type type, int32_t index, IShaderResourceView* resource_view) override;

	int32_t getResourceViewCount(IShader::Type type) const override;
	int32_t getResourceViewCount(void) const override;

	const Vector<ISamplerState*>& getSamplerStates(IShader::Type type) const override;
	const ISamplerState* getSamplerState(IShader::Type type, int32_t index) const override;
	ISamplerState* getSamplerState(IShader::Type type, int32_t index) override;
	void addSamplerState(IShader::Type type, ISamplerState* sampler) override;
	void removeSamplerState(IShader::Type type, int32_t index) override;
	void popSamplerState(IShader::Type type, int32_t count = 1) override;

	int32_t getSamplerCount(IShader::Type type) const override;
	int32_t getSamplerCount(void) const override;

protected:
	Vector<IShaderResourceView*> _resource_views[static_cast<size_t>(IShader::Type::Count)];
	Vector<ISamplerState*> _sampler_states[static_cast<size_t>(IShader::Type::Count)];
	Vector<IBuffer*> _constant_buffers[static_cast<size_t>(IShader::Type::Count)];
};

class ProgramBase : public IProgram
{
public:
	ProgramBase(void);
	~ProgramBase(void);

	const IShader* getAttachedShader(IShader::Type type) const override;
	IShader* getAttachedShader(IShader::Type type) override;

protected:
	IShader* _attached_shaders[static_cast<size_t>(IShader::Type::Count)] = { nullptr };
};

NS_END
