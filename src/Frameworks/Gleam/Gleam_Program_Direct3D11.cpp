/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#if defined(_WIN32) || defined(_WIN64)

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

static ShaderSetFunction g_buffer_set[static_cast<size_t>(IShader::Type::Count)] = {
	&ID3D11DeviceContext::VSSetConstantBuffers,
	&ID3D11DeviceContext::PSSetConstantBuffers,
	&ID3D11DeviceContext::DSSetConstantBuffers,
	&ID3D11DeviceContext::GSSetConstantBuffers,
	&ID3D11DeviceContext::HSSetConstantBuffers,
	&ID3D11DeviceContext::CSSetConstantBuffers
};

static ShaderResourceViewSetFunction g_resource_set[static_cast<size_t>(IShader::Type::Count)] = {
	&ID3D11DeviceContext::VSSetShaderResources,
	&ID3D11DeviceContext::PSSetShaderResources,
	&ID3D11DeviceContext::DSSetShaderResources,
	&ID3D11DeviceContext::GSSetShaderResources,
	&ID3D11DeviceContext::HSSetShaderResources,
	&ID3D11DeviceContext::CSSetShaderResources
};

static SamplerSetFunction g_sampler_set[static_cast<size_t>(IShader::Type::Count)] = {
	&ID3D11DeviceContext::VSSetSamplers,
	&ID3D11DeviceContext::PSSetSamplers,
	&ID3D11DeviceContext::DSSetSamplers,
	&ID3D11DeviceContext::GSSetSamplers,
	&ID3D11DeviceContext::HSSetSamplers,
	&ID3D11DeviceContext::CSSetSamplers
};


// Program Buffers
ProgramBuffersD3D11::ProgramBuffersD3D11(void)
{
}

ProgramBuffersD3D11::~ProgramBuffersD3D11(void)
{
}

void ProgramBuffersD3D11::addConstantBuffer(IShader::Type type, IBuffer* const_buffer)
{
	ProgramBuffersBase::addConstantBuffer(type, const_buffer);
	_buffers[static_cast<int32_t>(type)].emplace_back(static_cast<const BufferD3D11*>(const_buffer)->getBuffer());
}

void ProgramBuffersD3D11::removeConstantBuffer(IShader::Type type, int32_t index)
{
	ProgramBuffersBase::removeConstantBuffer(type, index);
	_buffers[static_cast<int32_t>(type)].erase(_buffers[static_cast<int32_t>(type)].begin() + index);
}

void ProgramBuffersD3D11::popConstantBuffer(IShader::Type type, int32_t count)
{
	ProgramBuffersBase::popConstantBuffer(type, count);

	while (count) {
		_buffers[static_cast<int32_t>(type)].pop_back();
		--count;
	}
}

void ProgramBuffersD3D11::addResourceView(IShader::Type type, IShaderResourceView* resource_view)
{
	ProgramBuffersBase::addResourceView(type, resource_view);
	_res_views[static_cast<int32_t>(type)].emplace_back(static_cast<const ShaderResourceViewD3D11*>(resource_view)->getResourceView());
}

void ProgramBuffersD3D11::removeResourceView(IShader::Type type, int32_t index)
{
	ProgramBuffersBase::removeResourceView(type, index);
	_res_views[static_cast<int32_t>(type)].erase(_res_views[static_cast<int32_t>(type)].begin() + index);
}

void ProgramBuffersD3D11::popResourceView(IShader::Type type, int32_t count)
{
	ProgramBuffersBase::popResourceView(type, count);

	while (count) {
		_res_views[static_cast<int32_t>(type)].pop_back();
		--count;
	}
}

void ProgramBuffersD3D11::setResourceView(IShader::Type type, int32_t index, IShaderResourceView* resource_view)
{
	ProgramBuffersBase::setResourceView(type, index, resource_view);
	_res_views[static_cast<int32_t>(type)][index] = static_cast<const ShaderResourceViewD3D11*>(resource_view)->getResourceView();
}

void ProgramBuffersD3D11::addSamplerState(IShader::Type type, ISamplerState* sampler)
{
	ProgramBuffersBase::addSamplerState(type, sampler);
	_samplers[static_cast<int32_t>(type)].emplace_back(static_cast<const SamplerStateD3D11*>(sampler)->getSamplerState());
}

void ProgramBuffersD3D11::removeSamplerState(IShader::Type type, int32_t index)
{
	ProgramBuffersBase::removeSamplerState(type, index);
	_samplers[static_cast<int32_t>(type)].erase(_samplers[static_cast<int32_t>(type)].begin() + index);
}

void ProgramBuffersD3D11::popSamplerState(IShader::Type type, int32_t count)
{
	ProgramBuffersBase::popSamplerState(type, count);

	while (count) {
		_samplers[static_cast<int32_t>(type)].pop_back();
		--count;
	}
}

IProgramBuffers* ProgramBuffersD3D11::clone(void) const
{
	ProgramBuffersD3D11* const pb = GAFF_ALLOCT(ProgramBuffersD3D11, *GetAllocator());

	if (!pb) {
		PrintfToLog(u8"Failed to clone ProgramBuffersD3D11.", LogMsgType::Error);
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

void ProgramBuffersD3D11::clearResourceViews(void)
{
	for (int32_t i = 0; i < static_cast<int32_t>(IShader::Type::Count); ++i) {
		_res_views[i].clear();
	}
}

void ProgramBuffersD3D11::clear(void)
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

void ProgramBuffersD3D11::bind(IRenderDevice& rd, int32_t res_view_offset, int32_t sampler_offset, int32_t buffer_offset)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	for (int32_t i = 0; i < static_cast<int32_t>(IShader::Type::Count); ++i) {
		Vector<ID3D11ShaderResourceView*>& res_views = _res_views[i];
		Vector<ID3D11SamplerState*>& samplers = _samplers[i];
		Vector<ID3D11Buffer*>& buffers = _buffers[i];

		(context->*g_resource_set[i])(static_cast<UINT>(res_view_offset), static_cast<UINT>(res_views.size()), res_views.data());
		(context->*g_sampler_set[i])(static_cast<UINT>(sampler_offset), static_cast<UINT>(samplers.size()), samplers.data());
		(context->*g_buffer_set[i])(static_cast<UINT>(buffer_offset), static_cast<UINT>(buffers.size()), buffers.data());
	}
}

RendererType ProgramBuffersD3D11::getRendererType(void) const
{
	return RendererType::Direct3D11;
}



// Program
ProgramD3D11::ProgramD3D11(void):
	_shader_vertex(nullptr), _shader_pixel(nullptr),
	_shader_domain(nullptr), _shader_geometry(nullptr),
	_shader_hull(nullptr), _shader_compute(nullptr)
{
}

ProgramD3D11::~ProgramD3D11(void)
{
}

void ProgramD3D11::attach(IShader* shader)
{
	GAFF_ASSERT(shader->getRendererType() == RendererType::Direct3D11);
	GAFF_ASSERT(static_cast<int32_t>(shader->getType()) < static_cast<int32_t>(IShader::Type::Count));

	_attached_shaders[static_cast<int32_t>(shader->getType())] = shader;

	switch (shader->getType()) {
		case IShader::Type::Vertex:
			SAFERELEASE(_shader_vertex)
			_shader_vertex = static_cast<const ShaderD3D11*>(shader)->getVertexShader();
			break;

		case IShader::Type::Pixel:
			SAFERELEASE(_shader_pixel)
			_shader_pixel = static_cast<const ShaderD3D11*>(shader)->getPixelShader();
			break;

		case IShader::Type::Domain:
			SAFERELEASE(_shader_domain)
			_shader_domain = static_cast<const ShaderD3D11*>(shader)->getDomainShader();
			break;

		case IShader::Type::Geometry:
			SAFERELEASE(_shader_geometry)
			_shader_geometry = static_cast<const ShaderD3D11*>(shader)->getGeometryShader();
			break;

		case IShader::Type::Hull:
			SAFERELEASE(_shader_hull)
			_shader_hull = static_cast<const ShaderD3D11*>(shader)->getHullShader();
			break;

		case IShader::Type::Compute:
			SAFERELEASE(_shader_compute)
			_shader_compute = static_cast<const ShaderD3D11*>(shader)->getComputeShader();
			break;

		default:
			break;
	}
}

void ProgramD3D11::detach(IShader::Type shader)
{
	GAFF_ASSERT(static_cast<int32_t>(shader) < static_cast<int32_t>(IShader::Type::Count));
	_attached_shaders[static_cast<size_t>(shader)] = nullptr;

	switch (shader) {
		case IShader::Type::Vertex:
			SAFERELEASE(_shader_vertex)
			_shader_vertex = nullptr;
			break;

		case IShader::Type::Pixel:
			SAFERELEASE(_shader_pixel)
			_shader_pixel = nullptr;
			break;

		case IShader::Type::Domain:
			SAFERELEASE(_shader_domain)
			_shader_domain = nullptr;
			break;

		case IShader::Type::Geometry:
			SAFERELEASE(_shader_geometry)
			_shader_geometry = nullptr;
			break;

		case IShader::Type::Hull:
			SAFERELEASE(_shader_hull)
			_shader_hull = nullptr;
			break;

		case IShader::Type::Compute:
			SAFERELEASE(_shader_compute)
			_shader_compute = nullptr;
			break;

		default:
			break;
	}
}

void ProgramD3D11::bind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->VSSetShader(_shader_vertex, NULL, 0);
	context->PSSetShader(_shader_pixel, NULL, 0);
	context->DSSetShader(_shader_domain, NULL, 0);
	context->GSSetShader(_shader_geometry, NULL, 0);
	context->HSSetShader(_shader_hull, NULL, 0);
	context->CSSetShader(_shader_compute, NULL, 0);
}

void ProgramD3D11::unbind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::Direct3D11);
	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	context->VSSetShader(NULL, NULL, 0);
	context->PSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
	context->GSSetShader(NULL, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->CSSetShader(NULL, NULL, 0);
}

RendererType ProgramD3D11::getRendererType(void) const
{
	return RendererType::Direct3D11;
}

NS_END

#endif
