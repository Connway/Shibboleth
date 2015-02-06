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

#pragma once

#include "Shibboleth_ModelComponent.h"
#include "Shibboleth_ModelAnimResources.h"

#include <Shibboleth_LoadingMessage.h>

#include <Shibboleth_RenderManager.h>
#include <Shibboleth_IApp.h>

#include <Gleam_IProgram.h>
#include <Gleam_Matrix4x4.h>
#include <Gleam_IBuffer.h>
#include <Gleam_IRenderDevice.h>

#define RELEASE_HOLDING_FLAG 0
#define LOAD_ONLY_HOLDING_FLAG 1

NS_SHIBBOLETH

COMP_REF_DEF_SAVE(ModelComponent, g_Ref_Def);
REF_IMPL_REQ(ModelComponent);

REF_IMPL_SHIB(ModelComponent)
.addBaseClass<ModelComponent>(ModelComponent::g_Hash)
.addBool("Load Only Holding Data", &ModelComponent::GetFlag<LOAD_ONLY_HOLDING_FLAG>, &ModelComponent::SetLoadOnlyHoldingFlag)
.addBool("Release Holding Data", &ModelComponent::GetFlag<RELEASE_HOLDING_FLAG>, &ModelComponent::SetReleaseHoldingFlag)
;

ModelComponent::ModelComponent(IApp& app):
	_render_mgr(app.getManagerT<Shibboleth::RenderManager>("Render Manager")),
	_res_mgr(app.getManagerT<Shibboleth::ResourceManager>("Resource Manager")),
	_app(app), _current_lod(0), _flags(0)
{
}

ModelComponent::~ModelComponent(void)
{
}

bool ModelComponent::validate(Gaff::JSON& json)
{
	Gaff::JSON material_file = json["Material File"];
	Gaff::JSON texture_file = json["Texture File"];
	Gaff::JSON sampler_file = json["Sampler File"];
	Gaff::JSON model_file = json["Model File"];
	bool valid = true;

	if (material_file && !material_file.isString()) {
		// log error
		json.setObject("Material File", Gaff::JSON::createNull());
		valid = false;
	}

	if (texture_file && !texture_file.isString()) {
		// log error
		json.setObject("Texture File", Gaff::JSON::createNull());
		valid = false;
	}

	if (sampler_file && !sampler_file.isString()) {
		// log error
		json.setObject("Sampler File", Gaff::JSON::createNull());
		valid = false;
	}

	if (model_file && !model_file.isString()) {
		// log error
		json.setObject("Model File", Gaff::JSON::createNull());
		valid = false;
	}

	return valid;
}

bool ModelComponent::load(const Gaff::JSON& json)
{
	Gaff::JSON material_file = json["Material File"];
	Gaff::JSON texture_file = json["Texture File"];
	Gaff::JSON sampler_file = json["Sampler File"];
	Gaff::JSON model_file = json["Model File"];

	g_Ref_Def.read(json, this);

	if (material_file.isString()) {
		_material_res = _res_mgr.requestResource(material_file.getString());
	}

	if (texture_file.isString()) {
		_texture_res = _res_mgr.requestResource(texture_file.getString());
	}

	if (sampler_file.isString()) {
		_sampler_res = _res_mgr.requestResource(sampler_file.getString());
	}

	if (model_file.isString()) {
		_model_res = _res_mgr.requestResource(model_file.getString());
	}

	_program_buffers = _render_mgr.createProgramBuffers();

	if (!_program_buffers) {
		return false;
	}

	BufferPtr buffer(_render_mgr.createBuffer());

	if (!buffer->init(
		_render_mgr.getRenderDevice(), nullptr, sizeof(float) * 16 * 3,
		Gleam::IBuffer::SHADER_DATA, 0, Gleam::IBuffer::WRITE)) {

		return false;
	}

	_program_buffers->addConstantBuffer(Gleam::IShader::SHADER_VERTEX, buffer.get());

	if (GetFlag<RELEASE_HOLDING_FLAG>()) {
		_app.getBroadcaster().listen<LoadingMessage>(this, &ModelComponent::HandleLoadingMessage);
	}

	auto callback_func = Gaff::Bind(this, &ModelComponent::LoadCallback);

	_texture_res.getResourcePtr()->addCallback(Gaff::Bind(this, &ModelComponent::TextureLoadedCallback));
	_sampler_res.getResourcePtr()->addCallback(Gaff::Bind(this, &ModelComponent::SamplerStateCallback));
	_material_res.getResourcePtr()->addCallback(callback_func);
	_texture_res.getResourcePtr()->addCallback(callback_func);
	_model_res.getResourcePtr()->addCallback(callback_func);

	return true;
}

void ModelComponent::allComponentsLoaded(void)
{
}

void ModelComponent::TextureLoadedCallback(const AHashString& /*resource*/, bool success)
{
	if (success) {
		_program_buffers->addResourceView(Gleam::IShader::SHADER_PIXEL, _texture_res->resource_views[0].get());
	}
}

void ModelComponent::SamplerStateCallback(const AHashString& /*resource*/, bool success)
{
	if (success) {
		_program_buffers->addSamplerState(Gleam::IShader::SHADER_PIXEL, _sampler_res->data[0].get());
	}
}

void ModelComponent::LoadCallback(const AHashString& /*resource*/, bool success)
{
	if (!success) {
		int i = 5;
		i = i;
		// complain about something
		return;
	}

	// process loaded data
}

void ModelComponent::HandleLoadingMessage(const LoadingMessage& msg)
{
	if (msg.state == LoadingMessage::LOADING_FINISHED) {
		if (_flags & RELEASE_HOLDING_FLAG) {
			_model_res->holding_data.getResourcePtr() = nullptr;
		}
	}
}

void ModelComponent::SetReleaseHoldingFlag(bool value)
{
	if (value) {
		_flags |= (1 << RELEASE_HOLDING_FLAG);
		_flags &= ~(1 << LOAD_ONLY_HOLDING_FLAG); // If we are releasing holding data, then we can't only load holding data
	} else {
		_flags &= ~(1 << RELEASE_HOLDING_FLAG);
	}
}

void ModelComponent::SetLoadOnlyHoldingFlag(bool value)
{
	if (value) {
		_flags |= (1 << LOAD_ONLY_HOLDING_FLAG);
		_flags &= ~(1 << RELEASE_HOLDING_FLAG); // If we are loading only the holding data, then we can't release it
	} else {
		_flags &= ~(1 << LOAD_ONLY_HOLDING_FLAG);
	}
}

void ModelComponent::render(void)
{
	if (!_material_res || !_model_res || !_texture_res || !_sampler_res ||
		!_material_res.getResourcePtr()->isLoaded() ||
		!_model_res.getResourcePtr()->isLoaded() ||
		!_texture_res.getResourcePtr()->isLoaded() ||
		!_sampler_res.getResourcePtr()->isLoaded()) {

		return;
	}

	static float rot = 0.0f;
	rot += 0.01f;

	Gleam::Matrix4x4 tocamera, projection, toworld;
	tocamera.setLookAtLH(0.0f, 5.0f, -5.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	projection.setPerspectiveLH(90.0f, 16.0f / 9.0f, 0.1f, 5000.0f);
	toworld.setIdentity();
	toworld.setRotationY(rot);

	// Update camera data
	Gleam::IBuffer* buffer = _program_buffers->getConstantBuffer(Gleam::IShader::SHADER_VERTEX, 0);

	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();
	rd.getActiveOutputRenderTarget()->bind(rd);

	float* matrix_data = (float*)buffer->map(rd);
		memcpy(matrix_data, toworld.getBuffer(), sizeof(float) * 16);
		memcpy(matrix_data + 16, tocamera.getBuffer(), sizeof(float) * 16);
		memcpy(matrix_data + 32, projection.getBuffer(), sizeof(float) * 16);
	buffer->unmap(_render_mgr.getRenderDevice());

	unsigned int current_device = rd.getCurrentDevice();
	ModelPtr& model = _model_res->models[current_device][_current_lod];

	_material_res->programs[current_device]->bind(rd, _program_buffers.get());

	for (unsigned int i = 0; i < model->getMeshCount(); ++i) {
		model->render(rd, i);
	}
}

NS_END
