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

#include "Shibboleth_RenderCommandSystem.h"
#include "Shibboleth_RenderManagerBase.h"
#include "Shibboleth_CameraComponent.h"
#include <Shibboleth_ECSComponentCommon.h>
#include <Shibboleth_ECSManager.h>
#include <gtx/euler_angles.hpp>

SHIB_REFLECTION_DEFINE_BEGIN(RenderCommandSubmissionSystem)
	.BASE(ISystem)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(RenderCommandSubmissionSystem)

SHIB_REFLECTION_DEFINE_BEGIN(RenderCommandSystem)
	.BASE(ISystem)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(RenderCommandSystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(RenderCommandSubmissionSystem)
SHIB_REFLECTION_CLASS_DEFINE(RenderCommandSystem)

static constexpr const char* k_mtp_mat_name = "_model_to_proj_matrix";

static const Material::TextureMap* GetTextureMap(const Material& material, Gleam::IShader::Type shader_type)
{
	const Material::TextureMap* texture_map = nullptr;

	switch (shader_type) {
		case Gleam::IShader::Type::Vertex:
			texture_map = &material.textures_vertex;
			break;

		case Gleam::IShader::Type::Pixel:
			texture_map = &material.textures_pixel;
			break;

		case Gleam::IShader::Type::Domain:
			texture_map = &material.textures_domain;
			break;

		case Gleam::IShader::Type::Geometry:
			texture_map = &material.textures_geometry;
			break;

		case Gleam::IShader::Type::Hull:
			texture_map = &material.textures_hull;
			break;

		default:
			break;
	}

	return texture_map;
}

static const Material::SamplerMap* GetSamplereMap(const Material& material, Gleam::IShader::Type shader_type)
{
	const Material::SamplerMap* sampler_map = nullptr;

	switch (shader_type) {
		case Gleam::IShader::Type::Vertex:
			sampler_map = &material.samplers_vertex;
			break;

		case Gleam::IShader::Type::Pixel:
			sampler_map = &material.samplers_pixel;
			break;

		case Gleam::IShader::Type::Domain:
			sampler_map = &material.samplers_domain;
			break;

		case Gleam::IShader::Type::Geometry:
			sampler_map = &material.samplers_geometry;
			break;

		case Gleam::IShader::Type::Hull:
			sampler_map = &material.samplers_hull;
			break;

		default:
			break;
	}

	return sampler_map;
}

template <class Map>
static void AddResourcesToWaitList(const Map& resource_map, Vector<IResource*>& list)
{
	for (const auto& pair : resource_map) {
		list.emplace_back(pair.second.get());
	}
}



//RenderCommandSubmissionSystem
bool RenderCommandSubmissionSystem::init(void)
{
	_render_mgr = &GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	return true;
}

void RenderCommandSubmissionSystem::update(uintptr_t thread_id_int)
{
	const int32_t starting_index = static_cast<int32_t>(_job_data_cache.size());
	const int32_t num_devices = _render_mgr->getNumDevices();

	if (num_devices > starting_index) {
		_submission_job_data_cache.resize(num_devices);
		_job_data_cache.resize(num_devices);

		for (int32_t i = starting_index; i < num_devices; ++i) {
			Gleam::IRenderDevice& device = _render_mgr->getDevice(i);

			_submission_job_data_cache[i].device = &device;
			_submission_job_data_cache[i].rcss = this;

			_job_data_cache[i].job_data = &_submission_job_data_cache[i];
			_job_data_cache[i].job_func = SubmitCommands;
		}
	}

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

	auto& job_pool = GetApp().getJobPool();
	job_pool.addJobs(_job_data_cache.data(), static_cast<int32_t>(_job_data_cache.size()), _job_counter);
	job_pool.helpWhileWaiting(thread_id, _job_counter);

	_cache_index = (_cache_index + 1) % 2;
	_render_mgr->presentAllOutputs();
}

void RenderCommandSubmissionSystem::SubmitCommands(uintptr_t /*thread_id_int*/, void* data)
{
	SubmissionData& job_data = *reinterpret_cast<SubmissionData*>(data);

	for (int32_t i = 0; i < static_cast<int32_t>(RenderManagerBase::RenderOrder::Count); ++i) {
		auto& cmds = job_data.rcss->_render_mgr->getRenderCommands(
			*job_data.device,
			static_cast<RenderManagerBase::RenderOrder>(i),
			job_data.rcss->_cache_index
		);

		for (RenderManagerBase::RenderCommand& cmd : cmds.command_list) {
			if (!cmd.cmd_list->isValid()) {
				// $TODO: Log error.
				continue;
			}

			job_data.device->executeCommandList(*cmd.cmd_list);

			// Do not deallocate.
			if (!cmd.owns_command) {
				cmd.cmd_list.release();
			}
		}

		job_data.device->clearRenderState();
		cmds.command_list.clear();
	}
}



// RenderCommandSystem
bool RenderCommandSystem::init(void)
{
	ECSQuery camera_query;
	ECSQuery object_query;

	object_query.addShared(_buffer_count, true);
	object_query.addShared(_raster_states);
	object_query.addShared(_materials);
	object_query.addShared(_models);

	object_query.add<Position>(_position);
	object_query.add<Rotation>(_rotation);
	object_query.add<Scale>(_scale);

	camera_query.add<Position>(_camera_position);
	camera_query.add<Rotation>(_camera_rotation);
	camera_query.add<Camera>(_camera);

	// Add callbacks for adding/removing buffers.
	auto object_add_func = Gaff::MemberFunc(this, &RenderCommandSystem::newObjectArchetype);
	auto object_remove_func = Gaff::MemberFunc(this, &RenderCommandSystem::removedObjectArchetype);

	object_query.addArchetypeCallbacks(std::move(object_add_func), std::move(object_remove_func));

	_render_mgr = &GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	_res_mgr = &GetApp().getManagerTFast<ResourceManager>();
	_ecs_mgr = &GetApp().getManagerTFast<ECSManager>();
	_job_pool = &GetApp().getJobPool();

	_ecs_mgr->registerQuery(std::move(object_query));
	_ecs_mgr->registerQuery(std::move(camera_query));

	return true;
}

void RenderCommandSystem::update(uintptr_t thread_id_int)
{
	const int32_t num_cameras = static_cast<int32_t>(_camera.size());

	_device_job_data_cache.clear();

	for (int32_t camera_index = 0; camera_index < num_cameras; ++camera_index) {
		_ecs_mgr->iterate<Camera>(
			_camera[camera_index],
			[&](EntityID, const Camera& camera) -> void
			{
				const auto devices = _render_mgr->getDevicesByTag(camera.device_tag);

				if (!devices) {
					// $TODO: Log error.
					return;
				}

				for (int32_t i = 0; i < static_cast<int32_t>(devices->size()); ++i) {
					auto it = Gaff::Find(
						_device_job_data_cache,
						devices->at(i),
						[](const DeviceJobData& lhs, const Gleam::IRenderDevice* device) -> bool
						{
							return lhs.device == device;
						});

					if (it == _device_job_data_cache.end()) {
						DeviceJobData& job_data = _device_job_data_cache.emplace_back();
						job_data.rcs = this;
						job_data.device = devices->at(i);

					} else {
						it->render_job_data_cache.clear();
						it->job_data_cache.clear();
					}
				}
			}
		);
	}

	// Do a second loop so that none of our _device_job_data_cache pointers get invalidated.
	_job_data_cache.resize(_device_job_data_cache.size());

	for (int32_t i = 0; i < static_cast<int32_t>(_device_job_data_cache.size()); ++i) {
		_job_data_cache[i].job_data = &_device_job_data_cache[i];
		_job_data_cache[i].job_func = DeviceJob;
	}

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

	_job_pool->addJobs(_job_data_cache.data(), static_cast<int32_t>(_job_data_cache.size()), _job_counter);
	_job_pool->helpWhileWaiting(thread_id, _job_counter);

	_cache_index = (_cache_index + 1) % 2;
}

void RenderCommandSystem::newObjectArchetype(const ECSArchetype& archetype)
{
	auto* const material = _materials.back();
	auto* const model = _models.back()->value.get();

	Vector<IResource*> resources{ material->material.get(), model };
	AddResourcesToWaitList(material->textures_vertex, resources);
	AddResourcesToWaitList(material->textures_pixel, resources);
	AddResourcesToWaitList(material->textures_domain, resources);
	AddResourcesToWaitList(material->textures_geometry, resources);
	AddResourcesToWaitList(material->textures_hull, resources);
	AddResourcesToWaitList(material->samplers_vertex, resources);
	AddResourcesToWaitList(material->samplers_pixel, resources);
	AddResourcesToWaitList(material->samplers_domain, resources);
	AddResourcesToWaitList(material->samplers_geometry, resources);
	AddResourcesToWaitList(material->samplers_hull, resources);

	const int32_t buffer_count = (_buffer_count.empty() || !_buffer_count.back()) ? 64 : _buffer_count.back()->value;

	_res_mgr->registerCallback(resources, Gaff::Func<void (const Vector<IResource*>&)>([this, &archetype, material, buffer_count](const Vector<IResource*>&) -> void
	{
		const U8String pb_name(U8String::CtorSprintf(), ProgramBuffersFormat, archetype.getHash());

		InstanceData& instance_data = _instance_data.emplace_back();
		instance_data.program_buffers = _res_mgr->createResourceT<ProgramBuffersResource>(pb_name.data());
		instance_data.buffer_instance_count = buffer_count;

		if (!instance_data.program_buffers->createProgramBuffers(material->material->getDevices())) {
			// $TODO: Log error
			return;
		}

		processNewArchetypeMaterial(instance_data, *material, archetype);
	}));
}

void RenderCommandSystem::removedObjectArchetype(int32_t index)
{
	_instance_data.erase(_instance_data.begin() + index);
}

void RenderCommandSystem::processNewArchetypeMaterial(
	InstanceData& instance_data,
	const Material& material,
	const ECSArchetype& archetype)
{
	const auto devices = material.material->getDevices();

	if (devices.empty()) {
		// $TODO: Log error.
		return;
	}

	for (int32_t i = 0; i < static_cast<int32_t>(Gleam::IShader::Type::PipelineCount); ++i) {
		const Gleam::IShader::Type shader_type = static_cast<Gleam::IShader::Type>(i);

		addStructuredBuffersSRVs(instance_data, material, archetype, devices, shader_type);

		for (Gleam::IRenderDevice* rd : devices) {
			const Gleam::IProgram* const program = material.material->getProgram(*rd);
			const Gleam::IShader* const shader = (program) ? program->getAttachedShader(shader_type) : nullptr;

			if (!shader) {
				continue;
			}

			const Gleam::ShaderReflection shader_refl = shader->getReflectionData();
			InstanceData::VarMap shader_vars{ ProxyAllocator("Graphics") };

			addTextureSRVs(material, shader_refl, shader_vars, *rd, shader_type);

			auto& var_srvs = instance_data.pipeline_data[i].srv_vars[rd];
			var_srvs = std::move(shader_vars);

			if (shader_type == Gleam::IShader::Type::Vertex) {
				for (const auto& sb_refl : shader_refl.structured_buffers) {
					const auto it = Gaff::Find(sb_refl.vars, k_mtp_mat_name, [](const Gleam::VarReflection& lhs, const char* rhs) -> bool { return lhs.name == rhs; });

					if (it != sb_refl.vars.end()) {
						instance_data.model_to_proj_offset = static_cast<int32_t>(it->start_offset);
						instance_data.instance_data = &instance_data.pipeline_data[i].buffer_vars[HashString32<>(sb_refl.name.data())];
					}
				}
			}

			Gleam::IProgramBuffers* const pb = instance_data.program_buffers->getProgramBuffer(*rd);

			if (!pb) {
				continue;
			}

			addConstantBuffers(shader_refl, *pb, *rd, archetype, devices, shader_type);
			addSamplers(material, shader_refl, *pb, *rd, shader_type);

			for (const Gleam::U8String& var_name : shader_refl.var_decl_order) {
				const auto it = var_srvs.find(HashString32<>(var_name.data()));

				if (it != var_srvs.end()) {
					pb->addResourceView(shader_type, it->second.get());

				} else {
					auto& buffers = instance_data.pipeline_data[i].buffer_vars;
					const auto it_buf = buffers.find(HashString32<>(var_name.data()));

					if (it_buf != buffers.end()) {
						pb->addResourceView(shader_type, it_buf->second.pages[0].srv_map[rd].get());
					} else {
						// $TODO: Log error.
					}
				}
			}
		}
	}
}

void RenderCommandSystem::addStructuredBuffersSRVs(
	InstanceData& instance_data,
	const Material& material,
	const ECSArchetype& archetype,
	const Vector<Gleam::IRenderDevice*>& devices,
	Gleam::IShader::Type shader_type)
{
	const Gleam::IProgram* const program = material.material->getProgram(*devices[0]);
	const Gleam::IShader* const shader = (program) ? program->getAttachedShader(shader_type) : nullptr;

	if (!shader) {
		return;
	}

	const Gleam::ShaderReflection refl = shader->getReflectionData();
	InstanceData::BufferVarMap var_map{ ProxyAllocator("Graphics") };

	for (const auto& sb_refl : refl.structured_buffers) {
		const Gleam::IBuffer::Settings settings = {
			nullptr,
			sb_refl.size_bytes * static_cast<size_t>(instance_data.buffer_instance_count), // size
			static_cast<int32_t>(sb_refl.size_bytes), // stride
			static_cast<int32_t>(sb_refl.size_bytes), // elem_size
			Gleam::IBuffer::Type::StructuredData,
			Gleam::IBuffer::MapType::Write,
			true
		};

		U8String b_name(U8String::CtorSprintf(), StructuredBufferFormat, sb_refl.name.data(), archetype.getHash(), 0);
		BufferResourcePtr buffer = _res_mgr->createResourceT<BufferResource>(b_name.data());

		if (!buffer->createBuffer(devices, settings)) {
			// $TODO: Log error.
			continue;
		}

		auto& buffer_data = var_map[HashString32<>(sb_refl.name.data())];
		var_map.set_allocator(ProxyAllocator("Graphics"));

		const auto it = Gaff::Find(refl.var_decl_order, sb_refl.name);
		buffer_data.srv_index = (it == refl.var_decl_order.end()) ? -1 : static_cast<int32_t>(eastl::distance(it, refl.var_decl_order.end()) - 1);
		buffer_data.buffer_string = std::move(b_name);
		auto& page = buffer_data.pages.emplace_back();

		for (Gleam::IRenderDevice* rd : devices) {
			Gleam::IShaderResourceView* const srv = _render_mgr->createShaderResourceView();

			if (!srv->init(*rd, buffer->getBuffer(*rd))) {
				// $TODO: Log error and use error texture.
				SHIB_FREET(srv, GetAllocator());
				return;
			}

			page.srv_map[rd].reset(srv);
		}

		page.buffer = std::move(buffer);
	}

	instance_data.pipeline_data[static_cast<int32_t>(shader_type)].buffer_vars = std::move(var_map);
}

void RenderCommandSystem::addTextureSRVs(
	const Material& material,
	const Gleam::ShaderReflection& refl,
	InstanceData::VarMap& var_map,
	Gleam::IRenderDevice& rd,
	Gleam::IShader::Type shader_type)
{
	const Material::TextureMap* const texture_map = GetTextureMap(material, shader_type);
	GAFF_ASSERT(texture_map);

	for (const Gleam::U8String& texture_name : refl.textures) {
		const auto it = texture_map->find(U8String(texture_name));

		if (it == texture_map->end()) {
			// $TODO: Use error texture.
			continue;
		}

		Gleam::IShaderResourceView* const srv = _render_mgr->createShaderResourceView();
		const Gleam::ITexture* const texture = it->second->getTexture(rd);

		if (!srv->init(rd, texture)) {
			// $TODO: Log error and use error texture.
			SHIB_FREET(srv, GetAllocator());
			continue;
		}

		var_map[HashString32<>(texture_name.data())].reset(srv);
	}
}

void RenderCommandSystem::addConstantBuffers(
	const Gleam::ShaderReflection& refl,
	Gleam::IProgramBuffers& pb,
	Gleam::IRenderDevice& rd,
	const ECSArchetype& archetype,
	const Vector<Gleam::IRenderDevice*>& devices,
	Gleam::IShader::Type shader_type)
{
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

		const U8String b_name(U8String::CtorSprintf(), ConstBufferFormat, const_buf_refl.name.data(), archetype.getHash());
		BufferResourcePtr buffer = _res_mgr->createResourceT<BufferResource>(b_name.data());

		if (!buffer->createBuffer(devices, settings)) {
			// $TODO: Log error.
			continue;
		}

		pb.addConstantBuffer(shader_type, buffer->getBuffer(rd));
	}
}

void RenderCommandSystem::addSamplers(
	const Material& material,
	const Gleam::ShaderReflection& refl,
	Gleam::IProgramBuffers& pb,
	Gleam::IRenderDevice& rd,
	Gleam::IShader::Type shader_type)
{
	SamplerStateResourcePtr& default_sampler_res = _render_mgr->getDefaultSamplerState();
	Gleam::ISamplerState* const default_sampler = default_sampler_res->getSamplerState(rd);
	const Material::SamplerMap* const sampler_map = GetSamplereMap(material, shader_type);

	GAFF_ASSERT(sampler_map);

	for (const Gleam::U8String& sampler_name : refl.samplers) {
		const U8String key(sampler_name.data());
		const auto it = sampler_map->find(key);

		if (it == sampler_map->end()) {
			pb.addSamplerState(shader_type, default_sampler);

		} else {
			Gleam::ISamplerState* const sampler = it->second->getSamplerState(rd);

			if (sampler) {
				pb.addSamplerState(shader_type, sampler);
			} else {
				// $TODO: Log error.
				pb.addSamplerState(shader_type, default_sampler);
			}
		}
	}
}

void RenderCommandSystem::GenerateCommandListJob(uintptr_t thread_id_int, void* data)
{
	RenderJobData& job_data = *reinterpret_cast<RenderJobData*>(data);

	if (!job_data.device) {
		// $TODO: Log error
		return;
	}

	if (!job_data.cmd_list) {
		// $TODO: Log error
		return;
	}

	if (!job_data.target) {
		// $TODO: Log error
		return;
	}

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	Gleam::IRenderDevice& owning_device = *job_data.device;
	Gleam::IRenderDevice* const deferred_device = job_data.rcs->_render_mgr->getDeferredDevice(owning_device, thread_id);

	const int32_t num_meshes = job_data.rcs->_models[job_data.index]->value->getNumMeshes();

	if (!num_meshes) {
		// $TODO: LOG ERROR
		return;
	}

	Gleam::IProgram* const program = job_data.rcs->_materials[job_data.index]->material->getProgram(owning_device);

	if (!program) {
		// $TODO: LOG ERROR
		return;
	}

	Gleam::ILayout* const layout = job_data.rcs->_materials[job_data.index]->material->getLayout(owning_device);

	if (!layout) {
		// $TODO: LOG ERROR
		return;
	}

	Gleam::IRasterState* const raster_state = job_data.rcs->_raster_states[job_data.index]->value->getRasterState(owning_device);

	if (!raster_state) {
		// $TODO: LOG ERROR
		return;
	}

	InstanceData& instance_data = job_data.rcs->_instance_data[job_data.index];

	if (!instance_data.instance_data) {
		// $TODO: LOG ERROR
		return;
	}

	job_data.target->bind(*deferred_device);
	raster_state->bind(*deferred_device);
	program->bind(*deferred_device);
	layout->bind(*deferred_device);

	const int32_t stride = instance_data.instance_data->pages[0].buffer->getBuffer(owning_device)->getStride();
	const int32_t object_count = job_data.rcs->_ecs_mgr->getNumEntities(job_data.rcs->_position[job_data.index]);
	const int32_t num_pages = (object_count / instance_data.buffer_instance_count) + 1;
	int32_t object_index = 0;

	// Make sure we have enough buffers for the number of object's we are rendering.
	if (int32_t start_index = static_cast<int32_t>(instance_data.instance_data->pages.size()); start_index < num_pages) {
		instance_data.instance_data->pages.resize(static_cast<size_t>(num_pages));
		const auto devices = job_data.rcs->_materials[job_data.index]->material->getDevices();


		for (; start_index < num_pages; ++start_index) {
			const size_t colon_index = instance_data.instance_data->buffer_string.find_last_of(':');

			instance_data.instance_data->buffer_string.erase(colon_index + 1);
			instance_data.instance_data->buffer_string.append_sprintf("%i", start_index);

			BufferResourcePtr buffer = job_data.rcs->_res_mgr->createResourceT<BufferResource>(
				instance_data.instance_data->buffer_string.data()
			);

			for (Gleam::IRenderDevice* rd : devices) {
				Gleam::IShaderResourceView* const srv = job_data.rcs->_render_mgr->createShaderResourceView();

				if (!srv->init(*rd, buffer->getBuffer(*rd))) {
					// $TODO: Log error and use error texture.
					SHIB_FREET(srv, GetAllocator());
					return;
				}

				instance_data.instance_data->pages[start_index].srv_map[rd].reset(srv);
			}

			instance_data.instance_data->pages[start_index].buffer = std::move(buffer);
		}
	}

	Vector<void*> buffer_cache(instance_data.instance_data->pages.size(), nullptr, ProxyAllocator("Graphics"));

	for (int32_t j = 0; j < static_cast<int32_t>(buffer_cache.size()); ++j) {
		buffer_cache[j] = instance_data.instance_data->pages[j].buffer->getBuffer(owning_device)->map(*deferred_device);
	}

	const Gleam::Vec3& centering_vector = job_data.rcs->_models[job_data.index]->value->getCenteringVector();

	job_data.rcs->_ecs_mgr->iterate<Position, Rotation, Scale>(
		job_data.rcs->_position[job_data.index], job_data.rcs->_rotation[job_data.index], job_data.rcs->_scale[job_data.index],
		[&](EntityID /*id*/, const Position& obj_pos, const Rotation& obj_rot, const Scale& obj_scale) -> void
		{
			const Gleam::Vec3 euler_angles = obj_rot.value * Gaff::TurnsToRad;

			Gleam::Mat4x4 transform = glm::yawPitchRoll(euler_angles.y, euler_angles.x, euler_angles.z);
			transform[3] = Gleam::Vec4(obj_pos.value, 1.0f);
			transform = glm::scale(transform, obj_scale.value);

			const int32_t instance_index = object_index % instance_data.buffer_instance_count;
			const int32_t buffer_index = object_index / instance_data.buffer_instance_count;
			void* const buffer = buffer_cache[buffer_index];

			// Write to instance buffer.
			Gleam::Mat4x4 center_object = glm::identity<Gleam::Mat4x4>();
			center_object[3] = Gleam::Vec4(centering_vector, 1.0f);

			Gleam::Mat4x4* const matrix = reinterpret_cast<Gleam::Mat4x4*>(reinterpret_cast<int8_t*>(buffer) + (stride * instance_index) + instance_data.model_to_proj_offset);
			*matrix = job_data.view_projection * transform * center_object;

			++object_index;
		}
	);

	for (const auto& page : instance_data.instance_data->pages) {
		page.buffer->getBuffer(owning_device)->unmap(*deferred_device);
	}


	// Iterate over all the instance buffers and set their resource views and render.
	Gleam::IProgramBuffers* const pb = instance_data.program_buffers->getProgramBuffer(owning_device);

	for (int32_t mesh_index = 0; mesh_index < num_meshes; ++mesh_index) {
		Gleam::IMesh* const mesh = job_data.rcs->_models[job_data.index]->value->getMesh(mesh_index)->getMesh(owning_device);

		if (!mesh) {
			continue;
		}

		for (int32_t k = 0; k < num_pages; ++k) {
			for (int32_t j = 0; j < static_cast<int32_t>(Gleam::IShader::Type::PipelineCount); ++j) {
				InstanceData::BufferVarMap& var_map = instance_data.pipeline_data[j].buffer_vars;

				for (auto& id : var_map) {
					if (id.second.srv_index < 0) {
						continue;
					}

					pb->setResourceView(
						static_cast<Gleam::IShader::Type>(j),
						id.second.srv_index,
						id.second.pages[k].srv_map[&owning_device].get()
					);
				}
			}

			pb->bind(*deferred_device);
			mesh->renderInstanced(*deferred_device, object_count);
		}
	}

	if (!deferred_device->finishCommandList(*job_data.cmd_list)) {
		// $TODO: Log error.
	}
}

void RenderCommandSystem::DeviceJob(uintptr_t thread_id_int, void* data)
{
	DeviceJobData& job_data = *reinterpret_cast<DeviceJobData*>(data);
	const int32_t num_objects = static_cast<int32_t>(job_data.rcs->_instance_data.size());
	const int32_t num_cameras = static_cast<int32_t>(job_data.rcs->_camera.size());
	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

	Gleam::IRenderDevice& device = *job_data.device;
	job_data.render_job_data_cache.clear();

	auto& render_cmds = job_data.rcs->_render_mgr->getRenderCommands(
		*job_data.device,
		RenderManagerBase::RenderOrder::InWorldWithDepthTest,
		job_data.rcs->_cache_index
	);

	for (int32_t camera_index = 0; camera_index < num_cameras; ++camera_index) {
		job_data.rcs->_ecs_mgr->iterate<Position, Rotation, Camera>(
			job_data.rcs->_camera_position[camera_index],
			job_data.rcs->_camera_rotation[camera_index],
			job_data.rcs->_camera[camera_index],
			[&](EntityID id, const Position& cam_pos, const Rotation& cam_rot, const Camera& camera) -> void
			{
				const auto devices = job_data.rcs->_render_mgr->getDevicesByTag(camera.device_tag);

				if (!devices || Gaff::Find(*devices, &device) == devices->end()) {
					return;
				}

				const auto* const g_buffer = job_data.rcs->_render_mgr->getGBuffer(id, device);

				if (!g_buffer) {
					// $TODO: Log error.
					return;
				}

				Gleam::IRenderTarget* const render_target = g_buffer->render_target.get();

				if (num_objects > 0) {
					const Gleam::IVec2& size = render_target->getSize();
					const Gleam::Mat4x4 projection = glm::perspectiveFovLH(
						camera.GetVerticalFOV() * Gaff::TurnsToRad,
						static_cast<float>(size.x),
						static_cast<float>(size.y),
						camera.z_near, camera.z_far
					);

					const Gleam::Vec3 euler_angles = cam_rot.value * Gaff::TurnsToRad;
					Gleam::Mat4x4 camera_transform = glm::yawPitchRoll(euler_angles.y, euler_angles.x, euler_angles.z);
					camera_transform[3] = Gleam::Vec4(cam_pos.value, 1.0f);

					const Gleam::Mat4x4 final_camera = projection * glm::inverse(camera_transform);

					for (int32_t i = 0; i < num_objects; ++i) {
						const int32_t cache_index = job_data.rcs->_cache_index;
						const int32_t cmd_list_end = job_data.rcs->_cmd_list_end[cache_index];
						Gleam::ICommandList* cmd_list = nullptr;

						// Grab command list from the cache.
						if (job_data.rcs->_cmd_lists[cache_index].size() > cmd_list_end) {
							cmd_list = job_data.rcs->_cmd_lists[cache_index][cmd_list_end].get();

						// Cache is full, create a new one and add it to the cache.
						} else {
							cmd_list = job_data.rcs->_render_mgr->createCommandList();
							job_data.rcs->_cmd_lists[cache_index].emplace_back(cmd_list);
						}

						++job_data.rcs->_cmd_list_end[cache_index];

						render_cmds.lock.Lock();
						auto& cmd = render_cmds.command_list.emplace_back();
						cmd.cmd_list.reset(cmd_list);
						cmd.owns_command = false;
						render_cmds.lock.Unlock();

						RenderJobData& render_data = job_data.render_job_data_cache.emplace_back();
						render_data.rcs = job_data.rcs;
						render_data.index = i;
						render_data.cmd_list = cmd_list;
						render_data.device = &device;
						render_data.target = render_target;
						render_data.view_projection = final_camera;
					}
				}
			}
		);
	}

	// Do a second loop so that none of our job_data.render_job_data_cache pointers get invalidated.
	if (job_data.job_data_cache.size() != job_data.render_job_data_cache.size()) {
		job_data.job_data_cache.resize(job_data.render_job_data_cache.size());

		for (int32_t i = 0; i < static_cast<int32_t>(job_data.render_job_data_cache.size()); ++i) {
			job_data.job_data_cache[i].job_data = &job_data.render_job_data_cache[i];
			job_data.job_data_cache[i].job_func = GenerateCommandListJob;
		}
	}

	job_data.rcs->_job_pool->addJobs(job_data.job_data_cache.data(), static_cast<int32_t>(job_data.job_data_cache.size()), job_data.job_counter);

	// $TODO: If the cache is significantly higher than the current usage, shrink the cache.

	job_data.rcs->_job_pool->helpWhileWaiting(thread_id, job_data.job_counter);

	// Clear this for next run.
	job_data.rcs->_cmd_list_end[job_data.rcs->_cache_index] = 0;
}

NS_END
