/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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
#include "Gleam_RefCounted.h"
#include <Gaff_RefPtr.h>

NS_GLEAM

class ProgramBuffersBase : public IProgramBuffers
{
public:
	ProgramBuffersBase(void);
	~ProgramBuffersBase(void);

	void clear(void) override;

	const Vector<IBuffer*>& getConstantBuffers(IShader::ShaderType type) const override;
	const IBuffer* getConstantBuffer(IShader::ShaderType type, int32_t index) const override;
	IBuffer* getConstantBuffer(IShader::ShaderType type, int32_t index) override;
	void addConstantBuffer(IShader::ShaderType type, IBuffer* const_buffer) override;
	void removeConstantBuffer(IShader::ShaderType type, int32_t index) override;
	void popConstantBuffer(IShader::ShaderType type, int32_t count = 1) override;

	int32_t getConstantBufferCount(IShader::ShaderType type) const override;
	int32_t getConstantBufferCount(void) const override;

	const Vector<IShaderResourceView*>& getResourceViews(IShader::ShaderType type) const override;
	const IShaderResourceView* getResourceView(IShader::ShaderType type, int32_t index) const override;
	IShaderResourceView* getResourceView(IShader::ShaderType type, int32_t index) override;
	void addResourceView(IShader::ShaderType type, IShaderResourceView* resource_view) override;
	void removeResourceView(IShader::ShaderType type, int32_t index) override;
	void popResourceView(IShader::ShaderType type, int32_t count = 1) override;

	int32_t getResourceViewCount(IShader::ShaderType type) const override;
	int32_t getResourceViewCount(void) const override;

	const Vector<ISamplerState*>& getSamplerStates(IShader::ShaderType type) const override;
	const ISamplerState* getSamplerState(IShader::ShaderType type, int32_t index) const override;
	ISamplerState* getSamplerState(IShader::ShaderType type, int32_t index) override;
	void addSamplerState(IShader::ShaderType type, ISamplerState* sampler) override;
	void removeSamplerState(IShader::ShaderType type, int32_t index) override;
	void popSamplerState(IShader::ShaderType type, int32_t count = 1) override;

	int32_t getSamplerCount(IShader::ShaderType type) const override;
	int32_t getSamplerCount(void) const override;

protected:
	Vector<IShaderResourceView*> _resource_views[IShader::SHADER_TYPE_SIZE];
	Vector<ISamplerState*> _sampler_states[IShader::SHADER_TYPE_SIZE];
	Vector<IBuffer*> _constant_buffers[IShader::SHADER_TYPE_SIZE];

	GLEAM_REF_COUNTED(ProgramBuffersBase);
};

class ProgramBase : public IProgram
{
public:
	ProgramBase(void);
	~ProgramBase(void);

	void destroy(void) override;

	const IShader* getAttachedShader(IShader::ShaderType type) const override;
	IShader* getAttachedShader(IShader::ShaderType type) override;

protected:
	Gaff::RefPtr<IShader> _attached_shaders[IShader::SHADER_TYPE_SIZE];

	GLEAM_REF_COUNTED(ProgramBase);
};

NS_END
