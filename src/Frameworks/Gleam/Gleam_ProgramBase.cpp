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
}

const Vector<IBuffer*>& ProgramBuffersBase::getConstantBuffers(IShader::Type type) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) < (static_cast<int32_t>(IShader::Type::Count) - 1));
	return _constant_buffers[static_cast<int32_t>(type)];
}

const IBuffer* ProgramBuffersBase::getConstantBuffer(IShader::Type type, int32_t index) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) < (static_cast<int32_t>(IShader::Type::Count) - 1) && index < static_cast<int32_t>(_constant_buffers[static_cast<int32_t>(type)].size()));
	return _constant_buffers[static_cast<int32_t>(type)][index];
}

IBuffer* ProgramBuffersBase::getConstantBuffer(IShader::Type type, int32_t index)
{
	GAFF_ASSERT(static_cast<int32_t>(type) < (static_cast<int32_t>(IShader::Type::Count) - 1) && index < static_cast<int32_t>(_constant_buffers[static_cast<int32_t>(type)].size()));
	return _constant_buffers[static_cast<int32_t>(type)][index];
}

void ProgramBuffersBase::addConstantBuffer(IShader::Type type, IBuffer* const_buffer)
{
	GAFF_ASSERT(static_cast<int32_t>(type) < (static_cast<int32_t>(IShader::Type::Count) - 1) && const_buffer && const_buffer->getRendererType() == getRendererType());
	_constant_buffers[static_cast<int32_t>(type)].emplace_back(const_buffer);
}

void ProgramBuffersBase::removeConstantBuffer(IShader::Type type, int32_t index)
{
	GAFF_ASSERT(static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count) && index < static_cast<int32_t>(_constant_buffers[static_cast<int32_t>(type)].size()));
	_constant_buffers[static_cast<int32_t>(type)].erase(_constant_buffers[static_cast<int32_t>(type)].begin() + index);
}

void ProgramBuffersBase::popConstantBuffer(IShader::Type type, int32_t count)
{
	GAFF_ASSERT(static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count) && count <= static_cast<int32_t>(_constant_buffers[static_cast<int32_t>(type)].size()));

	while (count) {
		_constant_buffers[static_cast<int32_t>(type)].pop_back();
		--count;
	}
}

int32_t ProgramBuffersBase::getConstantBufferCount(IShader::Type type) const
{
	return static_cast<int32_t>(_constant_buffers[static_cast<int32_t>(type)].size());
}

int32_t ProgramBuffersBase::getConstantBufferCount(void) const
{
	int32_t count = 0;

	for (int32_t i = 0; i < (static_cast<int32_t>(IShader::Type::Count) - 1); ++i) {
		count += static_cast<int32_t>(_constant_buffers[i].size());
	}

	return count;
}

const Vector<IShaderResourceView*>& ProgramBuffersBase::getResourceViews(IShader::Type type) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) < (static_cast<int32_t>(IShader::Type::Count) - 1));
	return _resource_views[static_cast<int32_t>(type)];
}

const IShaderResourceView* ProgramBuffersBase::getResourceView(IShader::Type type, int32_t index) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count) && index < static_cast<int32_t>(_resource_views[static_cast<int32_t>(type)].size()));
	return _resource_views[static_cast<int32_t>(type)][index];
}

IShaderResourceView* ProgramBuffersBase::getResourceView(IShader::Type type, int32_t index)
{
	GAFF_ASSERT(static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count) && index < static_cast<int32_t>(_resource_views[static_cast<int32_t>(type)].size()));
	return _resource_views[static_cast<int32_t>(type)][index];
}

void ProgramBuffersBase::addResourceView(IShader::Type type, IShaderResourceView* resource_view)
{
	GAFF_ASSERT(static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count) && resource_view && resource_view->getRendererType() == getRendererType());
	_resource_views[static_cast<int32_t>(type)].emplace_back(resource_view);
}

void ProgramBuffersBase::removeResourceView(IShader::Type type, int32_t index)
{
	GAFF_ASSERT(static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count) && index < static_cast<int32_t>(_resource_views[static_cast<int32_t>(type)].size()));
	_resource_views[static_cast<int32_t>(type)].erase(_resource_views[static_cast<int32_t>(type)].begin() + index);
}

void ProgramBuffersBase::popResourceView(IShader::Type type, int32_t count)
{
	GAFF_ASSERT(static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count) && count <= static_cast<int32_t>(_resource_views[static_cast<int32_t>(type)].size()));

	while (count) {
		_resource_views[static_cast<int32_t>(type)].pop_back();
		--count;
	}
}

void ProgramBuffersBase::setResourceView(IShader::Type type, int32_t index, IShaderResourceView* resource_view)
{
	GAFF_ASSERT(static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count) && index <= static_cast<int32_t>(_resource_views[static_cast<int32_t>(type)].size()));
	_resource_views[static_cast<int32_t>(type)][index] = resource_view;
}

int32_t ProgramBuffersBase::getResourceViewCount(IShader::Type type) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count));
	return static_cast<int32_t>(_resource_views[static_cast<int32_t>(type)].size());
}

int32_t ProgramBuffersBase::getResourceViewCount(void) const
{
	int32_t count = 0;

	for (int32_t i = 0; i < (static_cast<int32_t>(IShader::Type::Count) - 1); ++i) {
		count += static_cast<int32_t>(_resource_views[i].size());
	}

	return count;
}

const Vector<ISamplerState*>& ProgramBuffersBase::getSamplerStates(IShader::Type type) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count));
	return _sampler_states[static_cast<int32_t>(type)];
}

const ISamplerState* ProgramBuffersBase::getSamplerState(IShader::Type type, int32_t index) const
{
	GAFF_ASSERT((static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count)) && (index < static_cast<int32_t>(_sampler_states[static_cast<int32_t>(type)].size())));
	return _sampler_states[static_cast<int32_t>(type)][index];
}

ISamplerState* ProgramBuffersBase::getSamplerState(IShader::Type type, int32_t index)
{
	GAFF_ASSERT((static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count)) && (index < static_cast<int32_t>(_sampler_states[static_cast<int32_t>(type)].size())));
	return _sampler_states[static_cast<int32_t>(type)][index];
}

void ProgramBuffersBase::addSamplerState(IShader::Type type, ISamplerState* sampler)
{
	GAFF_ASSERT((static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count)) && sampler && (sampler->getRendererType() == getRendererType()));
	_sampler_states[static_cast<int32_t>(type)].emplace_back(sampler);
}

void ProgramBuffersBase::removeSamplerState(IShader::Type type, int32_t index)
{
	GAFF_ASSERT((static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count)) && (index < static_cast<int32_t>(_sampler_states[static_cast<int32_t>(type)].size())));
	_sampler_states[static_cast<int32_t>(type)].erase(_sampler_states[static_cast<int32_t>(type)].begin() + index);
}

void ProgramBuffersBase::popSamplerState(IShader::Type type, int32_t count)
{
	GAFF_ASSERT((static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count)) && (count <= static_cast<int32_t>(_sampler_states[static_cast<int32_t>(type)].size())));

	while (count) {
		_sampler_states[static_cast<int32_t>(type)].pop_back();
		--count;
	}
}

int32_t ProgramBuffersBase::getSamplerCount(IShader::Type type) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count));
	return static_cast<int32_t>(_sampler_states[static_cast<int32_t>(type)].size());
}

int32_t ProgramBuffersBase::getSamplerCount(void) const
{
	int32_t count = 0;

	for (int32_t i = 0; i < (static_cast<int32_t>(IShader::Type::Count) - 1); ++i) {
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

const IShader* ProgramBase::getAttachedShader(IShader::Type type) const
{
	GAFF_ASSERT((static_cast<int32_t>(type) >= static_cast<int32_t>(IShader::Type::Vertex)) && (static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count)));
	return _attached_shaders[static_cast<int32_t>(type)];
}

IShader* ProgramBase::getAttachedShader(IShader::Type type)
{
	GAFF_ASSERT((static_cast<int32_t>(type) >= static_cast<int32_t>(IShader::Type::Vertex)) && (static_cast<int32_t>(type) < static_cast<int32_t>(IShader::Type::Count)));
	return _attached_shaders[static_cast<int32_t>(type)];
}

NS_END
