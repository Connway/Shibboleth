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

#ifdef GLEAM_USE_D3D11

#include "Gleam_Program_Direct3D11.h"
#include "Gleam_ShaderResourceView_Direct3D11.h"
#include "Gleam_SamplerState_Direct3D11.h"
#include "Gleam_RenderDevice_Direct3D11.h"
#include "Gleam_Shader_Direct3D11.h"
#include "Gleam_Buffer_Direct3D11.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"
#include <d3dcompiler.h>

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


// Program Buffers
ProgramBuffers::ProgramBuffers(void)
{
}

ProgramBuffers::~ProgramBuffers(void)
{
}

void ProgramBuffers::addConstantBuffer(IShader::Type type, IBuffer* const_buffer)
{
	ProgramBuffersBase::addConstantBuffer(type, const_buffer);
	_buffers[static_cast<int32_t>(type)].emplace_back(static_cast<const Buffer*>(const_buffer)->getBuffer());
}

void ProgramBuffers::removeConstantBuffer(IShader::Type type, int32_t index)
{
	ProgramBuffersBase::removeConstantBuffer(type, index);
	_buffers[static_cast<int32_t>(type)].erase(_buffers[static_cast<int32_t>(type)].begin() + index);
}

void ProgramBuffers::popConstantBuffer(IShader::Type type, int32_t count)
{
	ProgramBuffersBase::popConstantBuffer(type, count);

	while (count) {
		_buffers[static_cast<int32_t>(type)].pop_back();
		--count;
	}
}

void ProgramBuffers::addResourceView(IShader::Type type, IShaderResourceView* resource_view)
{
	ProgramBuffersBase::addResourceView(type, resource_view);
	_res_views[static_cast<int32_t>(type)].emplace_back(static_cast<const ShaderResourceView*>(resource_view)->getResourceView());
}

void ProgramBuffers::removeResourceView(IShader::Type type, int32_t index)
{
	ProgramBuffersBase::removeResourceView(type, index);
	_res_views[static_cast<int32_t>(type)].erase(_res_views[static_cast<int32_t>(type)].begin() + index);
}

void ProgramBuffers::popResourceView(IShader::Type type, int32_t count)
{
	ProgramBuffersBase::popResourceView(type, count);

	while (count) {
		_res_views[static_cast<int32_t>(type)].pop_back();
		--count;
	}
}

void ProgramBuffers::setResourceView(IShader::Type type, int32_t index, IShaderResourceView* resource_view)
{
	ProgramBuffersBase::setResourceView(type, index, resource_view);
	_res_views[static_cast<int32_t>(type)][index] = static_cast<const ShaderResourceView*>(resource_view)->getResourceView();
}

void ProgramBuffers::addSamplerState(IShader::Type type, ISamplerState* sampler)
{
	ProgramBuffersBase::addSamplerState(type, sampler);
	_samplers[static_cast<int32_t>(type)].emplace_back(static_cast<const SamplerState*>(sampler)->getSamplerState());
}

void ProgramBuffers::removeSamplerState(IShader::Type type, int32_t index)
{
	ProgramBuffersBase::removeSamplerState(type, index);
	_samplers[static_cast<int32_t>(type)].erase(_samplers[static_cast<int32_t>(type)].begin() + index);
}

void ProgramBuffers::popSamplerState(IShader::Type type, int32_t count)
{
	ProgramBuffersBase::popSamplerState(type, count);

	while (count) {
		_samplers[static_cast<int32_t>(type)].pop_back();
		--count;
	}
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
		pb->_res_views[i] = _res_views[i];
		pb->_samplers[i] = _samplers[i];
		pb->_buffers[i] = _buffers[i];
	}

	return pb;
}

void ProgramBuffers::clearResourceViews(void)
{
	for (int32_t i = 0; i < static_cast<int32_t>(IShader::Type::Count); ++i) {
		_res_views[i].clear();
	}
}

void ProgramBuffers::clear(void)
{
	for (int32_t i = 0; i < static_cast<int32_t>(IShader::Type::Count); ++i) {
		_resource_views[i].clear();
		_sampler_states[i].clear();
		_constant_buffers[i].clear();
		_res_views[i].clear();
		_samplers[i].clear();
		_buffers[i].clear();
	}
}

void ProgramBuffers::bind(IRenderDevice& rd, int32_t res_view_offset, int32_t sampler_offset, int32_t buffer_offset)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	for (int32_t i = 0; i < static_cast<int32_t>(IShader::Type::Count); ++i) {
		Vector<ID3D11ShaderResourceView*>& res_views = _res_views[i];
		Vector<ID3D11SamplerState*>& samplers = _samplers[i];
		Vector<ID3D11Buffer*>& buffers = _buffers[i];

		(context->*s_resource_set[i])(static_cast<UINT>(res_view_offset), static_cast<UINT>(res_views.size()), res_views.data());
		(context->*s_sampler_set[i])(static_cast<UINT>(sampler_offset), static_cast<UINT>(samplers.size()), samplers.data());
		(context->*s_buffer_set[i])(static_cast<UINT>(buffer_offset), static_cast<UINT>(buffers.size()), buffers.data());
	}
}

RendererType ProgramBuffers::getRendererType(void) const
{
	return RendererType::Direct3D11;
}



// Program
Program::Program(void):
	_shader_vertex(nullptr), _shader_pixel(nullptr),
	_shader_domain(nullptr), _shader_geometry(nullptr),
	_shader_hull(nullptr), _shader_compute(nullptr)
{
}

Program::~Program(void)
{
}

void Program::attach(IShader* shader)
{
	GAFF_ASSERT(shader->getRendererType() == RendererType::Direct3D11);

	const IShader::Type shader_type = shader->getType();
	GAFF_ASSERT(static_cast<int32_t>(shader->getType()) >= 0 && shader_type < IShader::Type::Count);

	_attached_shaders[static_cast<int32_t>(shader_type)] = shader;

	switch (shader_type) {
		case IShader::Type::Vertex:
			GAFF_COM_SAFE_RELEASE(_shader_vertex)
			_shader_vertex = static_cast<const Shader*>(shader)->getVertexShader();
			break;

		case IShader::Type::Pixel:
			GAFF_COM_SAFE_RELEASE(_shader_pixel)
			_shader_pixel = static_cast<const Shader*>(shader)->getPixelShader();
			break;

		case IShader::Type::Domain:
			GAFF_COM_SAFE_RELEASE(_shader_domain)
			_shader_domain = static_cast<const Shader*>(shader)->getDomainShader();
			break;

		case IShader::Type::Geometry:
			GAFF_COM_SAFE_RELEASE(_shader_geometry)
			_shader_geometry = static_cast<const Shader*>(shader)->getGeometryShader();
			break;

		case IShader::Type::Hull:
			GAFF_COM_SAFE_RELEASE(_shader_hull)
			_shader_hull = static_cast<const Shader*>(shader)->getHullShader();
			break;

		case IShader::Type::Compute:
			GAFF_COM_SAFE_RELEASE(_shader_compute)
			_shader_compute = static_cast<const Shader*>(shader)->getComputeShader();
			break;

		default:
			break;
	}
}

void Program::detach(IShader::Type shader)
{
	GAFF_ASSERT(static_cast<int32_t>(shader) >= 0 && shader < IShader::Type::Count);
	_attached_shaders[static_cast<size_t>(shader)] = nullptr;

	switch (shader) {
		case IShader::Type::Vertex:
			GAFF_COM_SAFE_RELEASE(_shader_vertex)
			_shader_vertex = nullptr;
			break;

		case IShader::Type::Pixel:
			GAFF_COM_SAFE_RELEASE(_shader_pixel)
			_shader_pixel = nullptr;
			break;

		case IShader::Type::Domain:
			GAFF_COM_SAFE_RELEASE(_shader_domain)
			_shader_domain = nullptr;
			break;

		case IShader::Type::Geometry:
			GAFF_COM_SAFE_RELEASE(_shader_geometry)
			_shader_geometry = nullptr;
			break;

		case IShader::Type::Hull:
			GAFF_COM_SAFE_RELEASE(_shader_hull)
			_shader_hull = nullptr;
			break;

		case IShader::Type::Compute:
			GAFF_COM_SAFE_RELEASE(_shader_compute)
			_shader_compute = nullptr;
			break;

		default:
			break;
	}
}

void Program::bind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->VSSetShader(_shader_vertex, NULL, 0);
	context->PSSetShader(_shader_pixel, NULL, 0);
	context->DSSetShader(_shader_domain, NULL, 0);
	context->GSSetShader(_shader_geometry, NULL, 0);
	context->HSSetShader(_shader_hull, NULL, 0);
	context->CSSetShader(_shader_compute, NULL, 0);
}

void Program::unbind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDevice& rd3d = static_cast<RenderDevice&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->VSSetShader(NULL, NULL, 0);
	context->PSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
	context->GSSetShader(NULL, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->CSSetShader(NULL, NULL, 0);
}

RendererType Program::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

NS_END

#endif
