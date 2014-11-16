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

#pragma once

#include "Shibboleth_ModelComponent.h"
#include "Shibboleth_ModelAnimResources.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_LoadingMessage.h>

#include "Shibboleth_RenderManager.h"
#include <Gleam_IProgram.h>
#include <Gleam_Matrix4x4.h>
#include <Gleam_IBuffer.h>
#include <Gleam_IRenderDevice.h>

#define RELEASE_HOLDING_FLAG 0
#define LOAD_ONLY_HOLDING_FLAG 1

NS_SHIBBOLETH

COMP_REF_DEF_SAVE(ModelComponent, g_Ref_Def);
REF_IMPL_REQ(ModelComponent);

REF_IMPL_ASSIGN_SHIB(ModelComponent)
.addBaseClass<ModelComponent>(ModelComponent::g_Hash)
.addString("Material File", &ModelComponent::_material_filename)
.addString("Model File", &ModelComponent::_model_filename)
.addBool("Load Only Holding Data", &ModelComponent::GetFlag<LOAD_ONLY_HOLDING_FLAG>, &ModelComponent::SetLoadOnlyHoldingFlag)
.addBool("Release Holding Data", &ModelComponent::GetFlag<RELEASE_HOLDING_FLAG>, &ModelComponent::SetReleaseHoldingFlag)
;

ModelComponent::ModelComponent(IApp& app):
	_render_mgr(app.getManagerT<Shibboleth::RenderManager>("Render Manager")),
	_res_mgr(app.getManagerT<Shibboleth::ResourceManager>("Resource Manager")),
	_app(app), _flags(0)
{
}

ModelComponent::~ModelComponent(void)
{
}

bool ModelComponent::load(const Gaff::JSON& json)
{
	g_Ref_Def.read(json, this);

	_material_res = _res_mgr.requestResource(_material_filename.getBuffer());
	_model_res = _res_mgr.requestResource(_model_filename.getBuffer());

	auto callback_func = Gaff::Bind(this, &ModelComponent::LoadCallback);

	_material_res.getResourcePtr()->addCallback(callback_func);
	_model_res.getResourcePtr()->addCallback(callback_func);

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

	return true;
}

void ModelComponent::allComponentsLoaded(void)
{
}

void ModelComponent::LoadCallback(const AHashString& resource, bool success)
{
	if (!success) {
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
	if (!_material_res || !_model_res || !_material_res.getResourcePtr()->isLoaded() ||
		!_model_res.getResourcePtr()->isLoaded())
		return;

	//static float rot = 0.0f;
	//rot += 0.001f;

	Gleam::Matrix4x4 tocamera, projection, toworld;
	tocamera.setLookAtLH(0.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	projection.setPerspectiveLH(90.0f, 16.0f / 9.0f, 0.1f, 5000.0f);
	toworld.setIdentity();
	//toworld.setRotationY(rot);

	// Update camera data
	Gleam::IBuffer* buffer = _program_buffers->getConstantBuffer(Gleam::IShader::SHADER_VERTEX, 0);

	_render_mgr.getRenderDevice().getActiveOutputRenderTarget()->bind(_render_mgr.getRenderDevice());

	float* matrix_data = (float*)buffer->map(_render_mgr.getRenderDevice());
		memcpy(matrix_data, toworld.getBuffer(), sizeof(float) * 16);
		memcpy(matrix_data + 16, tocamera.getBuffer(), sizeof(float) * 16);
		memcpy(matrix_data + 32, projection.getBuffer(), sizeof(float) * 16);
	buffer->unmap(_render_mgr.getRenderDevice());

	_material_res->programs[0]->bind(_render_mgr.getRenderDevice(), _program_buffers.get());

	for (unsigned int i = 0; i < _model_res->models[0][0]->getMeshCount(); ++i) {
		_model_res->models[0][0]->render(_render_mgr.getRenderDevice(), i);
	}
}

NS_END
