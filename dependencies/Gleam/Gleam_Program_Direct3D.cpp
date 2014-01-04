/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gleam_Program_Direct3D.h"
#include "Gleam_ShaderResourceView_Direct3D.h"
#include "Gleam_SamplerState_Direct3D.h"
#include "Gleam_RenderDevice_Direct3D.h"
#include "Gleam_Shader_Direct3D.h"
#include "Gleam_Buffer_Direct3D.h"
#include "Gleam_IncludeD3D11.h"

NS_GLEAM

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
	_shader_vertex(NULLPTR), _shader_pixel(NULLPTR),
	_shader_domain(NULLPTR), _shader_geometry(NULLPTR),
	_shader_hull(NULLPTR), _shader_compute(NULLPTR)
{
}

ProgramD3D::~ProgramD3D(void)
{
	destroy();
	IProgram::destroy();
}

bool ProgramD3D::init(void)
{
	return true;
}

void ProgramD3D::destroy(void)
{
	SAFERELEASE(_shader_vertex)
	SAFERELEASE(_shader_pixel)
	SAFERELEASE(_shader_domain)
	SAFERELEASE(_shader_geometry)
	SAFERELEASE(_shader_hull)
	SAFERELEASE(_shader_compute)
}

void ProgramD3D::attach(const IShader* shader)
{
	assert(shader->isD3D());

	switch (shader->getType()) {
		case IShader::SHADER_VERTEX:
			SAFERELEASE(_shader_vertex)
			_shader_vertex = ((const ShaderD3D*)shader)->getVertexShader();
			_shader_vertex->AddRef();
			break;

		case IShader::SHADER_PIXEL:
			SAFERELEASE(_shader_pixel)
			_shader_pixel = ((const ShaderD3D*)shader)->getPixelShader();
			_shader_pixel->AddRef();
			break;

		case IShader::SHADER_DOMAIN:
			SAFERELEASE(_shader_domain)
			_shader_domain = ((const ShaderD3D*)shader)->getDomainShader();
			_shader_domain->AddRef();
			break;

		case IShader::SHADER_GEOMETRY:
			SAFERELEASE(_shader_geometry)
			_shader_geometry = ((const ShaderD3D*)shader)->getGeometryShader();
			_shader_geometry->AddRef();
			break;

		case IShader::SHADER_HULL:
			SAFERELEASE(_shader_hull)
			_shader_hull = ((const ShaderD3D*)shader)->getHullShader();
			_shader_hull->AddRef();
			break;

		case IShader::SHADER_COMPUTE:
			SAFERELEASE(_shader_compute)
			_shader_compute = ((const ShaderD3D&)shader).getComputeShader();
			_shader_compute->AddRef();
			break;
	}
}

void ProgramD3D::detach(const IShader* shader)
{
	assert(shader->isD3D());

	switch (shader->getType()) {
		case IShader::SHADER_VERTEX:
			SAFERELEASE(_shader_vertex)
			_shader_vertex = NULLPTR;
			break;

		case IShader::SHADER_PIXEL:
			SAFERELEASE(_shader_pixel)
			_shader_pixel = NULLPTR;
			break;

		case IShader::SHADER_DOMAIN:
			SAFERELEASE(_shader_domain)
			_shader_domain = NULLPTR;
			break;

		case IShader::SHADER_GEOMETRY:
			SAFERELEASE(_shader_geometry)
			_shader_geometry = NULLPTR;
			break;

		case IShader::SHADER_HULL:
			SAFERELEASE(_shader_hull)
			_shader_hull = NULLPTR;
			break;

		case IShader::SHADER_COMPUTE:
			SAFERELEASE(_shader_compute)
			_shader_compute = NULLPTR;
			break;
	}
}

bool ProgramD3D::link(void)
{
	return true;
}

void ProgramD3D::bind(IRenderDevice& rd)
{
	assert(rd.isD3D());

	ID3D11DeviceContext* context = ((const RenderDeviceD3D&)rd).getDeviceContext();

	context->VSSetShader(_shader_vertex, NULL, 0);
	context->PSSetShader(_shader_pixel, NULL, 0);
	context->DSSetShader(_shader_domain, NULL, 0);
	context->GSSetShader(_shader_geometry, NULL, 0);
	context->HSSetShader(_shader_hull, NULL, 0);
	context->CSSetShader(_shader_compute, NULL, 0);

	for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE; ++i) {
		///////////////////////////////////////////////////////////////////////
		// We can save performance here by caching these once ahead of time.
		// These will never change unless we add/remove more at run-time.
		// So, generate these caches in the add/remove calls so we don't have to
		// generate them every frame.
		///////////////////////////////////////////////////////////////////////
		GleamArray(IShaderResourceView*)& resource_views = _resource_views[i];
		GleamArray(ISamplerState*)& sampler_states = _sampler_states[i];
		GleamArray(IBuffer*)& const_bufs = _constant_buffers[i];

		GleamArray(ID3D11ShaderResourceView*) res_views(resource_views.size());
		GleamArray(ID3D11SamplerState*) samplers(sampler_states.size());
		GleamArray(ID3D11Buffer*) buffers(const_bufs.size());

		for (unsigned int j = 0; j < const_bufs.size(); ++j) {
			buffers.push(((BufferD3D*)const_bufs[j])->getBuffer());
		}

		for (unsigned int j = 0; j < resource_views.size(); ++j) {
			res_views.push(((const ShaderResourceViewD3D*)resource_views[j])->getResourceView());
		}

		for (unsigned int j = 0; j < sampler_states.size(); ++j) {
			samplers.push(((const SamplerStateD3D*)sampler_states[j])->getSamplerState());
		}
		///////////////////////////////////////////////////////////////////////

		(context->*shader_set[i])(0, buffers.size(), buffers.getArray());
		(context->*resource_set[i])(0, res_views.size(), res_views.getArray());
		(context->*sampler_set[i])(0, samplers.size(), samplers.getArray());
	}
}

void ProgramD3D::unbind(IRenderDevice& rd)
{
	assert(rd.isD3D());
	ID3D11DeviceContext* context = ((const RenderDeviceD3D&)rd).getDeviceContext();

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
