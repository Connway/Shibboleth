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

#ifdef GLEAM_USE_D3D11

#include "Direct3D11/Gleam_ProgramBuffers_Direct3D11.h"
#include "Direct3D11/Gleam_ShaderResourceView_Direct3D11.h"
#include "Direct3D11/Gleam_SamplerState_Direct3D11.h"
#include "Direct3D11/Gleam_RenderDevice_Direct3D11.h"
#include "Direct3D11/Gleam_Shader_Direct3D11.h"
#include "Direct3D11/Gleam_Buffer_Direct3D11.h"

NS_GLEAM

using ShaderSetFunction = void (__stdcall ID3D11DeviceContext::*)(UINT, UINT, ID3D11Buffer* const*);
using ShaderResourceViewSetFunction = void (__stdcall ID3D11DeviceContext::*)(UINT, UINT, ID3D11ShaderResourceView* const*);
using SamplerSetFunction = void (__stdcall ID3D11DeviceContext::*)(UINT, UINT, ID3D11SamplerState* const*);

static constexpr ShaderSetFunction s_buffer_set[] = {
	&ID3D11DeviceContext::VSSetConstantBuffers,
	&ID3D11DeviceContext::PSSetConstantBuffers,
	&ID3D11DeviceContext::DSSetConstantBuffers,
	&ID3D11DeviceContext::GSSetConstantBuffers,
	&ID3D11DeviceContext::HSSetConstantBuffers,
	&ID3D11DeviceContext::CSSetConstantBuffers
};

static constexpr ShaderResourceViewSetFunction s_resource_set[] = {
	&ID3D11DeviceContext::VSSetShaderResources,
	&ID3D11DeviceContext::PSSetShaderResources,
	&ID3D11DeviceContext::DSSetShaderResources,
	&ID3D11DeviceContext::GSSetShaderResources,
	&ID3D11DeviceContext::HSSetShaderResources,
	&ID3D11DeviceContext::CSSetShaderResources
};

static constexpr SamplerSetFunction s_sampler_set[] = {
	&ID3D11DeviceContext::VSSetSamplers,
	&ID3D11DeviceContext::PSSetSamplers,
	&ID3D11DeviceContext::DSSetSamplers,
	&ID3D11DeviceContext::GSSetSamplers,
	&ID3D11DeviceContext::HSSetSamplers,
	&ID3D11DeviceContext::CSSetSamplers
};

static_assert(std::size(s_buffer_set) == static_cast<size_t>(IShader::Type::Count), "std::size(s_buffer_set) != IShader::Type::Count");
static_assert(std::size(s_resource_set) == static_cast<size_t>(IShader::Type::Count), "std::size(s_resource_set) != IShader::Type::Count");
static_assert(std::size(s_sampler_set) == static_cast<size_t>(IShader::Type::Count), "std::size(s_sampler_set) != IShader::Type::Count");


const Vector<IBuffer*>& ProgramBuffers::getConstantBuffers(IShader::Type type) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	return _constant_buffers[static_cast<int32_t>(type)];
}

const IBuffer* ProgramBuffers::getConstantBuffer(IShader::Type type, int32_t index) const
{
	return const_cast<ProgramBuffers*>(this)->getConstantBuffer(type, index);
}

IBuffer* ProgramBuffers::getConstantBuffer(IShader::Type type, int32_t index)
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_constant_buffers[static_cast<int32_t>(type)].size()));
	return _constant_buffers[static_cast<int32_t>(type)][index];
}

void ProgramBuffers::addConstantBuffer(IShader::Type type, IBuffer* const_buffer)
{
	GAFF_ASSERT(const_buffer && const_buffer->getRendererType() == RendererType::Direct3D11);
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);

	_constant_buffers[static_cast<int32_t>(type)].emplace_back(const_buffer);
	_d3d_buffers[static_cast<int32_t>(type)].emplace_back(static_cast<const Buffer*>(const_buffer)->getBuffer());

	_constant_buffers[static_cast<int32_t>(type)].shrink_to_fit();
	_d3d_buffers[static_cast<int32_t>(type)].shrink_to_fit();
}

void ProgramBuffers::removeConstantBuffer(IShader::Type type, int32_t index)
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	GAFF_ASSERT(index >= 0 && index <= static_cast<int32_t>(_constant_buffers[static_cast<int32_t>(type)].size()));

	_constant_buffers[static_cast<int32_t>(type)].erase(_constant_buffers[static_cast<int32_t>(type)].begin() + index);
	_d3d_buffers[static_cast<int32_t>(type)].erase(_d3d_buffers[static_cast<int32_t>(type)].begin() + index);

	_constant_buffers[static_cast<int32_t>(type)].shrink_to_fit();
	_d3d_buffers[static_cast<int32_t>(type)].shrink_to_fit();
}

void ProgramBuffers::popConstantBuffer(IShader::Type type, int32_t count)
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	GAFF_ASSERT(count >= 0 && count <= static_cast<int32_t>(_constant_buffers[static_cast<int32_t>(type)].size()));

	while (count) {
		_constant_buffers[static_cast<int32_t>(type)].pop_back();
		_d3d_buffers[static_cast<int32_t>(type)].pop_back();

		--count;
	}

	_constant_buffers[static_cast<int32_t>(type)].shrink_to_fit();
	_d3d_buffers[static_cast<int32_t>(type)].shrink_to_fit();
}

int32_t ProgramBuffers::getConstantBufferCount(IShader::Type type) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	return static_cast<int32_t>(_constant_buffers[static_cast<int32_t>(type)].size());
}

int32_t ProgramBuffers::getConstantBufferCount(void) const
{
	int32_t count = 0;

	for (const auto& buffers : _constant_buffers) {
		count += static_cast<int32_t>(buffers.size());
	}

	return count;
}

const Vector<IShaderResourceView*>& ProgramBuffers::getResourceViews(IShader::Type type) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	return _resource_views[static_cast<int32_t>(type)];
}

const IShaderResourceView* ProgramBuffers::getResourceView(IShader::Type type, int32_t index) const
{
	return const_cast<ProgramBuffers*>(this)->getResourceView(type, index);
}

IShaderResourceView* ProgramBuffers::getResourceView(IShader::Type type, int32_t index)
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_resource_views[static_cast<int32_t>(type)].size()));
	return _resource_views[static_cast<int32_t>(type)][index];
}

void ProgramBuffers::addResourceView(IShader::Type type, IShaderResourceView* resource_view)
{
	GAFF_ASSERT(resource_view && resource_view->getRendererType() == RendererType::Direct3D11);
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);

	_resource_views[static_cast<int32_t>(type)].emplace_back(resource_view);
	_d3d_res_views[static_cast<int32_t>(type)].emplace_back(static_cast<const ShaderResourceView*>(resource_view)->getResourceView());

	_constant_buffers[static_cast<int32_t>(type)].shrink_to_fit();
	_d3d_res_views[static_cast<int32_t>(type)].shrink_to_fit();
}

void ProgramBuffers::removeResourceView(IShader::Type type, int32_t index)
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_resource_views[static_cast<int32_t>(type)].size()));

	_resource_views[static_cast<int32_t>(type)].erase(_resource_views[static_cast<int32_t>(type)].begin() + index);
	_d3d_res_views[static_cast<int32_t>(type)].erase(_d3d_res_views[static_cast<int32_t>(type)].begin() + index);

	_constant_buffers[static_cast<int32_t>(type)].shrink_to_fit();
	_d3d_res_views[static_cast<int32_t>(type)].shrink_to_fit();
}

void ProgramBuffers::popResourceView(IShader::Type type, int32_t count)
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	GAFF_ASSERT(count >= 0 && count <= static_cast<int32_t>(_resource_views[static_cast<int32_t>(type)].size()));

	while (count) {
		_constant_buffers[static_cast<int32_t>(type)].pop_back();
		_d3d_res_views[static_cast<int32_t>(type)].pop_back();

		--count;
	}

	_constant_buffers[static_cast<int32_t>(type)].shrink_to_fit();
	_d3d_res_views[static_cast<int32_t>(type)].shrink_to_fit();
}

void ProgramBuffers::setResourceView(IShader::Type type, int32_t index, IShaderResourceView* resource_view)
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_resource_views[static_cast<int32_t>(type)].size()));

	_resource_views[static_cast<int32_t>(type)][index] = resource_view;
	_d3d_res_views[static_cast<int32_t>(type)][index] = static_cast<const ShaderResourceView*>(resource_view)->getResourceView();
}

int32_t ProgramBuffers::getResourceViewCount(IShader::Type type) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	return static_cast<int32_t>(_resource_views[static_cast<int32_t>(type)].size());
}

int32_t ProgramBuffers::getResourceViewCount(void) const
{
	int32_t count = 0;

	for (const auto& res_views : _resource_views) {
		count += static_cast<int32_t>(res_views.size());
	}

	return count;
}

const Vector<const ISamplerState*>& ProgramBuffers::getSamplerStates(IShader::Type type) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	return _sampler_states[static_cast<int32_t>(type)];
}

const ISamplerState* ProgramBuffers::getSamplerState(IShader::Type type, int32_t index) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_sampler_states[static_cast<int32_t>(type)].size()));
	return _sampler_states[static_cast<int32_t>(type)][index];
}

void ProgramBuffers::addSamplerState(IShader::Type type, const ISamplerState* sampler)
{
	GAFF_ASSERT(sampler && sampler->getRendererType() == RendererType::Direct3D11);
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);

	_sampler_states[static_cast<int32_t>(type)].emplace_back(sampler);
	_d3d_samplers[static_cast<int32_t>(type)].emplace_back(static_cast<const SamplerState*>(sampler)->getSamplerState());

	_sampler_states[static_cast<int32_t>(type)].shrink_to_fit();
	_d3d_samplers[static_cast<int32_t>(type)].shrink_to_fit();
}

void ProgramBuffers::removeSamplerState(IShader::Type type, int32_t index)
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_sampler_states[static_cast<int32_t>(type)].size()));

	_sampler_states[static_cast<int32_t>(type)].erase(_sampler_states[static_cast<int32_t>(type)].begin() + index);
	_d3d_samplers[static_cast<int32_t>(type)].erase(_d3d_samplers[static_cast<int32_t>(type)].begin() + index);

	_sampler_states[static_cast<int32_t>(type)].shrink_to_fit();
	_d3d_samplers[static_cast<int32_t>(type)].shrink_to_fit();
}

void ProgramBuffers::popSamplerState(IShader::Type type, int32_t count)
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	GAFF_ASSERT(count >= 0 && count <= static_cast<int32_t>(_sampler_states[static_cast<int32_t>(type)].size()));

	while (count) {
		_sampler_states[static_cast<int32_t>(type)].pop_back();
		_d3d_samplers[static_cast<int32_t>(type)].pop_back();

		--count;
	}

	_sampler_states[static_cast<int32_t>(type)].shrink_to_fit();
	_d3d_samplers[static_cast<int32_t>(type)].shrink_to_fit();
}

int32_t ProgramBuffers::getSamplerCount(IShader::Type type) const
{
	GAFF_ASSERT(static_cast<int32_t>(type) >= 0 && type < IShader::Type::Count);
	return static_cast<int32_t>(_sampler_states[static_cast<int32_t>(type)].size());
}

int32_t ProgramBuffers::getSamplerCount(void) const
{
	int32_t count = 0;

	for (const auto& samplers : _sampler_states) {
		count += static_cast<int32_t>(samplers.size());
	}

	return count;
}

IProgramBuffers* ProgramBuffers::clone(void) const
{
	ProgramBuffers* const pb = GAFF_ALLOCT(ProgramBuffers, *GetAllocator());

	if (!pb) {
		PrintfToLog(u8"Failed to clone ProgramBuffers.", LogMsgType::Error);
		return nullptr;
	}

	for (int32_t i = 0; i < (static_cast<int32_t>(IShader::Type::Count) - 1); ++i) {
		pb->_resource_views[i] = _resource_views[i];
		pb->_sampler_states[i] = _sampler_states[i];
		pb->_constant_buffers[i] = _constant_buffers[i];
		pb->_d3d_res_views[i] = _d3d_res_views[i];
		pb->_d3d_samplers[i] = _d3d_samplers[i];
		pb->_d3d_buffers[i] = _d3d_buffers[i];
	}

	return pb;
}

void ProgramBuffers::clearResourceViews(void)
{
	for (int32_t i = 0; i < static_cast<int32_t>(IShader::Type::Count); ++i) {
		_d3d_res_views[i].clear();
	}
}

void ProgramBuffers::clear(void)
{
	for (int32_t i = 0; i < static_cast<int32_t>(IShader::Type::Count); ++i) {
		_resource_views[i].clear();
		_sampler_states[i].clear();
		_constant_buffers[i].clear();
		_d3d_res_views[i].clear();
		_d3d_samplers[i].clear();
		_d3d_buffers[i].clear();
	}
}

void ProgramBuffers::bind(IRenderDevice& rd, int32_t res_view_offset, int32_t sampler_offset, int32_t buffer_offset)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	for (int32_t i = 0; i < static_cast<int32_t>(IShader::Type::Count); ++i) {
		Vector<ID3D11ShaderResourceView*>& res_views = _d3d_res_views[i];
		Vector<ID3D11SamplerState*>& samplers = _d3d_samplers[i];
		Vector<ID3D11Buffer*>& buffers = _d3d_buffers[i];

		(context->*s_resource_set[i])(static_cast<UINT>(res_view_offset), static_cast<UINT>(res_views.size()), res_views.data());
		(context->*s_sampler_set[i])(static_cast<UINT>(sampler_offset), static_cast<UINT>(samplers.size()), samplers.data());
		(context->*s_buffer_set[i])(static_cast<UINT>(buffer_offset), static_cast<UINT>(buffers.size()), buffers.data());
	}
}

RendererType ProgramBuffers::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

NS_END

#endif
