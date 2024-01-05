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

#include "Gleam_IShader.h"

NS_GLEAM

class IShaderResourceView;
class ISamplerState;
class IBuffer;

class IProgramBuffers
{
public:
	virtual ~IProgramBuffers(void) {}

	virtual const Vector<IBuffer*>& getConstantBuffers(IShader::Type type) const = 0;
	virtual const IBuffer* getConstantBuffer(IShader::Type type, int32_t index) const = 0;
	virtual IBuffer* getConstantBuffer(IShader::Type type, int32_t index) = 0;
	virtual void addConstantBuffer(IShader::Type type, IBuffer* const_buffer) = 0;
	virtual void removeConstantBuffer(IShader::Type type, int32_t index) = 0;
	virtual void popConstantBuffer(IShader::Type type, int32_t count = 1) = 0;

	virtual int32_t getConstantBufferCount(IShader::Type type) const = 0;
	virtual int32_t getConstantBufferCount(void) const = 0;

	virtual const Vector<IShaderResourceView*>& getResourceViews(IShader::Type type) const = 0;
	virtual const IShaderResourceView* getResourceView(IShader::Type type, int32_t index) const = 0;
	virtual IShaderResourceView* getResourceView(IShader::Type type, int32_t index) = 0;
	virtual void addResourceView(IShader::Type type, IShaderResourceView* resource_view) = 0;
	virtual void removeResourceView(IShader::Type type, int32_t index) = 0;
	virtual void popResourceView(IShader::Type type, int32_t count = 1) = 0;
	virtual void setResourceView(IShader::Type type, int32_t index, IShaderResourceView* resource_view) = 0;

	virtual int32_t getResourceViewCount(IShader::Type type) const = 0;
	virtual int32_t getResourceViewCount(void) const = 0;

	virtual const Vector<ISamplerState*>& getSamplerStates(IShader::Type type) const = 0;
	virtual const ISamplerState* getSamplerState(IShader::Type type, int32_t index) const = 0;
	virtual ISamplerState* getSamplerState(IShader::Type type, int32_t index) = 0;
	virtual void addSamplerState(IShader::Type type, ISamplerState* sampler) = 0;
	virtual void removeSamplerState(IShader::Type type, int32_t index) = 0;
	virtual void popSamplerState(IShader::Type type, int32_t count = 1) = 0;

	virtual int32_t getSamplerCount(IShader::Type type) const = 0;
	virtual int32_t getSamplerCount(void) const = 0;

	virtual IProgramBuffers* clone(void) const = 0;

	virtual void clearResourceViews(void) = 0;
	virtual void clear(void) = 0;

	virtual void bind(IRenderDevice& rd, int32_t res_view_offset = 0, int32_t sampler_offset = 0, int32_t buffer_offset = 0) = 0;

	virtual RendererType getRendererType(void) const = 0;
};

NS_END
