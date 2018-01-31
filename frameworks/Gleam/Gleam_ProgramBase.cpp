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

#include "Gleam_ProgramBase.h"
#include "Gleam_IShaderResourceView.h"
#include "Gleam_ISamplerState.h"
#include "Gleam_IBuffer.h"
#include <Gaff_Assert.h>

NS_GLEAM

// ProgramBuffers
ProgramBuffersBase::ProgramBuffersBase(void)
{
}

ProgramBuffersBase::~ProgramBuffersBase(void)
{
	clear();
}

void ProgramBuffersBase::clear(void)
{
	for (int32_t i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		Vector<IShaderResourceView*>& resource_views = _resource_views[i];
		Vector<ISamplerState*>& samplers = _sampler_states[i];
		Vector<IBuffer*>& const_bufs = _constant_buffers[i];

		for (int32_t j = 0; j < static_cast<int32_t>(resource_views.size()); ++j) {
			resource_views[j]->release();
		}

		for (int32_t j = 0; j < static_cast<int32_t>(samplers.size()); ++j) {
			samplers[j]->release();
		}

		for (int32_t j = 0; j < static_cast<int32_t>(const_bufs.size()); ++j) {
			const_bufs[j]->release();
		}

		resource_views.clear();
		samplers.clear();
		const_bufs.clear();
	}
}

const Vector<IBuffer*>& ProgramBuffersBase::getConstantBuffers(IShader::ShaderType type) const
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE - 1);
	return _constant_buffers[type];
}

const IBuffer* ProgramBuffersBase::getConstantBuffer(IShader::ShaderType type, int32_t index) const
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE - 1 && index < static_cast<int32_t>(_constant_buffers[type].size()));
	return _constant_buffers[type][index];
}

IBuffer* ProgramBuffersBase::getConstantBuffer(IShader::ShaderType type, int32_t index)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE - 1 && index < static_cast<int32_t>(_constant_buffers[type].size()));
	return _constant_buffers[type][index];
}

void ProgramBuffersBase::addConstantBuffer(IShader::ShaderType type, IBuffer* const_buffer)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE - 1 && const_buffer && const_buffer->getRendererType() == getRendererType());
	_constant_buffers[type].emplace_back(const_buffer);
	const_buffer->addRef();
}

void ProgramBuffersBase::removeConstantBuffer(IShader::ShaderType type, int32_t index)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && index < static_cast<int32_t>(_constant_buffers[type].size()));
	_constant_buffers[type][index]->release();
	_constant_buffers[type].erase(_constant_buffers[type].begin() + index);
}

void ProgramBuffersBase::popConstantBuffer(IShader::ShaderType type, int32_t count)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && count <= static_cast<int32_t>(_constant_buffers[type].size()));

	while (count) {
		_constant_buffers[type].back()->release();
		_constant_buffers[type].pop_back();
		--count;
	}
}

int32_t ProgramBuffersBase::getConstantBufferCount(IShader::ShaderType type) const
{
	return static_cast<int32_t>(_constant_buffers[type].size());
}

int32_t ProgramBuffersBase::getConstantBufferCount(void) const
{
	int32_t count = 0;

	for (int32_t i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		count += static_cast<int32_t>(_constant_buffers[i].size());
	}

	return count;
}

const Vector<IShaderResourceView*>& ProgramBuffersBase::getResourceViews(IShader::ShaderType type) const
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE - 1);
	return _resource_views[type];
}

const IShaderResourceView* ProgramBuffersBase::getResourceView(IShader::ShaderType type, int32_t index) const
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && index < static_cast<int32_t>(_resource_views[type].size()));
	return _resource_views[type][index];
}

IShaderResourceView* ProgramBuffersBase::getResourceView(IShader::ShaderType type, int32_t index)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && index < static_cast<int32_t>(_resource_views[type].size()));
	return _resource_views[type][index];
}

void ProgramBuffersBase::addResourceView(IShader::ShaderType type, IShaderResourceView* resource_view)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && resource_view && resource_view->getRendererType() == getRendererType());
	_resource_views[type].emplace_back(resource_view);
	resource_view->addRef();
}

void ProgramBuffersBase::removeResourceView(IShader::ShaderType type, int32_t index)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && index < static_cast<int32_t>(_resource_views[type].size()));
	_resource_views[type][index]->release();
	_resource_views[type].erase(_resource_views[type].begin() + index);
}

void ProgramBuffersBase::popResourceView(IShader::ShaderType type, int32_t count)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && count <= static_cast<int32_t>(_resource_views[type].size()));

	while (count) {
		_resource_views[type].back()->release();
		_resource_views[type].pop_back();
		--count;
	}
}

int32_t ProgramBuffersBase::getResourceViewCount(IShader::ShaderType type) const
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE);
	return static_cast<int32_t>(_resource_views[type].size());
}

int32_t ProgramBuffersBase::getResourceViewCount(void) const
{
	int32_t count = 0;

	for (int32_t i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		count += static_cast<int32_t>(_resource_views[i].size());
	}

	return count;
}

const Vector<ISamplerState*>& ProgramBuffersBase::getSamplerStates(IShader::ShaderType type) const
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE - 1);
	return _sampler_states[type];
}

const ISamplerState* ProgramBuffersBase::getSamplerState(IShader::ShaderType type, int32_t index) const
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && index < static_cast<int32_t>(_sampler_states[type].size()));
	return _sampler_states[type][index];
}

ISamplerState* ProgramBuffersBase::getSamplerState(IShader::ShaderType type, int32_t index)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && index < static_cast<int32_t>(_sampler_states[type].size()));
	return _sampler_states[type][index];
}

void ProgramBuffersBase::addSamplerState(IShader::ShaderType type, ISamplerState* sampler)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && sampler && sampler->getRendererType() == getRendererType());
	_sampler_states[type].emplace_back(sampler);
	sampler->addRef();
}

void ProgramBuffersBase::removeSamplerState(IShader::ShaderType type, int32_t index)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && index < static_cast<int32_t>(_sampler_states[type].size()));
	_sampler_states[type][index]->release();
	_sampler_states[type].erase(_sampler_states[type].begin() + index);
}

void ProgramBuffersBase::popSamplerState(IShader::ShaderType type, int32_t count)
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE && count <= static_cast<int32_t>(_sampler_states[type].size()));

	while (count) {
		_sampler_states[type].back()->release();
		_sampler_states[type].pop_back();
		--count;
	}
}

int32_t ProgramBuffersBase::getSamplerCount(IShader::ShaderType type) const
{
	GAFF_ASSERT(type < IShader::SHADER_TYPE_SIZE);
	return static_cast<int32_t>(_sampler_states[type].size());
}

int32_t ProgramBuffersBase::getSamplerCount(void) const
{
	int32_t count = 0;

	for (int32_t i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		count += static_cast<int32_t>(_sampler_states[i].size());
	}

	return count;
}



// IProgram
ProgramBase::ProgramBase(void)
{
}

ProgramBase::~ProgramBase(void)
{
}

void ProgramBase::destroy(void)
{
	for (int32_t i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		_attached_shaders[i] = nullptr;
	}
}

const IShader* ProgramBase::getAttachedShader(IShader::ShaderType type) const
{
	GAFF_ASSERT(type >= IShader::SHADER_VERTEX && type < IShader::SHADER_TYPE_SIZE);
	return _attached_shaders[type].get();
}

IShader* ProgramBase::getAttachedShader(IShader::ShaderType type)
{
	GAFF_ASSERT(type >= IShader::SHADER_VERTEX && type < IShader::SHADER_TYPE_SIZE);
	return _attached_shaders[type].get();
}

NS_END
