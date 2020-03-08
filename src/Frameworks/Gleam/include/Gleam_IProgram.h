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

#include "Gleam_IShader.h"
#include "Gleam_Vector.h"
#include "Gleam_String.h"

NS_GLEAM

class IShaderResourceView;
class ISamplerState;
class IRenderDevice;
class IBuffer;

class IProgramBuffers
{
public:
	IProgramBuffers(void) {}
	virtual ~IProgramBuffers(void) {}

	virtual const Vector<IBuffer*>& getConstantBuffers(IShader::ShaderType type) const = 0;
	virtual const IBuffer* getConstantBuffer(IShader::ShaderType type, int32_t index) const = 0;
	virtual IBuffer* getConstantBuffer(IShader::ShaderType type, int32_t index) = 0;
	virtual void addConstantBuffer(IShader::ShaderType type, IBuffer* const_buffer) = 0;
	virtual void removeConstantBuffer(IShader::ShaderType type, int32_t index) = 0;
	virtual void popConstantBuffer(IShader::ShaderType type, int32_t count = 1) = 0;

	virtual int32_t getConstantBufferCount(IShader::ShaderType type) const = 0;
	virtual int32_t getConstantBufferCount(void) const = 0;

	virtual const Vector<IShaderResourceView*>& getResourceViews(IShader::ShaderType type) const = 0;
	virtual const IShaderResourceView* getResourceView(IShader::ShaderType type, int32_t index) const = 0;
	virtual IShaderResourceView* getResourceView(IShader::ShaderType type, int32_t index) = 0;
	virtual void addResourceView(IShader::ShaderType type, IShaderResourceView* resource_view) = 0;
	virtual void removeResourceView(IShader::ShaderType type, int32_t index) = 0;
	virtual void popResourceView(IShader::ShaderType type, int32_t count = 1) = 0;
	virtual void setResourceView(IShader::ShaderType type, int32_t index, IShaderResourceView* resource_view) = 0;

	virtual int32_t getResourceViewCount(IShader::ShaderType type) const = 0;
	virtual int32_t getResourceViewCount(void) const = 0;

	virtual const Vector<ISamplerState*>& getSamplerStates(IShader::ShaderType type) const = 0;
	virtual const ISamplerState* getSamplerState(IShader::ShaderType type, int32_t index) const = 0;
	virtual ISamplerState* getSamplerState(IShader::ShaderType type, int32_t index) = 0;
	virtual void addSamplerState(IShader::ShaderType type, ISamplerState* sampler) = 0;
	virtual void removeSamplerState(IShader::ShaderType type, int32_t index) = 0;
	virtual void popSamplerState(IShader::ShaderType type, int32_t count = 1) = 0;

	virtual int32_t getSamplerCount(IShader::ShaderType type) const = 0;
	virtual int32_t getSamplerCount(void) const = 0;

	virtual IProgramBuffers* clone(void) const = 0;

	virtual void clearResourceViews(void) = 0;
	virtual void clear(void) = 0;

	virtual void bind(IRenderDevice& rd) = 0;

	virtual RendererType getRendererType(void) const = 0;
};

class IProgram
{
public:
	IProgram(void) {}
	virtual ~IProgram(void) {}

	virtual void attach(IShader* shader) = 0;
	virtual void detach(IShader::ShaderType shader) = 0;

	virtual void bind(IRenderDevice& rd) = 0;
	virtual void unbind(IRenderDevice& rd) = 0;

	virtual RendererType getRendererType(void) const = 0;

	virtual const IShader* getAttachedShader(IShader::ShaderType type) const = 0;
	virtual IShader* getAttachedShader(IShader::ShaderType type) = 0;
};

NS_END
