/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Gleam_Program_Direct3D.h"
#include "Gleam_ShaderResourceView_Direct3D.h"
#include "Gleam_SamplerState_Direct3D.h"
#include "Gleam_IRenderDevice_Direct3D.h"
#include "Gleam_Shader_Direct3D.h"
#include "Gleam_Buffer_Direct3D.h"
#include "Gleam_IRenderDevice.h"
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

ProgramBuffersD3D::ProgramBuffersD3D(void)
{
}

ProgramBuffersD3D::~ProgramBuffersD3D(void)
{
}

void ProgramBuffersD3D::addConstantBuffer(IShader::SHADER_TYPE type, IBuffer* const_buffer)
{
	ProgramBuffersBase::addConstantBuffer(type, const_buffer);
	cacheBuffers(type);
}

void ProgramBuffersD3D::removeConstantBuffer(IShader::SHADER_TYPE type, size_t index)
{
	ProgramBuffersBase::removeConstantBuffer(type, index);
	cacheBuffers(type);
}

void ProgramBuffersD3D::popConstantBuffer(IShader::SHADER_TYPE type, size_t count)
{
	ProgramBuffersBase::popConstantBuffer(type, count);
	cacheBuffers(type);
}

void ProgramBuffersD3D::addResourceView(IShader::SHADER_TYPE type, IShaderResourceView* resource_view)
{
	ProgramBuffersBase::addResourceView(type, resource_view);
	cacheResViews(type);
}

void ProgramBuffersD3D::removeResourceView(IShader::SHADER_TYPE type, size_t index)
{
	ProgramBuffersBase::removeResourceView(type, index);
	cacheResViews(type);
}

void ProgramBuffersD3D::popResourceView(IShader::SHADER_TYPE type, size_t count)
{
	ProgramBuffersBase::popResourceView(type, count);
	cacheResViews(type);
}

void ProgramBuffersD3D::addSamplerState(IShader::SHADER_TYPE type, ISamplerState* sampler)
{
	ProgramBuffersBase::addSamplerState(type, sampler);
	cacheSamplers(type);
}

void ProgramBuffersD3D::removeSamplerState(IShader::SHADER_TYPE type, size_t index)
{
	ProgramBuffersBase::removeSamplerState(type, index);
	cacheSamplers(type);
}

void ProgramBuffersD3D::popSamplerState(IShader::SHADER_TYPE type, size_t count)
{
	ProgramBuffersBase::popSamplerState(type, count);
	cacheSamplers(type);
}

void ProgramBuffersD3D::cacheResViews(IShader::SHADER_TYPE type)
{
	GleamArray<IShaderResourceView*>& resource_views = _resource_views[type];
	GleamArray<ID3D11ShaderResourceView*>& res_views = _res_views[type];
	res_views.clearNoFree();

	for (size_t i = 0; i < resource_views.size(); ++i) {
		res_views.push(reinterpret_cast<const ShaderResourceViewD3D*>(resource_views[i])->getResourceView());
	}
}

void ProgramBuffersD3D::cacheSamplers(IShader::SHADER_TYPE type)
{
	GleamArray<ISamplerState*>& sampler_states = _sampler_states[type];
	GleamArray<ID3D11SamplerState*>& samplers = _samplers[type];
	samplers.clearNoFree();

	for (size_t i = 0; i < sampler_states.size(); ++i) {
		samplers.push(reinterpret_cast<const SamplerStateD3D*>(sampler_states[i])->getSamplerState());
	}
}

void ProgramBuffersD3D::cacheBuffers(IShader::SHADER_TYPE type)
{
	GleamArray<IBuffer*>& const_bufs = _constant_buffers[type];
	GleamArray<ID3D11Buffer*>& buffers = _buffers[type];
	buffers.clearNoFree();

	for (size_t i = 0; i < const_bufs.size(); ++i) {
		buffers.push(reinterpret_cast<BufferD3D*>(const_bufs[i])->getBuffer());
	}
}

bool ProgramBuffersD3D::isD3D(void) const
{
	return true;
}


typedef void (__stdcall ID3D11DeviceContext::*ShaderSetFunction)(UINT, UINT, ID3D11Buffer* const*);
typedef void (__stdcall ID3D11DeviceContext::*ShaderResourceViewSetFunction)(UINT, UINT, ID3D11ShaderResourceView* const*);
typedef void (__stdcall ID3D11DeviceContext::*SamplerSetFunction)(UINT, UINT, ID3D11SamplerState* const*);

static ShaderSetFunction shader_set[IShader::SHADER_TYPE_SIZE] = {
	&ID3D11DeviceContext::VSSetConstantBuffers,
	&ID3D11DeviceContext::PSSetConstantBuffers,
	&ID3D11DeviceContext::DSSetConstantBuffers,
	&ID3D11DeviceContext::GSSetConstantBuffers,
	&ID3D11DeviceContext::HSSetConstantBuffers,
	&ID3D11DeviceContext::CSSetConstantBuffers
};

static ShaderResourceViewSetFunction resource_set[IShader::SHADER_TYPE_SIZE] = {
	&ID3D11DeviceContext::VSSetShaderResources,
	&ID3D11DeviceContext::PSSetShaderResources,
	&ID3D11DeviceContext::DSSetShaderResources,
	&ID3D11DeviceContext::GSSetShaderResources,
	&ID3D11DeviceContext::HSSetShaderResources,
	&ID3D11DeviceContext::CSSetShaderResources
};

static SamplerSetFunction sampler_set[IShader::SHADER_TYPE_SIZE] = {
	&ID3D11DeviceContext::VSSetSamplers,
	&ID3D11DeviceContext::PSSetSamplers,
	&ID3D11DeviceContext::DSSetSamplers,
	&ID3D11DeviceContext::GSSetSamplers,
	&ID3D11DeviceContext::HSSetSamplers,
	&ID3D11DeviceContext::CSSetSamplers
};

ProgramD3D::ProgramD3D(void):
	_shader_vertex(nullptr), _shader_pixel(nullptr),
	_shader_domain(nullptr), _shader_geometry(nullptr),
	_shader_hull(nullptr), _shader_compute(nullptr)
{
}

ProgramD3D::~ProgramD3D(void)
{
	destroy();
}

bool ProgramD3D::init(void)
{
	return true;
}

void ProgramD3D::attach(IShader* shader)
{
	assert(shader->isD3D());

	_attached_shaders[shader->getType()] = shader;

	switch (shader->getType()) {
		case IShader::SHADER_VERTEX:
			SAFERELEASE(_shader_vertex)
			_shader_vertex = reinterpret_cast<const ShaderD3D*>(shader)->getVertexShader();
			break;

		case IShader::SHADER_PIXEL:
			SAFERELEASE(_shader_pixel)
			_shader_pixel = reinterpret_cast<const ShaderD3D*>(shader)->getPixelShader();
			break;

		case IShader::SHADER_DOMAIN:
			SAFERELEASE(_shader_domain)
			_shader_domain = reinterpret_cast<const ShaderD3D*>(shader)->getDomainShader();
			break;

		case IShader::SHADER_GEOMETRY:
			SAFERELEASE(_shader_geometry)
			_shader_geometry = reinterpret_cast<const ShaderD3D*>(shader)->getGeometryShader();
			break;

		case IShader::SHADER_HULL:
			SAFERELEASE(_shader_hull)
			_shader_hull = reinterpret_cast<const ShaderD3D*>(shader)->getHullShader();
			break;

		case IShader::SHADER_COMPUTE:
			SAFERELEASE(_shader_compute)
			_shader_compute = reinterpret_cast<const ShaderD3D*>(shader)->getComputeShader();
			break;
	}
}

void ProgramD3D::detach(IShader::SHADER_TYPE shader)
{
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
	}
}

void ProgramD3D::bind(IRenderDevice& rd, IProgramBuffers* program_buffers)
{
	assert(rd.isD3D());

	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();

	context->VSSetShader(_shader_vertex, NULL, 0);
	context->PSSetShader(_shader_pixel, NULL, 0);
	context->DSSetShader(_shader_domain, NULL, 0);
	context->GSSetShader(_shader_geometry, NULL, 0);
	context->HSSetShader(_shader_hull, NULL, 0);
	context->CSSetShader(_shader_compute, NULL, 0);

	if (program_buffers) {
		assert(program_buffers->isD3D());
		ProgramBuffersD3D* pb = reinterpret_cast<ProgramBuffersD3D*>(program_buffers);

		for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE; ++i) {
			GleamArray<ID3D11ShaderResourceView*>& res_views = pb->_res_views[i];
			GleamArray<ID3D11SamplerState*>& samplers = pb->_samplers[i];
			GleamArray<ID3D11Buffer*>& buffers = pb->_buffers[i];

			(context->*shader_set[i])(0, static_cast<UINT>(buffers.size()), buffers.getArray());
			(context->*resource_set[i])(0, static_cast<UINT>(res_views.size()), res_views.getArray());
			(context->*sampler_set[i])(0, static_cast<UINT>(samplers.size()), samplers.getArray());
		}
	}
}

void ProgramD3D::unbind(IRenderDevice& rd)
{
	assert(rd.isD3D());
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();

	context->VSSetShader(NULL, NULL, 0);
	context->PSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
	context->GSSetShader(NULL, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->CSSetShader(NULL, NULL, 0);
}

bool ProgramD3D::isD3D(void) const
{
	return true;
}

NS_END

#endif
