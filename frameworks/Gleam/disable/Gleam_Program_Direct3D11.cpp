/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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
#include <d3dcompiler.h>

NS_GLEAM

typedef void(__stdcall ID3D11DeviceContext::*ShaderSetFunction)(UINT, UINT, ID3D11Buffer* const*);
typedef void(__stdcall ID3D11DeviceContext::*ShaderResourceViewSetFunction)(UINT, UINT, ID3D11ShaderResourceView* const*);
typedef void(__stdcall ID3D11DeviceContext::*SamplerSetFunction)(UINT, UINT, ID3D11SamplerState* const*);

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


// Program Buffers
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

IProgramBuffers* ProgramBuffersD3D::clone(void) const
{
	ProgramBuffersD3D* pb = GAFF_ALLOCT(ProgramBuffersD3D, *GetAllocator());

	if (!pb) {
		PrintfToLog("Failed to clone ProgramBuffersD3D.", LOG_ERROR);
		return nullptr;
	}

	for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		pb->_resource_views[i].resize(_resource_views[i].size());
		pb->_sampler_states[i].resize(_sampler_states[i].size());
		pb->_constant_buffers[i].resize(_constant_buffers[i].size());

		for (size_t j = 0; j < _resource_views[i].size(); ++j) {
			pb->_resource_views[i][j] = _resource_views[i][j];
			pb->_resource_views[i][j]->addRef();
		}

		for (size_t j = 0; j < _sampler_states[i].size(); ++j) {
			pb->_sampler_states[i][j] = _sampler_states[i][j];
			pb->_sampler_states[i][j]->addRef();
		}

		for (size_t j = 0; j < _constant_buffers[i].size(); ++j) {
			pb->_constant_buffers[i][j] = _constant_buffers[i][j];
			pb->_constant_buffers[i][j]->addRef();
		}

		pb->cacheResViews(static_cast<IShader::SHADER_TYPE>(i));
		pb->cacheSamplers(static_cast<IShader::SHADER_TYPE>(i));
		pb->cacheBuffers(static_cast<IShader::SHADER_TYPE>(i));
	}

	return pb;
}

void ProgramBuffersD3D::bind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D);
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();

	for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE; ++i) {
		GleamArray<ID3D11ShaderResourceView*>& res_views = _res_views[i];
		GleamArray<ID3D11SamplerState*>& samplers = _samplers[i];
		GleamArray<ID3D11Buffer*>& buffers = _buffers[i];

		(context->*shader_set[i])(0, static_cast<UINT>(buffers.size()), buffers.getArray());
		(context->*resource_set[i])(0, static_cast<UINT>(res_views.size()), res_views.getArray());
		(context->*sampler_set[i])(0, static_cast<UINT>(samplers.size()), samplers.getArray());
	}
}

RendererType ProgramBuffersD3D::getRendererType(void) const
{
	return RENDERER_DIRECT3D;
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


// Program
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
	GAFF_ASSERT(shader->getRendererType() == RENDERER_DIRECT3D);

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

void ProgramD3D::bind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D);
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();

	context->VSSetShader(_shader_vertex, NULL, 0);
	context->PSSetShader(_shader_pixel, NULL, 0);
	context->DSSetShader(_shader_domain, NULL, 0);
	context->GSSetShader(_shader_geometry, NULL, 0);
	context->HSSetShader(_shader_hull, NULL, 0);
	context->CSSetShader(_shader_compute, NULL, 0);
}

void ProgramD3D::unbind(IRenderDevice& rd)
{
	GAFF_ASSERT(rd.getRendererType() == RENDERER_DIRECT3D);
	IRenderDeviceD3D& rd3d = reinterpret_cast<IRenderDeviceD3D&>(*(reinterpret_cast<char*>(&rd) + sizeof(IRenderDevice)));
	ID3D11DeviceContext* context = rd3d.getActiveDeviceContext();

	context->VSSetShader(NULL, NULL, 0);
	context->PSSetShader(NULL, NULL, 0);
	context->DSSetShader(NULL, NULL, 0);
	context->GSSetShader(NULL, NULL, 0);
	context->HSSetShader(NULL, NULL, 0);
	context->CSSetShader(NULL, NULL, 0);
}

ProgramReflection ProgramD3D::getReflectionData(void) const
{
	ProgramReflection reflection;

	for (size_t i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		if (_attached_shaders[i]) {
			reflection.shader_reflection[i] = getShaderReflectionData(_attached_shaders[i].get(), reflection);
		}
	}

	return reflection;
}

RendererType ProgramD3D::getRendererType(void) const
{
	return RENDERER_DIRECT3D;
}

ShaderReflection ProgramD3D::getShaderReflectionData(const IShader* shader, ProgramReflection& prog_refl) const
{
	ShaderReflection reflection;

	ID3DBlob* blob = reinterpret_cast<const ShaderD3D*>(shader)->getByteCodeBuffer();

	ID3D11ShaderReflection* refl = nullptr;
	HRESULT result = D3DReflect(
		blob->GetBufferPointer(), blob->GetBufferSize(),
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&refl)
	);

	if (FAILED(result)) {
		return reflection;
	}

	D3D11_SHADER_DESC shader_desc;
	result = refl->GetDesc(&shader_desc);

	if (FAILED(result)) {
		refl->Release();
		return reflection;
	}

	reflection.num_constant_buffers = shader_desc.ConstantBuffers;

	for (unsigned int i = 0; i < shader_desc.ConstantBuffers; ++i) {
		ID3D11ShaderReflectionConstantBuffer* cb_refl = refl->GetConstantBufferByIndex(i);

		D3D11_SHADER_BUFFER_DESC cb_desc;
		result = cb_refl->GetDesc(&cb_desc);

		if (FAILED(result)) {
			refl->Release();
			return reflection;
		}

		reflection.const_buff_reflection[i].name = cb_desc.Name;
		reflection.const_buff_reflection[i].size_bytes = cb_desc.Size;
	}

	for (unsigned int i = 0; i < shader_desc.BoundResources; ++i) {
		D3D11_SHADER_INPUT_BIND_DESC res_desc;
		result = refl->GetResourceBindingDesc(i, &res_desc);

		if (FAILED(result)) {
			refl->Release();
			return reflection;
		}

		switch (res_desc.Type) {
			case D3D_SIT_TEXTURE:
				reflection.textures[reflection.num_textures] = res_desc.Name;
				++reflection.num_textures;
				break;

			case D3D_SIT_SAMPLER:
				reflection.samplers[reflection.num_samplers] = res_desc.Name;
				++reflection.num_samplers;
				break;

			case D3D_SIT_STRUCTURED:
				reflection.structured_buffers[reflection.num_structured_buffers] = res_desc.Name;
				++reflection.num_structured_buffers;
				break;
		}
	}

	refl->Release();

	prog_refl.total_structured_buffers += reflection.num_structured_buffers;
	prog_refl.total_textures += reflection.num_textures;
	prog_refl.total_samplers += reflection.num_samplers;
	prog_refl.total_structured_buffers += reflection.num_structured_buffers;

	return reflection;
}

NS_END

#endif