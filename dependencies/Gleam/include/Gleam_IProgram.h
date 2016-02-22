/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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
#include "Gleam_Array.h"

#define MAX_SHADER_VAR 16
#define MAX_SHADER_VAR_NAME 256

NS_GLEAM

class IShaderResourceView;
class ISamplerState;
class IRenderDevice;
class IBuffer;

class IProgramBuffers : public Gaff::IRefCounted
{
public:
	IProgramBuffers(void) {}
	virtual ~IProgramBuffers(void) {}

	virtual void clear(void) = 0;

	virtual const GleamArray<IBuffer*>& getConstantBuffers(IShader::SHADER_TYPE type) const = 0;
	virtual const IBuffer* getConstantBuffer(IShader::SHADER_TYPE type, size_t index) const = 0;
	virtual IBuffer* getConstantBuffer(IShader::SHADER_TYPE type, size_t index) = 0;
	virtual void addConstantBuffer(IShader::SHADER_TYPE type, IBuffer* const_buffer) = 0;
	virtual void removeConstantBuffer(IShader::SHADER_TYPE type, size_t index) = 0;
	virtual void popConstantBuffer(IShader::SHADER_TYPE type, size_t count = 1) = 0;

	virtual size_t getConstantBufferCount(IShader::SHADER_TYPE type) const = 0;
	virtual size_t getConstantBufferCount(void) const = 0;

	virtual const GleamArray<IShaderResourceView*>& getResourceViews(IShader::SHADER_TYPE type) const = 0;
	virtual const IShaderResourceView* getResourceView(IShader::SHADER_TYPE type, size_t index) const = 0;
	virtual IShaderResourceView* getResourceView(IShader::SHADER_TYPE type, size_t index) = 0;
	virtual void addResourceView(IShader::SHADER_TYPE type, IShaderResourceView* resource_view) = 0;
	virtual void removeResourceView(IShader::SHADER_TYPE type, size_t index) = 0;
	virtual void popResourceView(IShader::SHADER_TYPE type, size_t count = 1) = 0;

	virtual size_t getResourceViewCount(IShader::SHADER_TYPE type) const = 0;
	virtual size_t getResourceViewCount(void) const = 0;

	virtual const GleamArray<ISamplerState*>& getSamplerStates(IShader::SHADER_TYPE type) const = 0;
	virtual const ISamplerState* getSamplerState(IShader::SHADER_TYPE type, size_t index) const = 0;
	virtual ISamplerState* getSamplerState(IShader::SHADER_TYPE type, size_t index) = 0;
	virtual void addSamplerState(IShader::SHADER_TYPE type, ISamplerState* sampler) = 0;
	virtual void removeSamplerState(IShader::SHADER_TYPE type, size_t index) = 0;
	virtual void popSamplerState(IShader::SHADER_TYPE type, size_t count = 1) = 0;

	virtual size_t getSamplerCount(IShader::SHADER_TYPE type) const = 0;
	virtual size_t getSamplerCount(void) const = 0;

	virtual IProgramBuffers* clone(void) const = 0;

	virtual void bind(IRenderDevice& rd) = 0;

	virtual RendererType getRendererType(void) const = 0;
};

struct ConstBufferReflection
{
	char name[MAX_SHADER_VAR_NAME];
	size_t size_bytes;
};

struct ShaderReflection
{
	ConstBufferReflection const_buff_reflection[MAX_SHADER_VAR];
	char samplers[MAX_SHADER_VAR][MAX_SHADER_VAR_NAME];
	char srvs[MAX_SHADER_VAR][MAX_SHADER_VAR_NAME];

	size_t num_cbs;
	size_t num_samplers;
	size_t num_srvs;
};

struct ProgramReflection
{
	ShaderReflection shader_reflection[IShader::SHADER_TYPE_SIZE - 1];
};

class IProgram : public Gaff::IRefCounted
{
public:
	IProgram(void) {}
	virtual ~IProgram(void) {}

	virtual bool init(void) = 0;
	virtual void destroy(void) = 0;

	virtual void attach(IShader* shader) = 0;
	virtual void detach(IShader::SHADER_TYPE shader) = 0;

	virtual void bind(IRenderDevice& rd) = 0;
	virtual void unbind(IRenderDevice& rd) = 0;

	virtual ProgramReflection getReflectionData(void) const = 0;

	virtual RendererType getRendererType(void) const = 0;

	virtual const IShader* getAttachedShader(IShader::SHADER_TYPE type) const = 0;
	virtual IShader* getAttachedShader(IShader::SHADER_TYPE type) = 0;
};

NS_END
