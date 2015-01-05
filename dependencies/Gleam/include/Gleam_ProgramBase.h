/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

NS_GLEAM

class ProgramBuffersBase : public IProgramBuffers
{
public:
	ProgramBuffersBase(void);
	~ProgramBuffersBase(void);

	void clear(void);

	const GleamArray<IBuffer*>& getConstantBuffers(IShader::SHADER_TYPE type) const;
	const IBuffer* getConstantBuffer(IShader::SHADER_TYPE type, unsigned int index) const;
	IBuffer* getConstantBuffer(IShader::SHADER_TYPE type, unsigned int index);
	void addConstantBuffer(IShader::SHADER_TYPE type, IBuffer* const_buffer);
	void removeConstantBuffer(IShader::SHADER_TYPE type, unsigned int index);
	void popConstantBuffer(IShader::SHADER_TYPE type);

	unsigned int getConstantBufferCount(IShader::SHADER_TYPE type) const;
	unsigned int getConstantBufferCount(void) const;

	const GleamArray<IShaderResourceView*>& getResourceViews(IShader::SHADER_TYPE type) const;
	const IShaderResourceView* getResourceView(IShader::SHADER_TYPE type, unsigned int index) const;
	IShaderResourceView* getResourceView(IShader::SHADER_TYPE type, unsigned int index);
	void addResourceView(IShader::SHADER_TYPE type, IShaderResourceView* resource_view);
	void removeResourceView(IShader::SHADER_TYPE type, unsigned int index);
	void popResourceView(IShader::SHADER_TYPE type);

	unsigned int getResourceViewCount(IShader::SHADER_TYPE type) const;
	unsigned int getResourceViewCount(void) const;

	const GleamArray<ISamplerState*>& getSamplerStates(IShader::SHADER_TYPE type) const;
	const ISamplerState* getSamplerState(IShader::SHADER_TYPE type, unsigned int index) const;
	ISamplerState* getSamplerState(IShader::SHADER_TYPE type, unsigned int index);
	void addSamplerState(IShader::SHADER_TYPE type, ISamplerState* sampler);
	void removeSamplerState(IShader::SHADER_TYPE type, unsigned int index);
	void popSamplerState(IShader::SHADER_TYPE type);

	unsigned int getSamplerCount(IShader::SHADER_TYPE type) const;
	unsigned int getSamplerCount(void) const;

protected:
	GleamArray<IShaderResourceView*> _resource_views[IShader::SHADER_TYPE_SIZE];
	GleamArray<ISamplerState*> _sampler_states[IShader::SHADER_TYPE_SIZE];
	GleamArray<IBuffer*> _constant_buffers[IShader::SHADER_TYPE_SIZE];

	GLEAM_REF_COUNTED(ProgramBuffersBase);
};

class ProgramBase : public IProgram
{
public:
	ProgramBase(void);
	~ProgramBase(void);

	void destroy(void);

	const IShader* getAttachedShader(IShader::SHADER_TYPE type) const;
	IShader* getAttachedShader(IShader::SHADER_TYPE type);

protected:
	Gaff::RefPtr<IShader> _attached_shaders[IShader::SHADER_TYPE_SIZE];

	GLEAM_REF_COUNTED(ProgramBase);
};

NS_END
