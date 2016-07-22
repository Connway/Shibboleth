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

#include "Shibboleth_ModelComponent.h"
#include "Shibboleth_ModelAnimResources.h"

#include <Shibboleth_OcclusionUserDataTags.h>
#include <Shibboleth_SchemaManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>

#include <Gleam_IRenderDevice.h>
#include <Gleam_IProgram.h>
#include <Gleam_IBuffer.h>
#include <Gleam_IModel.h>

#define MC_IN_OM 0x01 // In Occlusion Manager
#define MC_STATIC 0x02

//#define MC_INIT 0x04

NS_SHIBBOLETH

// Defining enum reflection for Gleam::IBuffer here because I don't think we will ever need it outside of this file.
SHIB_ENUM_REF_DEF_EMBEDDED(BUFFER_TYPE, Gleam::IBuffer::BUFFER_TYPE);
SHIB_ENUM_REF_IMPL_EMBEDDED(BUFFER_TYPE, Gleam::IBuffer::BUFFER_TYPE)
.addValue("Vertex Data", Gleam::IBuffer::VERTEX_DATA)
.addValue("Index Data", Gleam::IBuffer::INDEX_DATA)
.addValue("Shader Data", Gleam::IBuffer::SHADER_DATA)
//.addValue("Structured Data", Gleam::IBuffer::STRUCTURED_DATA)
;

SHIB_ENUM_REF_DEF_EMBEDDED(MAP_TYPE, Gleam::IBuffer::MAP_TYPE);
SHIB_ENUM_REF_IMPL_EMBEDDED(MAP_TYPE, Gleam::IBuffer::MAP_TYPE)
.addValue("None", Gleam::IBuffer::NONE)
.addValue("Read", Gleam::IBuffer::READ)
.addValue("Write", Gleam::IBuffer::WRITE)
.addValue("Read/Write", Gleam::IBuffer::READ_WRITE)
.addValue("Write No Overwrite", Gleam::IBuffer::WRITE_NO_OVERWRITE)
;

// Helper function for making resource requests.
template <class T>
static void RequestResourceArray(Array<T>& res_array, ResourceManager& res_mgr, const Gaff::JSON& json, const Gaff::FunctionBinder<void, ResourceContainer*>& callback)
{
	if (!json.isNull()) {
		res_array.resize(json.size());

		json.forEachInArray([&](size_t index, const Gaff::JSON& res) -> bool
		{
			res_array[index] = res_mgr.requestResource(res.getString());
			res_array[index].getResourcePtr()->addCallback(callback);
			return false;
		});
	}
}

COMP_REF_DEF_SAVE(ModelComponent, g_ref_def);
REF_IMPL_REQ(ModelComponent);

SHIB_REF_IMPL(ModelComponent)
.addBaseClassInterfaceOnly<ModelComponent>()
;

const char* ModelComponent::GetFriendlyName(void)
{
	return "Model Component";
}

ModelComponent::ModelComponent(void):
	_occlusion_mgr(GetApp().getManagerT<OcclusionManager>("Occlusion Manager")),
	_requests_finished(0), _total_requests(0), _flags(0)
{
}

ModelComponent::~ModelComponent(void)
{
	auto callback = Gaff::Bind(this, &ModelComponent::ResourceLoadedCallback);

	for (auto it_md = _mesh_data.begin(); it_md != _mesh_data.end(); ++it_md) {
		it_md->program_buffers.getResourcePtr()->removeCallback(callback);
		it_md->material.getResourcePtr()->removeCallback(callback);

		for (auto it_tex = it_md->textures.begin(); it_tex != it_md->textures.end(); ++it_tex) {
			it_tex->getResourcePtr()->removeCallback(callback);
		}

		for (auto it_samp = it_md->samplers.begin(); it_samp != it_md->samplers.end(); ++it_samp) {
			it_samp->getResourcePtr()->removeCallback(callback);
		}
	}

	_model.getResourcePtr()->removeCallback(callback);
}

const Gaff::JSON& ModelComponent::getSchema(void) const
{
	static const Gaff::JSON& schema = GetApp().getManagerT<SchemaManager>("Schema Manager").getSchema("ModelComponent.schema");
	return schema;
}

bool ModelComponent::load(const Gaff::JSON& json)
{
	g_ref_def.read(json, this);

	Gaff::JSON model_file = json["Model File"];
	Gaff::JSON materials = json["Materials"];

	_mesh_data.resize(materials.size());

	materials.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool
	{
		Gaff::JSON material = value["Material"];
		Gaff::JSON textures = value["Textures"];
		Gaff::JSON samplers = value["Samplers"];

		_total_requests += (material.isNull()) ? 0 : 2; // One for material, one for program buffers
		_total_requests += (textures.isNull()) ? 0 : textures.size();
		_total_requests += (samplers.isNull()) ? 0 : samplers.size();

		return false;
	});

	if (!model_file.isNull()) {
		_total_requests += 1;
	}

	ResourceManager& res_mgr = Shibboleth::GetApp().getManagerT<Shibboleth::ResourceManager>("Resource Manager");

	if (model_file.isString()) {
		_model = res_mgr.requestResource(model_file.getString());
		_model.getResourcePtr()->addCallback(Gaff::Bind(this, &ModelComponent::ResourceLoadedCallback));
	}

	requestResources(materials, res_mgr);
	return true;
}

void ModelComponent::allComponentsLoaded(void)
{
	Component::allComponentsLoaded();
}

void ModelComponent::addToWorld(void)
{
	Component::addToWorld();

	if (isActive() && _requests_finished == _total_requests) {
		addToOcclusionManager();
	}
}

void ModelComponent::removeFromWorld(void)
{
	Component::removeFromWorld();

	if (Gaff::IsAnyBitSet<char>(_flags, MC_IN_OM)) {
		removeFromOcclusionManager();
	}
}

void ModelComponent::setActive(bool active)
{
	Component::setActive(active);

	if (getOwner()->isInWorld() && active != Gaff::IsAnyBitSet<char>(_flags, MC_IN_OM)) {
		if (active) {
			addToOcclusionManager();
		} else {
			removeFromOcclusionManager();
		}
	}
}

void ModelComponent::setStatic(bool is_static)
{
	if (is_static != isStatic()) {
		if (Gaff::IsAnyBitSet<char>(_flags, MC_IN_OM)) {
			removeFromOcclusionManager();

			if (is_static) {
				Gaff::SetBits<char>(_flags, MC_STATIC);
			} else {
				Gaff::ClearBits<char>(_flags, MC_STATIC);
			}

			addToOcclusionManager();
		}
	}
}

bool ModelComponent::isStatic(void) const
{
	return Gaff::IsAnyBitSet<char>(_flags, MC_STATIC);
}

size_t ModelComponent::determineLOD(const Gleam::Vector4CPU& /*pos*/)
{
	// IMPLEMENT ME!
	return 0;
}

//unsigned int ModelComponent::getMeshInstanceHash(size_t index) const
//{
//	GAFF_ASSERT(_model);
//	unsigned int hash = INIT_HASH32;
//
//	// Find first device with a model in it
//	for (size_t i = 0; i < _model->models.size(); ++i) {
//		// Use LOD 0
//		if (!_model->models[i].empty()) {
//			// Add mesh and shader pointer to hash
//			const Gleam::IMesh* mesh = _model->models[i][0]->getMesh(index);
//			const Gleam::IProgram* program = _materials[index]->programs[i].get();
//
//			hash = Gaff::FNV1aHash32T(&mesh, hash);
//			hash = Gaff::FNV1aHash32T(&program, hash);
//
//			// Add all textures used by shader to hash
//			//for (size_t j = 0; j < _texture_mappings.size(); ++j) {
//			//	for (size_t k = 0; k < _texture_mappings[i].size(); ++k) {
//			//		if (_texture_mappings[j][k].first == index) {
//			//			const Gleam::ITexture* texture = _textures[j]->textures[i].get();
//			//			hash = Gaff::FNV1aHash32T(&texture, hash);
//			//		}
//			//	}
//			//}
//
//			// Add all samplers used by shader to hash
//			//for (size_t j = 0; j < _sampler_mappings.size(); ++j) {
//			//	for (size_t k = 0; k < _sampler_mappings[i].size(); ++k) {
//			//		if (_sampler_mappings[j][k].first == index) {
//			//			const Gleam::ISamplerState* sampler = _samplers[j]->data[i].get();
//			//			hash = Gaff::FNV1aHash32T(&sampler, hash);
//			//		}
//			//	}
//			//}
//
//			// Add all buffers used by shader to hash (this pretty much makes this instance unique and will never be instanced)
//			//for (size_t j = 0; j < _buffer_mappings.size(); ++j) {
//			//	for (size_t k = 0; k < _buffer_mappings[i].size(); ++k) {
//			//		if (_buffer_mappings[j][k].first == index) {
//			//			const Gleam::IBuffer* buffer = _buffers[j]->data[i].get();
//			//			hash = Gaff::FNV1aHash32T(&buffer, hash);
//			//		}
//			//	}
//			//}
//
//			break;
//		}
//	}
//
//	return hash;
//}

const Array<ModelComponent::MeshData>& ModelComponent::getMeshData(void) const
{
	return _mesh_data;
}

Array<ModelComponent::MeshData>& ModelComponent::getMeshData(void)
{
	return _mesh_data;
}

const ModelData& ModelComponent::getModel(void) const
{
	return *_model;
}

ModelData& ModelComponent::getModel(void)
{
	return *_model;
}

void ModelComponent::ResourceLoadedCallback(ResourceContainer* resource)
{
	if (resource->isLoaded()) {
		size_t new_val = AtomicIncrement(&_requests_finished);

		if (new_val == _total_requests) {
			setupResources();

			if (getOwner()->isInWorld() && isActive()) {
				addToOcclusionManager();
			}
		}

	} else {
		// handle failure
	}
}

void ModelComponent::requestResources(const Gaff::JSON& materials, ResourceManager& res_mgr)
{
	auto callback = Gaff::Bind(this, &ModelComponent::ResourceLoadedCallback);
	char temp[256] = { 0 };

	materials.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
	{
		MeshData& mesh_data = _mesh_data[index];
		Gaff::JSON material = value["Material"];
		Gaff::JSON textures = value["Textures"];
		Gaff::JSON samplers = value["Samplers"];

		if (!material.isNull()) {
			// Make a unique name for this resource
			snprintf(temp, 256, "ProgramBuffers#%s#%s#%u%zu", getOwner()->getName(), getName().getBuffer(), getOwner()->getID(), index);

			mesh_data.program_buffers = res_mgr.requestResource("ProgramBuffers", temp);
			mesh_data.material = res_mgr.requestResource(material.getString());

			mesh_data.program_buffers.getResourcePtr()->addCallback(callback);
			mesh_data.material.getResourcePtr()->addCallback(callback);
		}

		RequestResourceArray(mesh_data.textures, res_mgr, textures, callback);
		RequestResourceArray(mesh_data.samplers, res_mgr, samplers, callback);

		return false;
	});
}

void ModelComponent::setupResources(void)
{
	GAFF_ASSERT(_model->models[0][0]->getMeshCount() == _mesh_data.size());

	unsigned int num_devices = GetApp().getManagerT<RenderManager>().getRenderDevice().getNumDevices();

	for (auto it_md = _mesh_data.begin(); it_md != _mesh_data.end(); ++it_md) {
		for (unsigned int device = 0; device < num_devices; ++device) {
			Gleam::ProgramReflection prog_refl = it_md->material->programs[device]->getReflectionData();
			ProgramBuffersPtr& program_buffers = it_md->program_buffers->data[device];

			GAFF_ASSERT(it_md->textures.size() == prog_refl.total_textures);
			GAFF_ASSERT(it_md->samplers.size() == prog_refl.total_samplers);

			// Map textures
			if (!it_md->textures.empty()) {
				size_t curr_shader_type = 0;
				size_t curr_tex_count = 0;

				// Find first shader with textures
				while (curr_shader_type < Gleam::IShader::SHADER_TYPE_SIZE - 1 &&
					!prog_refl.shader_reflection[curr_shader_type].num_textures) {

					++curr_shader_type;
				}

				// Map all the textures to the program buffer
				for (auto it_tex = it_md->textures.begin(); it_tex != it_md->textures.end(); ++it_tex) {
					ShaderResourceViewPtr& res_view = (*it_tex)->resource_views[device];
					program_buffers->addResourceView(static_cast<Gleam::IShader::SHADER_TYPE>(curr_shader_type), res_view.get());

					++curr_tex_count;

					// If this shader has all its textures mapped, find the next shader that needs textures.
					if (curr_tex_count >= prog_refl.shader_reflection[curr_shader_type].num_textures) {
						++curr_shader_type;

						while (curr_shader_type < Gleam::IShader::SHADER_TYPE_SIZE - 1 &&
							!prog_refl.shader_reflection[curr_shader_type].num_textures) {

							++curr_shader_type;
						}
					}
				}
			}

			// Map shaders
			if (!it_md->samplers.empty()) {
				size_t curr_shader_type = 0;
				size_t curr_sampler_count = 0;

				// Find first shader with samplers
				while (curr_shader_type < Gleam::IShader::SHADER_TYPE_SIZE - 1 &&
					!prog_refl.shader_reflection[curr_shader_type].num_samplers) {

					++curr_shader_type;
				}

				// Map all the samplers to the program buffer
				for (auto it_samp = it_md->samplers.begin(); it_samp != it_md->samplers.end(); ++it_samp) {
					SamplerStatePtr& sampler = (*it_samp)->data[device];
					program_buffers->addSamplerState(static_cast<Gleam::IShader::SHADER_TYPE>(curr_shader_type), sampler.get());

					++curr_sampler_count;

					// If this shader has all its samplers mapped, find the next shader that needs samplers.
					if (curr_sampler_count >= prog_refl.shader_reflection[curr_shader_type].num_samplers) {
						++curr_shader_type;

						while (curr_shader_type < Gleam::IShader::SHADER_TYPE_SIZE - 1 &&
							!prog_refl.shader_reflection[curr_shader_type].num_samplers) {
							
							++curr_shader_type;
						}
					}
				}
			}
		}
	}

	getOwner()->setLocalAABB(_model->combined_aabb);
}

void ModelComponent::addToOcclusionManager(void)
{
	GAFF_ASSERT(!Gaff::IsAnyBitSet<char>(_flags, MC_IN_OM));

	_occlusion_id = _occlusion_mgr.addObject(
		getOwner(), (isStatic()) ? OcclusionManager::OT_STATIC : OcclusionManager::OT_DYNAMIC,
		OcclusionManager::UserData(uint64_t(this), OMT_MODEL_COMP)
	);

	Gaff::SetBits<char>(_flags, MC_IN_OM);
}

void ModelComponent::removeFromOcclusionManager(void)
{
	GAFF_ASSERT(Gaff::IsAnyBitSet<char>(_flags, MC_IN_OM));
	_occlusion_mgr.removeObject(_occlusion_id);
	_occlusion_id = OcclusionManager::OcclusionID();
	Gaff::ClearBits<char>(_flags, MC_IN_OM);
}

NS_END
