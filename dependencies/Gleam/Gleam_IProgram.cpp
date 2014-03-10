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

#include "Gleam_IShaderResourceView.h"
#include "Gleam_ISamplerState.h"
#include "Gleam_IProgram.h"
#include "Gleam_IBuffer.h"

NS_GLEAM

IProgram::IProgram(void)
{
}

IProgram::~IProgram(void)
{
}

void IProgram::destroy(void)
{
	clearResources();
}

void IProgram::clearResources(void)
{
	for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		GleamArray<IShaderResourceView*>& resource_views = _resource_views[i];
		GleamArray<ISamplerState*>& samplers = _sampler_states[i];
		GleamArray<IBuffer*>& const_bufs = _constant_buffers[i];

		for (unsigned int j = 0; j < resource_views.size(); ++j) {
			resource_views[j]->release();
		}

		for (unsigned int j = 0; j < samplers.size(); ++j) {
			samplers[j]->release();
		}

		for (unsigned int j = 0; j < const_bufs.size(); ++j) {
			const_bufs[j]->release();
		}

		resource_views.clear();
		samplers.clear();
		const_bufs.clear();
	}
}

const IBuffer* IProgram::getConstantBuffer(IShader::SHADER_TYPE type, unsigned int index) const
{
	assert(type < IShader::SHADER_TYPE_SIZE - 1 && index < _constant_buffers[type].size());
	return _constant_buffers[type][index];
}

IBuffer* IProgram::getConstantBuffer(IShader::SHADER_TYPE type, unsigned int index)
{
	assert(type < IShader::SHADER_TYPE_SIZE - 1 && index < _constant_buffers[type].size());
	return _constant_buffers[type][index];
}

void IProgram::addConstantBuffer(IShader::SHADER_TYPE type, IBuffer* const_buffer)
{
	assert(type < IShader::SHADER_TYPE_SIZE - 1 && const_buffer);
	_constant_buffers[type].push(const_buffer);
	const_buffer->addRef();
}

void IProgram::removeConstantBuffer(IShader::SHADER_TYPE type, unsigned int index)
{
	assert(type < IShader::SHADER_TYPE_SIZE && index < _constant_buffers[type].size());
	_constant_buffers[type][index]->release();
	_constant_buffers[type].erase(index);
}

void IProgram::popConstantBuffer(IShader::SHADER_TYPE type)
{
	assert(type < IShader::SHADER_TYPE_SIZE && _constant_buffers[type].size());
	_constant_buffers[type].pop();
}

unsigned int IProgram::getConstantBufferCount(IShader::SHADER_TYPE type) const
{
	return _constant_buffers[type].size();
}

unsigned int IProgram::getConstantBufferCount(void) const
{
	unsigned int count = 0;

	for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		count += _constant_buffers[i].size();
	}

	return count;
}

const IShaderResourceView* IProgram::getResourceView(IShader::SHADER_TYPE type, unsigned int index) const
{
	assert(type < IShader::SHADER_TYPE_SIZE && index < _resource_views[type].size());
	return _resource_views[type][index];
}

IShaderResourceView* IProgram::getResourceView(IShader::SHADER_TYPE type, unsigned int index)
{
	assert(type < IShader::SHADER_TYPE_SIZE && index < _resource_views[type].size());
	return _resource_views[type][index];
}

void IProgram::addResourceView(IShader::SHADER_TYPE type, IShaderResourceView* resource_view)
{
	assert(type < IShader::SHADER_TYPE_SIZE && resource_view && resource_view->isD3D() == isD3D());
	_resource_views[type].push(resource_view);
	resource_view->addRef();
}

void IProgram::removeResourceView(IShader::SHADER_TYPE type, unsigned int index)
{
	assert(type < IShader::SHADER_TYPE_SIZE && index < _resource_views[type].size());
	_resource_views[type][index]->release();
	_resource_views[type].erase(index);
}

void IProgram::popResourceView(IShader::SHADER_TYPE type)
{
	assert(type < IShader::SHADER_TYPE_SIZE && _resource_views[type].size());
	_resource_views[type].pop();
}

unsigned int IProgram::getResourceViewCount(IShader::SHADER_TYPE type) const
{
	assert(type < IShader::SHADER_TYPE_SIZE);
	return _resource_views[type].size();
}

unsigned int IProgram::getResourceViewCount(void) const
{
	unsigned int count = 0;

	for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		count += _resource_views[i].size();
	}

	return count;
}

const ISamplerState* IProgram::getSamplerState(IShader::SHADER_TYPE type, unsigned int index) const
{
	assert(type < IShader::SHADER_TYPE_SIZE && index < _sampler_states[type].size());
	return _sampler_states[type][index];
}

ISamplerState* IProgram::getSamplerState(IShader::SHADER_TYPE type, unsigned int index)
{
	assert(type < IShader::SHADER_TYPE_SIZE && index < _sampler_states[type].size());
	return _sampler_states[type][index];
}

void IProgram::addSamplerState(IShader::SHADER_TYPE type, ISamplerState* sampler)
{
	assert(type < IShader::SHADER_TYPE_SIZE && sampler && sampler->isD3D() == isD3D());
	_sampler_states[type].push(sampler);
	sampler->addRef();
}

void IProgram::removeSamplerState(IShader::SHADER_TYPE type, unsigned int index)
{
	assert(type < IShader::SHADER_TYPE_SIZE && index < _sampler_states[type].size());
	_sampler_states[type][index]->release();
	_sampler_states[type].erase(index);
}

void IProgram::popSamplerState(IShader::SHADER_TYPE type)
{
	assert(type < IShader::SHADER_TYPE_SIZE && _sampler_states[type].size());
	_sampler_states[type].pop();
}

unsigned int IProgram::getSamplerCount(IShader::SHADER_TYPE type) const
{
	assert(type < IShader::SHADER_TYPE_SIZE);
	return _sampler_states[type].size();
}

unsigned int IProgram::getSamplerCount(void) const
{
	unsigned int count = 0;

	for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		count += _sampler_states[i].size();
	}

	return count;
}

NS_END
