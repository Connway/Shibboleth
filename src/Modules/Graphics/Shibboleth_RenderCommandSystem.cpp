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

#include "Shibboleth_RenderCommandSystem.h"
#include "Shibboleth_RenderManagerBase.h"
#include "Shibboleth_CameraComponent.h"
#include <Shibboleth_ECSComponentCommon.h>
#include <Shibboleth_ECSManager.h>

SHIB_REFLECTION_DEFINE(RenderCommandSystem)

NS_SHIBBOLETH

static constexpr const char* k_mtp_mat_name = "_model_to_proj_matrix";

static const Material::TextureMap* GetTextureMap(const Material& material, Gleam::IShader::ShaderType shader_type)
{
	const Material::TextureMap* texture_map = nullptr;

	switch (shader_type) {
		case Gleam::IShader::SHADER_VERTEX:
			texture_map = &material.textures_vertex;
			break;

		case Gleam::IShader::SHADER_PIXEL:
			texture_map = &material.textures_pixel;
			break;

		case Gleam::IShader::SHADER_DOMAIN:
			texture_map = &material.textures_domain;
			break;

		case Gleam::IShader::SHADER_GEOMETRY:
			texture_map = &material.textures_geometry;
			break;

		case Gleam::IShader::SHADER_HULL:
			texture_map = &material.textures_hull;
			break;

		default:
			break;
	}

	return texture_map;
}

static const Material::SamplerMap* GetSamplereMap(const Material& material, Gleam::IShader::ShaderType shader_type)
{
	const Material::SamplerMap* sampler_map = nullptr;

	switch (shader_type) {
	case Gleam::IShader::SHADER_VERTEX:
		sampler_map = &material.samplers_vertex;
		break;

	case Gleam::IShader::SHADER_PIXEL:
		sampler_map = &material.samplers_pixel;
		break;

	case Gleam::IShader::SHADER_DOMAIN:
		sampler_map = &material.samplers_domain;
		break;

	case Gleam::IShader::SHADER_GEOMETRY:
		sampler_map = &material.samplers_geometry;
		break;

	case Gleam::IShader::SHADER_HULL:
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

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(RenderCommandSystem)
	.BASE(ISystem)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(RenderCommandSystem)

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
	auto add_func = Gaff::MemberFunc(this, &RenderCommandSystem::newArchetype);
	auto remove_func = Gaff::MemberFunc(this, &RenderCommandSystem::removedArchetype);

	object_query.addArchetypeCallbacks(std::move(add_func), std::move(remove_func));

	_render_mgr = &GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	_res_mgr = &GetApp().getManagerTFast<ResourceManager>();
	_ecs_mgr = &GetApp().getManagerTFast<ECSManager>();
	_ecs_mgr->registerQuery(std::move(object_query));
	_ecs_mgr->registerQuery(std::move(camera_query));

	return true;
}

void RenderCommandSystem::update()
{
	// All arrays should be same size.
	const int32_t num_objects = static_cast<int32_t>(_instance_data.size());
	const int32_t num_cameras = static_cast<int32_t>(_camera.size());

	_render_job_data_cache.resize(num_objects * num_cameras);
	_job_data_cache.resize(num_objects * num_cameras);

	for (int32_t camera_index = 0; camera_index < num_cameras; ++camera_index) {
		_ecs_mgr->iterate<Position, Rotation, Camera>([&](EntityID, const Position& cam_pos, const Rotation& cam_rot, const Camera& camera) -> void
		{
			// $TODO: Change this to render to a deferred render context instead.
			const Gleam::IWindow* const window = _render_mgr->getWindow(camera.display_tag);

			if (!window) {
				return;
			}

			const auto devices = _render_mgr->getDevicesByTag(camera.display_tag);

			if (!devices || devices->size() != 1) {
				// $TODO: Log error.
				return;
			}

			Gleam::IRenderOutput* const output = _render_mgr->getOutput(camera.display_tag);

			if (!output) {
				// $TODO: Log error.
				return;
			}

			Gleam::IRenderDevice& device = *devices->front();

			output->getRenderTarget().bind(device);
			output->getRenderTarget().clear(device, Gleam::IRenderTarget::CLEAR_ALL, 1.0f, 0, Gleam::COLOR_BLACK);
			device.frameBegin(*output);

			const glm::mat4x4 projection = glm::perspectiveFovLH(
				camera.GetVerticalFOV(),
				static_cast<float>(window->getWidth()),
				static_cast<float>(window->getHeight()),
				camera.z_near, camera.z_far
			);

			const glm::mat4x4 camera_transform = glm::translate(glm::mat4_cast(cam_rot.value), cam_pos.value);
			const glm::mat4x4 final_camera = projection * camera_transform;

			// $TODO: Spawn a job per archetype we are processing.
			for (int32_t i = 0; i < num_objects; ++i) {
				const int32_t cache_index = camera_index * num_objects + i;
				auto& deferred_data = _instance_data[i].deferred_data[&device];

				_render_job_data_cache[cache_index] = RenderJobData{
					this,
					i,
					deferred_data.command_list.get(),
					//deferred_data.deferred_context.get(),
					&device,
					final_camera
				};

				_job_data_cache[cache_index] = Gaff::JobData{
					RenderCommandSystem::GenerateCommandListJob,
					&_render_job_data_cache[cache_index]
				};

				GenerateCommandListJob(&_render_job_data_cache[cache_index]);

				//device.executeCommandList(*deferred_data.command_list);
			}

			device.frameEnd(*output);
		},
		_camera_position[camera_index], _camera_rotation[camera_index], _camera[camera_index]);
	}
}

void RenderCommandSystem::newArchetype(const ECSArchetype& archetype)
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

	const int32_t buffer_count = (_buffer_count.back()) ? _buffer_count.back()->value : 64;

	_res_mgr->registerCallback(resources, Gaff::Func<void (const Vector<IResource*>&)>([this, &archetype, material, buffer_count](const Vector<IResource*>&) -> void {
		const U8String pb_name(U8String::CtorSprintf(), "RenderCommandSystem:ProgramBuffers:%llu", archetype.getHash());

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

void RenderCommandSystem::removedArchetype(int32_t index)
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

	for (Gleam::IRenderDevice* rd : devices) {
		auto& deferred_data = instance_data.deferred_data[rd];
		deferred_data.deferred_context.reset(rd->createDeferredRenderDevice());
		deferred_data.command_list.reset(_render_mgr->createCommandList());
	}

	for (int32_t i = 0; i < static_cast<int32_t>(Gleam::IShader::SHADER_PIPELINE_COUNT); ++i) {
		const Gleam::IShader::ShaderType shader_type = static_cast<Gleam::IShader::ShaderType>(i);

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

			if (shader_type == Gleam::IShader::SHADER_VERTEX) {
				for (const auto& sb_refl : shader_refl.structured_buffers) {
					const auto it = Gaff::Find(sb_refl.vars, k_mtp_mat_name, [](const Gleam::VarReflection& lhs, const char* rhs) -> bool { return lhs.name == rhs; });

					if (it != sb_refl.vars.end()) {
						instance_data.model_to_proj_offset = static_cast<int32_t>(it->start_offset);
						instance_data.instance_data = &instance_data.pipeline_data[i].buffer_vars[HashString32(sb_refl.name)];
					}
				}
			}

			Gleam::IProgramBuffers* const pb = instance_data.program_buffers->getProgramBuffer(*rd);

			if (!pb) {
				continue;
			}

			addSamplers(material, shader_refl, *pb, *rd, shader_type);

			for (const Gleam::U8String& var_name : shader_refl.var_decl_order) {
				const auto it = var_srvs.find(HashString32(var_name.data()));

				if (it != var_srvs.end()) {
					pb->addResourceView(shader_type, it->second.get());

				} else {
					auto& buffers = instance_data.pipeline_data[i].buffer_vars;
					const auto it_buf = buffers.find(HashString32(var_name.data()));

					if (it_buf != buffers.end()) {
						pb->addResourceView(shader_type, it_buf->second.front().srv_map[rd].get());
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
	Gleam::IShader::ShaderType shader_type)
{
	const Gleam::IProgram* const program = material.material->getProgram(*devices[0]);
	const Gleam::IShader* const shader = (program) ? program->getAttachedShader(shader_type) : nullptr;

	if (!shader) {
		return;
	}

	const Gleam::ShaderReflection refl = shader->getReflectionData();
	InstanceData::BufferVarMap var_map{ ProxyAllocator("Graphics") };

	for (const auto& sb_refl : refl.structured_buffers) {
		const Gleam::IBuffer::BufferSettings settings = {
			nullptr,
			sb_refl.size_bytes * static_cast<size_t>(instance_data.buffer_instance_count), // size
			static_cast<int32_t>(sb_refl.size_bytes), // stride
			Gleam::IBuffer::BT_STRUCTURED_DATA,
			Gleam::IBuffer::MT_WRITE,
			true
		};

		const U8String b_name(U8String::CtorSprintf(), "RenderCommandSystem:Buffer:%s:%llu:0", sb_refl.name.data(), archetype.getHash());
		BufferResourcePtr buffer = _res_mgr->createResourceT<BufferResource>(b_name.data());

		if (!buffer->createBuffer(devices, settings)) {
			// $TODO: Log error.
			continue;
		}

		auto& instance_buffers = var_map[HashString32(sb_refl.name)];
		var_map.set_allocator(ProxyAllocator("Graphics"));

		for (Gleam::IRenderDevice* rd : devices) {
			Gleam::IShaderResourceView* const srv = _render_mgr->createShaderResourceView();

			if (!srv->init(*rd, buffer->getBuffer(*rd))) {
				// $TODO: Log error and use error texture.
				SHIB_FREET(srv, GetAllocator());
				return;
			}

			const auto it = Gaff::Find(refl.var_decl_order, sb_refl.name);

			InstanceData::InstanceBufferData buffer_data;
			buffer_data.srv_index = (it == refl.var_decl_order.end()) ? -1 : static_cast<int32_t>(eastl::distance(it, refl.var_decl_order.end()) - 1);
			buffer_data.srv_map[rd].reset(srv);
			buffer_data.buffer = buffer;

			instance_buffers.emplace_back(std::move(buffer_data));
		}
	}

	instance_data.pipeline_data[shader_type].buffer_vars = std::move(var_map);
}

void RenderCommandSystem::addTextureSRVs(
	const Material& material,
	const Gleam::ShaderReflection& refl,
	InstanceData::VarMap& var_map,
	Gleam::IRenderDevice& rd,
	Gleam::IShader::ShaderType shader_type)
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

		var_map[HashString32(texture_name)].reset(srv);
	}
}

void RenderCommandSystem::addSamplers(
	const Material& material,
	const Gleam::ShaderReflection& refl,
	Gleam::IProgramBuffers& pb,
	Gleam::IRenderDevice& rd,
	Gleam::IShader::ShaderType shader_type)
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

void RenderCommandSystem::GenerateCommandListJob(void* data)
{
	const RenderJobData& job_data = *reinterpret_cast<RenderJobData*>(data);

	if (!job_data.device) {
		// $TODO: Log error
		return;
	}

	if (!job_data.cmd_list) {
		// $TODO: Log error
		return;
	}

	//Gleam::IRenderDevice& owning_device = *job_data.device->getOwningDevice();
	Gleam::IRenderDevice& owning_device = *job_data.device;

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

	raster_state->bind(*job_data.device);
	program->bind(*job_data.device);
	layout->bind(*job_data.device);

	Vector<void*> _buffer_cache(instance_data.instance_data->size(), nullptr, ProxyAllocator("Graphics"));
	const int32_t stride = instance_data.instance_data->at(0).buffer->getBuffer(owning_device)->getStride();
	int32_t object_count = 0;

	for (int32_t j = 0; j < static_cast<int32_t>(_buffer_cache.size()); ++j) {
		_buffer_cache[j] = instance_data.instance_data->at(j).buffer->getBuffer(owning_device)->map(*job_data.device);
	}

	//float scalar = 1.0f;

	job_data.rcs->_ecs_mgr->iterate<Position, Rotation, Scale>([&](EntityID /*id*/, const Position& obj_pos, const Rotation& obj_rot, const Scale& obj_scale) -> void
	{
		//const glm::quat rot = glm::rotate(obj_rot.value, 0.001f * scalar, glm::vec3(0.0f, 1.0f, 0.0f));
		//Rotation::Set(*_ecs_mgr, id, Rotation{ rot });
		//scalar *= -1.0f;

		const glm::mat4x4 transform =
			glm::translate(glm::identity<glm::mat4x4>(), obj_pos.value) *
			//glm::mat4_cast(rot) *
			glm::mat4_cast(obj_rot.value) *
			glm::scale(glm::identity<glm::mat4x4>(), obj_scale.value);

		const int32_t instance_index = object_count % instance_data.buffer_instance_count;
		const int32_t buffer_index = object_count / instance_data.buffer_instance_count;
		void* const buffer = _buffer_cache[buffer_index];

		// Write to instance buffer.
		glm::mat4x4* const matrix = reinterpret_cast<glm::mat4x4*>(reinterpret_cast<int8_t*>(buffer) + (stride * instance_index) + instance_data.model_to_proj_offset);
		*matrix = job_data.view_projection * transform;

		++object_count;
	},
	job_data.rcs->_position[job_data.index], job_data.rcs->_rotation[job_data.index], job_data.rcs->_scale[job_data.index]);

	for (const auto& id : *instance_data.instance_data) {
		id.buffer->getBuffer(owning_device)->unmap(*job_data.device);
	}


	// Iterate over all the instance buffers and set their resource views and render.
	Gleam::IProgramBuffers* const pb = instance_data.program_buffers->getProgramBuffer(owning_device);
	const int32_t num_buffers = (object_count / instance_data.buffer_instance_count) + 1;

	for (int32_t mesh_index = 0; mesh_index < num_meshes; ++mesh_index) {
		Gleam::IMesh* const mesh = job_data.rcs->_models[job_data.index]->value->getMesh(mesh_index)->getMesh(owning_device);

		if (!mesh) {
			continue;
		}

		for (int32_t k = 0; k < num_buffers; ++k) {
			for (int32_t j = 0; j < Gleam::IShader::SHADER_PIPELINE_COUNT; ++j) {
				InstanceData::BufferVarMap& var_map = instance_data.pipeline_data[j].buffer_vars;

				for (auto& id : var_map) {
					if (id.second[k].srv_index < 0) {
						continue;
					}

					pb->setResourceView(
						static_cast<Gleam::IShader::ShaderType>(j),
						id.second[k].srv_index,
						id.second[k].srv_map[&owning_device].get()
					);
				}
			}

			pb->bind(*job_data.device);
			mesh->renderInstanced(*job_data.device, object_count);
		}
	}

	//job_data.device->finishCommandList(*job_data.cmd_list);
}

NS_END
