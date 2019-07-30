/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

static ShaderSetFunction g_shader_set[IShader::SHADER_TYPE_SIZE] = {
	&ID3D11DeviceContext::VSSetConstantBuffers,
	&ID3D11DeviceContext::PSSetConstantBuffers,
	&ID3D11DeviceContext::DSSetConstantBuffers,
	&ID3D11DeviceContext::GSSetConstantBuffers,
	&ID3D11DeviceContext::HSSetConstantBuffers,
	&ID3D11DeviceContext::CSSetConstantBuffers
};

static ShaderResourceViewSetFunction g_resource_set[IShader::SHADER_TYPE_SIZE] = {
	&ID3D11DeviceContext::VSSetShaderResources,
	&ID3D11DeviceContext::PSSetShaderResources,
	&ID3D11DeviceContext::DSSetShaderResources,
	&ID3D11DeviceContext::GSSetShaderResources,
	&ID3D11DeviceContext::HSSetShaderResources,
	&ID3D11DeviceContext::CSSetShaderResources
};

static SamplerSetFunction g_sampler_set[IShader::SHADER_TYPE_SIZE] = {
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

void ProgramBuffersD3D11::addConstantBuffer(IShader::ShaderType type, IBuffer* const_buffer)
{
	ProgramBuffersBase::addConstantBuffer(type, const_buffer);
	cacheBuffers(type);
}

void ProgramBuffersD3D11::removeConstantBuffer(IShader::ShaderType type, int32_t index)
{
	ProgramBuffersBase::removeConstantBuffer(type, index);
	cacheBuffers(type);
}

void ProgramBuffersD3D11::popConstantBuffer(IShader::ShaderType type, int32_t count)
{
	ProgramBuffersBase::popConstantBuffer(type, count);
	cacheBuffers(type);
}

void ProgramBuffersD3D11::addResourceView(IShader::ShaderType type, IShaderResourceView* resource_view)
{
	ProgramBuffersBase::addResourceView(type, resource_view);
	cacheResViews(type);
}

void ProgramBuffersD3D11::removeResourceView(IShader::ShaderType type, int32_t index)
{
	ProgramBuffersBase::removeResourceView(type, index);
	cacheResViews(type);
}

void ProgramBuffersD3D11::popResourceView(IShader::ShaderType type, int32_t count)
{
	ProgramBuffersBase::popResourceView(type, count);
	cacheResViews(type);
}

void ProgramBuffersD3D11::addSamplerState(IShader::ShaderType type, ISamplerState* sampler)
{
	ProgramBuffersBase::addSamplerState(type, sampler);
	cacheSamplers(type);
}

void ProgramBuffersD3D11::removeSamplerState(IShader::ShaderType type, int32_t index)
{
	ProgramBuffersBase::removeSamplerState(type, index);
	cacheSamplers(type);
}

void ProgramBuffersD3D11::popSamplerState(IShader::ShaderType type, int32_t count)
{
	ProgramBuffersBase::popSamplerState(type, count);
	cacheSamplers(type);
}

IProgramBuffers* ProgramBuffersD3D11::clone(void) const
{
	ProgramBuffersD3D11* const pb = GAFF_ALLOCT(ProgramBuffersD3D11, *GetAllocator());

	if (!pb) {
		PrintfToLog("Failed to clone ProgramBuffersD3D11.", LOG_ERROR);
		return nullptr;
	}

	for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		pb->_resource_views[i].resize(_resource_views[i].size());
		pb->_sampler_states[i].resize(_sampler_states[i].size());
		pb->_constant_buffers[i].resize(_constant_buffers[i].size());

		for (int32_t j = 0; j < static_cast<int32_t>(_resource_views[i].size()); ++j) {
			pb->_resource_views[i][j] = _resource_views[i][j];
		}

		for (int32_t j = 0; j < static_cast<int32_t>(_sampler_states[i].size()); ++j) {
			pb->_sampler_states[i][j] = _sampler_states[i][j];
		}

		for (int32_t j = 0; j < static_cast<int32_t>(_constant_buffers[i].size()); ++j) {
			pb->_constant_buffers[i][j] = _constant_buffers[i][j];
		}

		pb->cacheResViews(static_cast<IShader::ShaderType>(i));
		pb->cacheSamplers(static_cast<IShader::ShaderType>(i));
		pb->cacheBuffers(static_cast<IShader::ShaderType>(i));
	}

	return pb;
}

void ProgramBuffersD3D11::bind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11);
	RenderDeviceD3D11& rd3d = static_cast<RenderDeviceD3D11&>(rd);
	ID3D11DeviceContext3* const context = rd3d.getDeviceContext();

	for (int32_t i = 0; i < IShader::SHADER_TYPE_SIZE; ++i) {
		Vector<ID3D11ShaderResourceView*>& res_views = _res_views[i];
		Vector<ID3D11SamplerState*>& samplers = _samplers[i];
		Vector<ID3D11Buffer*>& buffers = _buffers[i];

		(context->*g_shader_set[i])(0, static_cast<UINT>(buffers.size()), buffers.data());
		(context->*g_resource_set[i])(0, static_cast<UINT>(res_views.size()), res_views.data());
		(context->*g_sampler_set[i])(0, static_cast<UINT>(samplers.size()), samplers.data());
	}
}

RendererType ProgramBuffersD3D11::getRendererType(void) const
{
	return RendererType::DIRECT3D11;
}

void ProgramBuffersD3D11::cacheResViews(IShader::ShaderType type)
{
	Vector<IShaderResourceView*>& resource_views = _resource_views[type];
	Vector<ID3D11ShaderResourceView*>& res_views = _res_views[type];
	res_views.clear();

	for (int32_t i = 0; i < static_cast<int32_t>(resource_views.size()); ++i) {
		res_views.emplace_back(static_cast<const ShaderResourceViewD3D11*>(resource_views[i])->getResourceView());
	}
}

void ProgramBuffersD3D11::cacheSamplers(IShader::ShaderType type)
{
	Vector<ISamplerState*>& sampler_states = _sampler_states[type];
	Vector<ID3D11SamplerState*>& samplers = _samplers[type];
	samplers.clear();

	for (int32_t i = 0; i < static_cast<int32_t>(sampler_states.size()); ++i) {
		samplers.emplace_back(static_cast<const SamplerStateD3D11*>(sampler_states[i])->getSamplerState());
	}
}

void ProgramBuffersD3D11::cacheBuffers(IShader::ShaderType type)
{
	Vector<IBuffer*>& const_bufs = _constant_buffers[type];
	Vector<ID3D11Buffer*>& buffers = _buffers[type];
	buffers.clear();

	for (int32_t i = 0; i < static_cast<int32_t>(const_bufs.size()); ++i) {
		buffers.emplace_back(static_cast<BufferD3D11*>(const_bufs[i])->getBuffer());
	}
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
	GAFF_ASSERT(shader->getRendererType() == RendererType::DIRECT3D11);
	GAFF_ASSERT(shader->getType() < IShader::SHADER_TYPE_SIZE);

	_attached_shaders[shader->getType()] = shader;

	switch (shader->getType()) {
		case IShader::SHADER_VERTEX:
			SAFERELEASE(_shader_vertex)
			_shader_vertex = static_cast<const ShaderD3D11*>(shader)->getVertexShader();
			break;

		case IShader::SHADER_PIXEL:
			SAFERELEASE(_shader_pixel)
			_shader_pixel = static_cast<const ShaderD3D11*>(shader)->getPixelShader();
			break;

		case IShader::SHADER_DOMAIN:
			SAFERELEASE(_shader_domain)
			_shader_domain = static_cast<const ShaderD3D11*>(shader)->getDomainShader();
			break;

		case IShader::SHADER_GEOMETRY:
			SAFERELEASE(_shader_geometry)
			_shader_geometry = static_cast<const ShaderD3D11*>(shader)->getGeometryShader();
			break;

		case IShader::SHADER_HULL:
			SAFERELEASE(_shader_hull)
			_shader_hull = static_cast<const ShaderD3D11*>(shader)->getHullShader();
			break;

		case IShader::SHADER_COMPUTE:
			SAFERELEASE(_shader_compute)
			_shader_compute = static_cast<const ShaderD3D11*>(shader)->getComputeShader();
			break;

		default:
			break;
	}
}

void ProgramD3D11::detach(IShader::ShaderType shader)
{
	GAFF_ASSERT(shader < IShader::ShaderType::SHADER_TYPE_SIZE);
	_attached_shaders[shader] = nullptr;

	switch (shader) {
		case IShader::SHADER_VERTEX:
			SAFERELEASE(_shader_vertex)
			_shader_vertex = nullptr;
			break;

		case IShader::SHADER_PIXEL:
			SAFERELEASE(_shader_pixel)
			_shader_pixel = nullptr;
			break;

		case IShader::SHADER_DOMAIN:
			SAFERELEASE(_shader_domain)
			_shader_domain = nullptr;
			break;

		case IShader::SHADER_GEOMETRY:
			SAFERELEASE(_shader_geometry)
			_shader_geometry = nullptr;
			break;

		case IShader::SHADER_HULL:
			SAFERELEASE(_shader_hull)
			_shader_hull = nullptr;
			break;

		case IShader::SHADER_COMPUTE:
			SAFERELEASE(_shader_compute)
			_shader_compute = nullptr;
			break;

		default:
			break;
	}
}

void ProgramD3D11::bind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11);
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
	GAFF_ASSERT(rd.getRendererType() == RendererType::DIRECT3D11);
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
	return RendererType::DIRECT3D11;
}

NS_END

#endif
