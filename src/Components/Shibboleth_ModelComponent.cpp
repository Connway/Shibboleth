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

//#include <Shibboleth_LoadingMessage.h>

#include <Shibboleth_SchemaManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>

#include <Gleam_IRenderDevice.h>
#include <Gleam_Matrix4x4.h>
#include <Gleam_IProgram.h>
#include <Gleam_IBuffer.h>

NS_SHIBBOLETH

// Defining enum reflection for Gleam::IBuffer here because I don't think we will ever need it outside of this file.
SHIB_ENUM_REF_DEF_EMBEDDED(BUFFER_TYPE, Gleam::IBuffer::BUFFER_TYPE);
SHIB_ENUM_REF_IMPL_EMBEDDED(BUFFER_TYPE, Gleam::IBuffer::BUFFER_TYPE)
.addValue("Shader Data", Gleam::IBuffer::SHADER_DATA)
.addValue("Structured Data", Gleam::IBuffer::STRUCTURED_DATA)
;

SHIB_ENUM_REF_DEF_EMBEDDED(MAP_TYPE, Gleam::IBuffer::MAP_TYPE);
SHIB_ENUM_REF_IMPL_EMBEDDED(MAP_TYPE, Gleam::IBuffer::MAP_TYPE)
.addValue("None", Gleam::IBuffer::NONE)
.addValue("Read", Gleam::IBuffer::READ)
.addValue("Write", Gleam::IBuffer::WRITE)
.addValue("Read/Write", Gleam::IBuffer::READ_WRITE)
.addValue("Write No Overwrite", Gleam::IBuffer::WRITE_NO_OVERWRITE)
;


COMP_REF_DEF_SAVE(ModelComponent, gRefDef);
REF_IMPL_REQ(ModelComponent);

SHIB_REF_IMPL(ModelComponent)
.addBaseClassInterfaceOnly<ModelComponent>()
.addArray("Render Mode Overrides", &ModelComponent::_render_mode_overrides, GetEnumRefDef<RenderModes>())
;

ModelComponent::ModelComponent(void):
	_render_mgr(Shibboleth::GetApp().getManagerT<Shibboleth::RenderManager>("Render Manager")),
	_requests_finished(0), _total_requests(0), _init(false)
{
}

ModelComponent::~ModelComponent(void)
{
	auto callback = Gaff::Bind(this, &ModelComponent::ResourceLoadedCallback);

	for (auto it = _program_buffers.begin(); it != _program_buffers.end(); ++it) {
		it->getResourcePtr()->removeCallback(callback);
	}

	for (auto it = _textures.begin(); it != _textures.end(); ++it) {
		it->getResourcePtr()->removeCallback(callback);
	}

	for (auto it = _materials.begin(); it != _materials.end(); ++it) {
		it->getResourcePtr()->removeCallback(callback);
	}

	for (auto it = _samplers.begin(); it != _samplers.end(); ++it) {
		it->getResourcePtr()->removeCallback(callback);
	}

	for (auto it = _buffers.begin(); it != _buffers.end(); ++it) {
		it->getResourcePtr()->removeCallback(callback);
	}

	_model.getResourcePtr()->removeCallback(callback);
}

const Gaff::JSON& ModelComponent::getSchema(void) const
{
	static const Gaff::JSON& schema = GetApp().getManagerT<SchemaManager>("Schema Manager").getSchema("ModelComponent.schema");
	return schema;
}

bool ModelComponent::validate(const Gaff::JSON& json)
{
	if (!Component::validate(json)) {
		// log error
		return false;
	}

	Gaff::JSON material_files = json["Material Files"];
	Gaff::JSON texture_files = json["Texture Files"];
	Gaff::JSON sampler_files = json["Sampler Files"];
	Gaff::JSON buffer_map = json["Buffer Map"];
	Gaff::JSON buffers = json["Buffers"];
	Gaff::JSON model_file = json["Model File"];

	if (material_files) {
		_total_requests += material_files.size() * 2;
	}

	if (texture_files) {
		_total_requests += texture_files.size();
	}

	if (sampler_files) {
		_total_requests += sampler_files.size();
	}

	if (model_file) {
		_total_requests += 1;
	}

	if (buffers) {
		_total_requests += buffers.size();
	}

	return true;
}

bool ModelComponent::load(const Gaff::JSON& json)
{
	Gaff::JSON model_file = json["Model File"];
	gRefDef.read(json, this);

	ResourceManager& res_mgr = Shibboleth::GetApp().getManagerT<Shibboleth::ResourceManager>("Resource Manager");

	if (model_file.isString()) {
		_model = res_mgr.requestResource(model_file.getString());
		_model.getResourcePtr()->addCallback(Gaff::Bind(this, &ModelComponent::ResourceLoadedCallback));
	}

	requestMaterials(json, res_mgr);
	requestTextures(json, res_mgr);
	requestSamplers(json, res_mgr);
	requestBuffers(json, res_mgr);

	//Shibboleth::GetApp().getBroadcaster().listen<LoadingMessage>(Gaff::Bind(this, &ModelComponent::HandleLoadingMessage));

	return true;
}

void ModelComponent::allComponentsLoaded(void)
{
}

size_t ModelComponent::determineLOD(const Gleam::Vector4CPU& pos)
{
	// IMPLEMENT ME!
	return 0;
}

const Array< ResourceWrapper<ProgramBuffersData> >& ModelComponent::getProgramBuffers(void) const
{
	return _program_buffers;
}

Array< ResourceWrapper<ProgramBuffersData> >& ModelComponent::getProgramBuffers(void)
{
	return _program_buffers;
}

const Array< ResourceWrapper<ProgramData> >& ModelComponent::getMaterials(void) const
{
	return _materials;
}

Array< ResourceWrapper<ProgramData> >& ModelComponent::getMaterials(void)
{
	return _materials;
}

const Array< ResourceWrapper<BufferData> >& ModelComponent::getBuffers(void) const
{
	return _buffers;
}

Array< ResourceWrapper<BufferData> >& ModelComponent::getBuffers(void)
{
	return _buffers;
}

const ModelData& ModelComponent::getModel(void) const
{
	return *_model;
}

ModelData& ModelComponent::getModel(void)
{
	return *_model;
}

const Array<size_t>* ModelComponent::getRenderModes(void) const
{
	return _material_modes;
}

void ModelComponent::ResourceLoadedCallback(ResourceContainer* resource)
{
	if (resource->isLoaded()) {
		size_t new_val = AtomicIncrement(&_requests_finished);

		if (new_val == _total_requests) {
			setupResources();
		}

	} else {
		// handle failure
	}
}

//void ModelComponent::HandleLoadingMessage(const LoadingMessage& msg)
//{
//	if (msg.state == LoadingMessage::LOADING_FINISHED) {
//		_model->holding_data.getResourcePtr() = nullptr;
//	}
//}

void ModelComponent::requestMaterials(const Gaff::JSON& json, ResourceManager& res_mgr)
{
	auto callback = Gaff::Bind(this, &ModelComponent::ResourceLoadedCallback);
	Gaff::JSON material_files = json["Material Files"];

	if (material_files.isArray()) {
		char temp[256] = { 0 };

		_program_buffers.resize(material_files.size());
		_materials.resize(material_files.size());

		material_files.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
		{
			ResourceWrapper<ProgramData> material = res_mgr.requestResource(value.getString());
			material.getResourcePtr()->addCallback(callback);
			_materials[index] = material;

			// Make a unique name for this resource
			sprintf(temp, "ProgramBuffers#%s#%s#%u%zu", getOwner()->getName().getBuffer(), getName().getBuffer(), getOwner()->getID(), index);
			ResourceWrapper<ProgramBuffersData> program_buffers = res_mgr.requestResource("ProgramBuffers", temp);
			program_buffers.getResourcePtr()->addCallback(callback);
			_program_buffers[index] = program_buffers;

			return false;
		});
	}
}

void ModelComponent::requestTextures(const Gaff::JSON& json, ResourceManager& res_mgr)
{
	auto callback = Gaff::Bind(this, &ModelComponent::ResourceLoadedCallback);
	Gaff::JSON texture_files = json["Texture Files"];

	if (texture_files.isArray()) {
		_texture_mappings.resize(texture_files.size());
		_textures.resize(texture_files.size());

		texture_files.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
		{
			Gaff::JSON file = value["File"];
			Gaff::JSON material_map = value["Material Map"];

			ResourceWrapper<TextureData> texture = res_mgr.requestResource(file.getString());
			texture.getResourcePtr()->addCallback(callback);
			_textures[index] = texture;

			_texture_mappings[index].resize(material_map.size());

			material_map.forEachInArray([&](size_t index2, const Gaff::JSON& value2) -> bool
			{
				Gaff::JSON material_index = value2["Material Index"];
				Gaff::JSON shader = value2["Shader"];

				MaterialMapping mapping;
				mapping.first = static_cast<size_t>(material_index.getInteger());
				mapping.second = GetEnumRefDef<Gleam::IShader::SHADER_TYPE>().getValue(shader.getString());

				_texture_mappings[index][index2] = mapping;

				return false;
			});

			return false;
		});
	}
}

void ModelComponent::requestSamplers(const Gaff::JSON& json, ResourceManager& res_mgr)
{
	auto callback = Gaff::Bind(this, &ModelComponent::ResourceLoadedCallback);
	Gaff::JSON sampler_files = json["Sampler Files"];

	if (sampler_files.isArray()) {
		_sampler_mappings.resize(sampler_files.size());
		_samplers.resize(sampler_files.size());

		sampler_files.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
		{
			Gaff::JSON file = value["File"];
			Gaff::JSON material_map = value["Material Map"];

			ResourceWrapper<SamplerStateData> sampler = res_mgr.requestResource(file.getString());
			sampler.getResourcePtr()->addCallback(callback);
			_samplers[index] = sampler;

			_sampler_mappings[index].resize(material_map.size());

			material_map.forEachInArray([&](size_t index2, const Gaff::JSON& value2) -> bool
			{
				Gaff::JSON material_index = value2["Material Index"];
				Gaff::JSON shader = value2["Shader"];

				MaterialMapping mapping;
				mapping.first = static_cast<size_t>(material_index.getInteger());
				mapping.second = GetEnumRefDef<Gleam::IShader::SHADER_TYPE>().getValue(shader.getString());

				_sampler_mappings[index][index2] = mapping;

				return false;
			});

			return false;
		});
	}
}

void ModelComponent::requestBuffers(const Gaff::JSON& json, ResourceManager& res_mgr)
{
	auto callback = Gaff::Bind(this, &ModelComponent::ResourceLoadedCallback);
	Gaff::JSON buffers = json["Buffers"];

	if (buffers.isArray()) {
		_buffer_mappings.resize(buffers.size());
		_buffer_settings.resize(buffers.size());
		_buffers.resize(buffers.size());

		const auto& buffer_type = GetEnumRefDef<Gleam::IBuffer::BUFFER_TYPE>();
		const auto& map_type = GetEnumRefDef<Gleam::IBuffer::MAP_TYPE>();

		buffers.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
		{
			Gaff::JSON type = value["Type"];
			Gaff::JSON cpu_access = value["CPU Access"];
			Gaff::JSON size = value["Size"];
			Gaff::JSON stride = value["Stride"];
			Gaff::JSON gpu_read_only = value["GPU Read Only"];
			Gaff::JSON material_map = value["Material Map"];

			Gleam::IBuffer::BufferSettings buffer_settings = {
				nullptr,
				static_cast<unsigned int>(size.getInteger()),
				static_cast<unsigned int>(stride.getInteger()),
				buffer_type.getValue(type.getString()),
				map_type.getValue(cpu_access.getString()),
				gpu_read_only.isTrue()
			};

			_buffer_settings[index] = buffer_settings;

			// Make a unique name for this resource
			char temp[256] = { 0 };
			sprintf(temp, "Buffer#%s#%s#%u%zu", getOwner()->getName().getBuffer(), getName().getBuffer(), getOwner()->getID(), index);
			ResourceWrapper<BufferData> buffer = res_mgr.requestResource("Buffer", temp, reinterpret_cast<unsigned long long>(&_buffer_settings[index]));
			buffer.getResourcePtr()->addCallback(callback);
			_buffers[index] = buffer;

			_buffer_mappings[index].resize(material_map.size());

			material_map.forEachInArray([&](size_t index2, const Gaff::JSON& value2) -> bool
			{
				Gaff::JSON material_index = value2["Material Index"];
				Gaff::JSON shader = value2["Shader"];

				MaterialMapping mapping;
				mapping.first = static_cast<size_t>(material_index.getInteger());
				mapping.second = GetEnumRefDef<Gleam::IShader::SHADER_TYPE>().getValue(shader.getString());

				_buffer_mappings[index][index2] = mapping;

				return false;
			});

			return false;
		});
	}
}

void ModelComponent::setupResources(void)
{
	assert(_model->models[0][0]->getMeshCount() == _materials.size());

	for (size_t i = 0; i < _model->models[0][0]->getMeshCount(); ++i) {
		if (i < _render_mode_overrides.size()) {
			_render_mode_overrides[i] = (_render_mode_overrides[i] == RM_NONE) ? _materials[i]->render_mode : _render_mode_overrides[i];
		} else {
			_render_mode_overrides.emplacePush(_materials[i]->render_mode);
		}

		_material_modes[_render_mode_overrides[i]].emplacePush(i);
	}

	for (size_t i = 0; i < RM_COUNT; ++i) {
		_material_modes[i].trim();
	}

	unsigned int num_devices = Shibboleth::GetApp().getManagerT<Shibboleth::RenderManager>("Render Manager").getRenderDevice().getNumDevices();

	for (unsigned int device = 0; device < num_devices; ++device) {
		// for each texture
		//		add to program buffers via mapping
		for (size_t i = 0; i < _textures.size(); ++i) {
			Array<MaterialMapping>& texture_mappings = _texture_mappings[i];

			for (size_t j = 0; j < texture_mappings.size(); ++j) {
				MaterialMapping& mat_map = texture_mappings[j];

				ProgramBuffersPtr& program_buffers = _program_buffers[mat_map.first]->data[device];
				ShaderResourceViewPtr& res_view = _textures[i]->resource_views[device];

				if (program_buffers && res_view) {
					program_buffers->addResourceView(mat_map.second, res_view.get());
				}
			}
		}

		// for each sampler
		//		add to program buffers via mapping
		for (size_t i = 0; i < _samplers.size(); ++i) {
			Array<MaterialMapping>& sampler_mappings = _sampler_mappings[i];

			for (size_t j = 0; j < sampler_mappings.size(); ++j) {
				MaterialMapping& mat_map = sampler_mappings[j];

				ProgramBuffersPtr& program_buffers = _program_buffers[mat_map.first]->data[device];
				SamplerStatePtr& sampler = _samplers[i]->data[device];

				if (program_buffers && sampler) {
					program_buffers->addSamplerState(mat_map.second, sampler.get());
				}
			}
		}

		// for each buffer
		//		add to program buffers
		for (size_t i = 0; i < _buffers.size(); ++i) {
			BufferPtr& buffer = _buffers[i]->data[device];

			if (buffer) {
				Array<MaterialMapping>& buffer_mappings = _buffer_mappings[i];

				for (size_t j = 0; j < buffer_mappings.size(); ++j) {
					MaterialMapping mat_map = buffer_mappings[j];
					ProgramBuffersPtr& program_buffers = _program_buffers[mat_map.first]->data[device];

					if (program_buffers) {
						program_buffers->addConstantBuffer(mat_map.second, buffer.get());
					}
				}
			}
		}
	}

	// Clear the mappings, we don't need them anymore.
	_buffer_settings.clear();
	_buffer_mappings.clear();
	_texture_mappings.clear();
	_sampler_mappings.clear();

	_init = true;
}

// HACK: Should be removed. For testing purposes only.
void ModelComponent::render(double dt, Gleam::IRenderDevice& rd, unsigned int device)
{
	if (!_init) {
		return;
	}

	static float rot = 0.0f;
	rot += static_cast<float>(Gaff::Pi * dt);

	Gleam::Matrix4x4 tocamera, projection, toworld, final_transform;
	tocamera.setLookAtLH(0.0f, 5.0f, -5.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	projection.setPerspectiveLH(90.0f * Gaff::DegToRad, 4.0f / 3.0f, 0.1f, 5000.0f);
	toworld.setIdentity();
	toworld.setTranslate(getOwner()->getWorldPosition());
	toworld.setRotationY(rot);

	final_transform = projection * tocamera * toworld;

	//Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();
	//unsigned int current_device = rd.getCurrentDevice();
	//rd.getActiveOutputRenderTarget()->bind(rd);

	ModelPtr& model = _model->models[device][0];

	Gleam::IBuffer* buffer = _buffers[0]->data[device].get();

	float* matrix_data = reinterpret_cast<float*>(buffer->map(rd));
		memcpy(matrix_data, final_transform.getBuffer(), sizeof(float) * 16);
	buffer->unmap(rd);

	for (unsigned int i = 0; i < model->getMeshCount(); ++i) {
		_materials[i]->programs[device]->bind(rd, _program_buffers[i]->data[device].get());

		model->render(rd, i);
	}
}

bool ModelComponent::isReadyToRender(void) const
{
	return _init;
}

NS_END
