/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Model/Shibboleth_ModelPipelineData.h"
#include "Shibboleth_RenderManager.h"
#include <Gleam_ShaderResourceView.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_Texture.h>
#include <Gaff_ContainerAlgorithm.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ModelPipelineData)
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::ModelPipelineData)



namespace
{
	template <class T>
	static const auto* GetResourceMap(const T& texture_data, Gleam::IShader::Type shader_type)
	{
		switch (shader_type) {
			case Gleam::IShader::Type::Vertex:
				return &texture_data.vertex;

			case Gleam::IShader::Type::Pixel:
				return &texture_data.pixel;

			case Gleam::IShader::Type::Domain:
				return &texture_data.domain;

			case Gleam::IShader::Type::Geometry:
				return &texture_data.geometry;

			case Gleam::IShader::Type::Hull:
				return &texture_data.hull;
		}

		GAFF_ASSERT(false);
		return static_cast<decltype(&texture_data.vertex)>(nullptr);
	}

	static constexpr const char8_t* const k_mtp_mat_name = u8"_model_to_proj_matrix";
	static Shibboleth::ProxyAllocator s_allocator{ GRAPHICS_ALLOCATOR };
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ModelPipelineData)

bool ModelPipelineData::init(RenderManager& render_mgr)
{
	_render_mgr = &render_mgr;
	return true;
}

ModelInstanceHandle ModelPipelineData::registerModel(const ModelInstanceData& model_data, const ITransformProvider& tform_provider)
{
	const ModelInstanceHandle handle = model_data.createInstanceHandle(tform_provider);
	const EA::Thread::AutoMutex lock(_new_models_lock);
	auto it_bucket = _new_models.find(handle.bucket_hash);

	if (it_bucket == _new_models.end()) {
		const auto result = _new_models.insert(handle.bucket_hash);
		GAFF_ASSERT(result.second);

		it_bucket = result.first;
		it_bucket->second.data = model_data;
	}

	it_bucket->second.handles.emplace_back(handle);

	return handle;
}

void ModelPipelineData::unregisterModel(ModelInstanceHandle handle)
{
	const EA::Thread::AutoMutex lock(_pending_removes_lock);
	auto& handles = _pending_removes[handle.bucket_hash];

	if (handles.empty()) {
		handles.set_allocator(GRAPHICS_ALLOCATOR);
	}

	handles.emplace_back(handle);
}

void ModelPipelineData::markDirty(ModelInstanceHandle handle)
{
	// $TODO: Implement.
	GAFF_REF(handle);
}

void ModelPipelineData::processChanges(uintptr_t /*thread_id_int*/)
{
	// Cache the queues so that while the next frame is simulating, it doesn't stomp over anything.
	{
		const EA::Thread::AutoMutex remove_lock(_pending_removes_lock);
		const EA::Thread::AutoMutex new_lock(_new_models_lock);

		_pending_removes.swap(_pending_removes_cache);
		_new_models.swap(_new_models_cache);
	}

	// $TODO: Might want to thread these.
	for (const auto& entry : _pending_removes_cache) {
		for (const ModelInstanceHandle& handle : entry.second) {
			removeInstance(handle);
		}
	}

	for (const auto& entry : _new_models) {
		for (const ModelInstanceHandle& handle : entry.second.handles) {
			addInstance(entry.second.data, handle);
		}
	}

	_pending_removes_cache.clear();
	_new_models_cache.clear();
}

void ModelPipelineData::addInstance(const ModelInstanceData& model_data, ModelInstanceHandle handle)
{
	auto it_bucket = _model_buckets.find(handle.bucket_hash);
	ModelBucket& bucket = (it_bucket == _model_buckets.end()) ?
		createBucket(model_data, handle) :
		it_bucket->second;

	const auto it_tform = Gaff::LowerBound(bucket.transform_providers, handle.transform_provider);

	// Already registered.
	if (it_tform != bucket.transform_providers.end() && *it_tform == handle.transform_provider) {
		return;
	}

	bucket.transform_providers.emplace(it_tform, handle.transform_provider);

	//MeshInstance& mesh_instance = (bucket.mesh_instances.empty() || bucket.mesh_instances.back().buffer_instance_count == model_data.instances_per_page) ?
		//createNewMeshInstance(bucket, model_data)) :
		//bucket.mesh_instance.last();

//	if (!mesh_instance.buffer_instance_count) {
//	}
}

void ModelPipelineData::removeInstance(ModelInstanceHandle handle)
{
	// Check pending handles first.
	const auto it_new_bucket = _new_models_cache.find(handle.bucket_hash);

	if (it_new_bucket != _new_models_cache.end()) {
		const auto it = Gaff::Find(it_new_bucket->second.handles, handle);

		if (it != it_new_bucket->second.handles.end()) {
			it_new_bucket->second.handles.erase_unsorted(it);

			if (it_new_bucket->second.handles.empty()) {
				_new_models_cache.erase(it_new_bucket);
			}

			return;
		}
	}
}

ModelPipelineData::ModelBucket& ModelPipelineData::createBucket(const ModelInstanceData& model_data, ModelInstanceHandle handle)
{
	ModelBucket& model_bucket = _model_buckets[handle.bucket_hash];

	// $TODO: Maybe error check this?
	// Assuming that all resources have the same devices.
	const auto devices = model_data.material_data.front().material->getDevices();
	GAFF_ASSERT(!devices.empty());

	model_bucket.mesh_instances.resize(model_data.model->getNumMeshes());

	Gleam::ShaderReflection shader_refl[static_cast<size_t>(Gleam::IShader::Type::Count)];

	// Shader reflection should be the same across all devices. Caching once up front instead of requesting it per device.
	for (const Gleam::IShader::Type shader_type : Gaff::EnumIterator<Gleam::IShader::Type>()) {
		const int32_t shader_type_index = static_cast<int32_t>(shader_type);
		const Gleam::Program* const program = model_data.material_data[0].material->getProgram(*devices[0]);
		const Gleam::Shader* const shader = (program) ? static_cast<const Gleam::Shader*>(program->getAttachedShader(shader_type)) : nullptr;

		GAFF_ASSERT(shader);

		shader_refl[shader_type_index] = shader->getReflectionData();
	}

	for (int32_t i = 0; i < static_cast<int32_t>(model_bucket.mesh_instances.size()); ++i) {
		const MaterialInstanceData& material_data = model_data.material_data[i];
		MeshInstance& mesh_instance = model_bucket.mesh_instances[i];

		mesh_instance.buffer_instance_count = model_data.model->getInstancesPerBuffer();
		mesh_instance.device_data.reserve(devices.size());

		for (Gleam::RenderDevice* rd : devices) {
			Gleam::ProgramBuffers* const pb = SHIB_ALLOCT(Gleam::ProgramBuffers, s_allocator);
			MeshInstanceDeviceData& device_data = mesh_instance.device_data[rd];

			device_data.program_buffers.reset(pb);

			for (const Gleam::IShader::Type shader_type : Gaff::EnumIterator<Gleam::IShader::Type>()) {
				const int32_t shader_type_index = static_cast<int32_t>(shader_type);
				const auto& pipeline_refl = shader_refl[shader_type_index];

				if (shader_type == Gleam::IShader::Type::Vertex) {
					for (const auto& sb_refl : pipeline_refl.structured_buffers) {
						const auto it = Gaff::Find(sb_refl.vars, k_mtp_mat_name, [](const Gleam::VarReflection& lhs, const char8_t* rhs)->bool { return lhs.name == rhs; });

						if (it != sb_refl.vars.end()) {
							mesh_instance.model_to_proj_offset = static_cast<int32_t>(it->start_offset);
						}
					}
				}

				addStructuredBuffersSRVs(
					mesh_instance,
					*rd,
					device_data,
					shader_type,
					pipeline_refl
				);

				// $TODO: When texture arrays are added, move this out of bucket data and into instance data.
				addTextureSRVs(*rd, device_data, shader_type, pipeline_refl, material_data);

				addConstantBuffers(*rd, device_data, shader_type, pipeline_refl, *pb);
				addSamplers(*rd, shader_type, pipeline_refl, material_data, *pb);

				const auto& buffer_vars = device_data.pipeline_data[shader_type_index].buffer_vars;
				const auto& srv_vars = device_data.pipeline_data[shader_type_index].srv_vars;

				for (const Gleam::U8String& var_name : pipeline_refl.var_decl_order) {
					// $TODO: Just use Hash32 instead? No need to store copy of string just to do a lookup on the hash.
					const HashString32<> name(var_name.data());
					const auto it = srv_vars.find(name);

					if (it != srv_vars.end()) {
						pb->addResourceView(shader_type, *it->second.srv);

					} else {
						const auto it_buf = buffer_vars.find(name);

						if (it_buf != buffer_vars.end()) {
							pb->addResourceView(shader_type, *it_buf->second.pages[0].srv);
						} else {
							// $TODO: Log error.
						}
					}
				}
			}
		}
	}

	return model_bucket;
}

void ModelPipelineData::addStructuredBuffersSRVs(
	MeshInstance& mesh_instance,
	Gleam::RenderDevice& device,
	MeshInstanceDeviceData& device_data,
	const Gleam::IShader::Type shader_type,
	const Gleam::ShaderReflection& refl)
{
	BufferVarMap& var_map = device_data.pipeline_data[static_cast<int32_t>(shader_type)].buffer_vars;

	for (const auto& sb_refl : refl.structured_buffers) {
		const Gleam::IBuffer::Settings settings = {
			nullptr,
			sb_refl.size_bytes * static_cast<size_t>(mesh_instance.buffer_instance_count), // size
			static_cast<int32_t>(sb_refl.size_bytes), // stride
			static_cast<int32_t>(sb_refl.size_bytes), // elem_size
			Gleam::IBuffer::Type::StructuredData,
			Gleam::IBuffer::MapType::Write,
			true
		};

		Gleam::Buffer* const buffer = SHIB_ALLOCT(Gleam::Buffer, s_allocator);
		GAFF_ASSERT(buffer);

		if (!buffer->init(device, settings)) {
			// $TODO: Log error.
			GAFF_ASSERT(false);
		}

		auto& buffer_data = var_map[HashString32<>(sb_refl.name.data())];

		const auto it = Gaff::Find(refl.var_decl_order, sb_refl.name);
		// Leaving original line here. This didn't make sense to me, but maybe I'm forgetting something.
		// buffer_data.srv_index = (it == refl.var_decl_order.end()) ? -1 : static_cast<int32_t>(eastl::distance(it, refl.var_decl_order.end()) - 1);
		buffer_data.srv_index = (it == refl.var_decl_order.end()) ? -1 : static_cast<int32_t>(eastl::distance(refl.var_decl_order.begin(), it));
		InstanceBuffer& page = buffer_data.pages.emplace_back();

		Gleam::ShaderResourceView* const srv = SHIB_ALLOCT(Gleam::ShaderResourceView, s_allocator);
		GAFF_ASSERT(srv);

		if (!srv->init(device, *buffer)) {
			// $TODO: Log error.
			GAFF_ASSERT(false);
		}

		page.buffer.reset(buffer);
		page.srv.reset(srv);
	}
}

void ModelPipelineData::addTextureSRVs(
	Gleam::RenderDevice& device,
	MeshInstanceDeviceData& device_data,
	const Gleam::IShader::Type shader_type,
	const Gleam::ShaderReflection& refl,
	const MaterialInstanceData& material_data)
{
	const TextureInstanceData::TextureMap* const texture_map = GetResourceMap(material_data.textures, shader_type);
	VarMap& var_map = device_data.pipeline_data[static_cast<int32_t>(shader_type)].srv_vars;

	for (const Gleam::U8String& texture_name : refl.textures) {
		const HashString32<> name(texture_name.data());
		const auto it = texture_map->find(name);

		if (it == texture_map->end()) {
			// $TODO: Use error texture.
			// $TODO: Log error.
			continue;
		}

		Gleam::ShaderResourceView* const srv = SHIB_ALLOCT(Gleam::ShaderResourceView, s_allocator);
		const Gleam::Texture* const texture = it->second->getTexture(device);

		if (!srv->init(device, *texture)) {
			// $TODO: Log error.
			GAFF_ASSERT(false);
		}

		InstanceTexture& inst = var_map[std::move(name)];
		inst.texture = texture;
		inst.srv.reset(srv);
	}
}

void ModelPipelineData::addConstantBuffers(
	Gleam::RenderDevice& device,
	MeshInstanceDeviceData& device_data,
	const Gleam::IShader::Type shader_type,
	const Gleam::ShaderReflection& refl,
	Gleam::ProgramBuffers& pb)
{
	ConstBufferVarMap& var_map = device_data.pipeline_data[static_cast<int32_t>(shader_type)].const_buffer_vars;

	for (const Gleam::ConstBufferReflection& const_buf_refl : refl.const_buff_reflection) {
		const Gleam::IBuffer::Settings settings = {
			nullptr,
			const_buf_refl.size_bytes, // size
			static_cast<int32_t>(const_buf_refl.size_bytes), // stride
			static_cast<int32_t>(const_buf_refl.size_bytes), // elem_size
			Gleam::IBuffer::Type::ShaderConstantData,
			Gleam::IBuffer::MapType::Write,
			true
		};

		Gleam::Buffer* const buffer = SHIB_ALLOCT(Gleam::Buffer, s_allocator);

		if (!buffer->init(device, settings)) {
			// $TODO: Log error.
			GAFF_ASSERT(false);
		}

		const HashString32<> name{ const_buf_refl.name.data() };
		var_map[std::move(name)].reset(buffer);

		pb.addConstantBuffer(shader_type, *buffer);
	}
}

void ModelPipelineData::addSamplers(
	Gleam::RenderDevice& device,
	const Gleam::IShader::Type shader_type,
	const Gleam::ShaderReflection& refl,
	const MaterialInstanceData& material_data,
	Gleam::ProgramBuffers& pb)
{
	const SamplerInstanceData::SamplerMap* const sampler_map = GetResourceMap(material_data.samplers, shader_type);
	const ResourcePtr<SamplerStateResource>& default_sampler_res = _render_mgr->getDefaultSamplerState();
	const Gleam::SamplerState* const default_sampler = default_sampler_res->getSamplerState(device);

	for (const Gleam::U8String& sampler_name : refl.samplers) {
		const HashString32<> key(sampler_name.data());
		const auto it = sampler_map->find(key);

		if (it == sampler_map->end()) {
			pb.addSamplerState(shader_type, *default_sampler);

		} else {
			const Gleam::SamplerState* const sampler = it->second->getSamplerState(device);

			if (sampler) {
				pb.addSamplerState(shader_type, *sampler);
			} else {
				// $TODO: Log error.
				pb.addSamplerState(shader_type, *default_sampler);
			}
		}
	}
}

NS_END
