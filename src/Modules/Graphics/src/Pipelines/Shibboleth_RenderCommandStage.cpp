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

#include "Pipelines/Shibboleth_RenderCommandStage.h"
#include "Shibboleth_GraphicsLogging.h"
#include <Shibboleth_ITransformProvider.h>
#include <Shibboleth_ResourceManager.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_Mesh.h>
#include <Gaff_Function.h>

SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE(Shibboleth::RenderCommandStage, Shibboleth::IRenderPipelineStage)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(RenderCommandStage);

bool RenderCommandStage::init(RenderManager& render_mgr)
{
	_job_pool = &GetApp().getJobPool();
	_render_mgr = &render_mgr;

	return true;
}

void RenderCommandStage::update(uintptr_t thread_id_int)
{
	_device_job_data_cache.clear();

//	for (const CameraView& view : _render_mgr->getCameraViews()) {
//	}

	//	for (int32_t camera_index = 0; camera_index < num_cameras; ++camera_index) {
	//		_ecs_mgr->iterate<Camera>(
	//			_camera[camera_index],
	//			[&](ECSEntityID, const Camera& camera) -> void
	//			{
	//				const auto devices = _render_mgr->getDevicesByTag(camera.device_tag);
	//
	//				if (!devices) {
	//					// $TODO: Log error.
	//					return;
	//				}
	//
	//				for (int32_t i = 0; i < static_cast<int32_t>(devices->size()); ++i) {
	//					auto it = Gaff::Find(
	//						_device_job_data_cache,
	//						devices->at(i),
	//						[](const DeviceJobData& lhs, const Gleam::IRenderDevice* device) -> bool
	//						{
	//							return lhs.device == device;
	//						});
	//
	//					if (it == _device_job_data_cache.end()) {
	//						DeviceJobData& job_data = _device_job_data_cache.emplace_back();
	//						job_data.rcs = this;
	//						job_data.device = devices->at(i);
	//
	//					} else {
	//						it->render_job_data_cache.clear();
	//						it->job_data_cache.clear();
	//					}
	//				}
	//			}
	//		);
	//	}

	// Do a second loop so that none of our _device_job_data_cache pointers get invalidated.
	_job_data_cache.resize(_device_job_data_cache.size());

	for (int32_t i = 0; i < static_cast<int32_t>(_device_job_data_cache.size()); ++i) {
		_job_data_cache[i].job_data = &_device_job_data_cache[i];
		_job_data_cache[i].job_func = DeviceJob;
	}

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

	if (_job_data_cache.size() > 0) {
		_job_pool->addJobs(_job_data_cache.data(), static_cast<int32_t>(_job_data_cache.size()), _job_counter);
		_job_pool->helpWhileWaiting(thread_id, _job_counter);
	}
}

const RenderCommandData& RenderCommandStage::getRenderCommands(void) const
{
	return _render_commands;
}

void RenderCommandStage::GenerateCommandListJob(uintptr_t thread_id_int, void* data)
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
	Gleam::RenderDevice& owning_device = *job_data.device;
	Gleam::RenderDevice* const deferred_device = job_data.rcs->_render_mgr->getDeferredDevice(owning_device, thread_id);

	const int32_t num_meshes = job_data.rcs->_models[job_data.index]->value->getNumMeshes();

	if (!num_meshes) {
		// $TODO: LOG ERROR
		return;
	}

	Gleam::Program* const program = job_data.rcs->_materials[job_data.index]->material->getProgram(owning_device);

	if (!program) {
		// $TODO: LOG ERROR
		return;
	}

	Gleam::Layout* const layout = job_data.rcs->_materials[job_data.index]->material->getLayout(owning_device);

	if (!layout) {
		// $TODO: LOG ERROR
		return;
	}

	Gleam::RasterState* const raster_state = job_data.rcs->_raster_states[job_data.index]->value->getRasterState(owning_device);

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
			const size_t colon_index = instance_data.instance_data->buffer_string.rfind(':');

			instance_data.instance_data->buffer_string.erase(colon_index + 1);
			instance_data.instance_data->buffer_string.append_sprintf(u8"%i", start_index);

			BufferResourcePtr buffer = job_data.rcs->_resource_mgr->createResourceT<BufferResource>(
				instance_data.instance_data->buffer_string.data()
			);

			for (Gleam::RenderDevice* rd : devices) {
				Gleam::ShaderResourceView* const srv = job_data.rcs->_render_mgr->createShaderResourceView();

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

	Vector<void*> buffer_cache(instance_data.instance_data->pages.size(), nullptr, GRAPHICS_ALLOCATOR);

	for (int32_t j = 0; j < static_cast<int32_t>(buffer_cache.size()); ++j) {
		buffer_cache[j] = instance_data.instance_data->pages[j].buffer->getBuffer(owning_device)->map(*deferred_device);
	}

	const Gleam::Vec3& centering_vector = job_data.rcs->_models[job_data.index]->value->getCenteringVector();

	//	job_data.rcs->_ecs_mgr->iterate<Position, Rotation, Scale>(
	//		job_data.rcs->_position[job_data.index], job_data.rcs->_rotation[job_data.index], job_data.rcs->_scale[job_data.index],
	//		[&](ECSEntityID /*id*/, const Position& obj_pos, const Rotation& obj_rot, const Scale& obj_scale) -> void
	//		{
	//			const Gleam::Vec3 euler_angles = obj_rot.value * Gaff::TurnsToRad;
	//
	//			Gleam::Mat4x4 transform = glm::yawPitchRoll(euler_angles.y, euler_angles.x, euler_angles.z);
	//			transform[3] = Gleam::Vec4(obj_pos.value, 1.0f);
	//			transform = glm::scale(transform, obj_scale.value);
	//
	//			const int32_t instance_index = object_index % instance_data.buffer_instance_count;
	//			const int32_t buffer_index = object_index / instance_data.buffer_instance_count;
	//			void* const buffer = buffer_cache[buffer_index];
	//
	//			// Write to instance buffer.
	//			Gleam::Mat4x4 center_object = glm::identity<Gleam::Mat4x4>();
	//			center_object[3] = Gleam::Vec4(centering_vector, 1.0f);
	//
	//			Gleam::Mat4x4* const matrix = reinterpret_cast<Gleam::Mat4x4*>(reinterpret_cast<int8_t*>(buffer) + (stride * instance_index) + instance_data.model_to_proj_offset);
	//			*matrix = job_data.view_projection * transform * center_object;
	//
	//			++object_index;
	//		}
	//	);

	for (const auto& page : instance_data.instance_data->pages) {
		page.buffer->getBuffer(owning_device)->unmap(*deferred_device);
	}


	// Iterate over all the instance buffers and set their resource views and render.
	Gleam::ProgramBuffers* const pb = instance_data.program_buffers->getProgramBuffer(owning_device);

	for (int32_t mesh_index = 0; mesh_index < num_meshes; ++mesh_index) {
		Gleam::Mesh* const mesh = job_data.rcs->_models[job_data.index]->value->getMesh(mesh_index)->getMesh(owning_device);

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

void RenderCommandStage::DeviceJob(uintptr_t thread_id_int, void* data)
{
	DeviceJobData& job_data = *reinterpret_cast<DeviceJobData*>(data);
	const int32_t num_objects = static_cast<int32_t>(job_data.rcs->_instance_data.size());
	//const int32_t num_cameras = static_cast<int32_t>(job_data.rcs->_camera.size());
	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

	const int32_t cache_index = job_data.rcs->_render_mgr->getRenderCacheIndex();

	Gleam::RenderDevice& device = *job_data.device;
	job_data.render_job_data_cache.clear();

	auto& render_cmds = job_data.rcs->_render_mgr->getRenderCommands(
		*job_data.device,
		RenderManager::RenderOrder::InWorldWithDepthTest,
		cache_index
	);

	//	for (int32_t camera_index = 0; camera_index < num_cameras; ++camera_index) {
	//		job_data.rcs->_ecs_mgr->iterate<Position, Rotation, Camera>(
	//			job_data.rcs->_camera_position[camera_index],
	//			job_data.rcs->_camera_rotation[camera_index],
	//			job_data.rcs->_camera[camera_index],
	//			[&](ECSEntityID id, const Position& cam_pos, const Rotation& cam_rot, const Camera& camera) -> void
	//			{
	//				const auto devices = job_data.rcs->_render_mgr->getDevicesByTag(camera.device_tag);
	//
	//				if (!devices || Gaff::Find(*devices, &device) == devices->end()) {
	//					return;
	//				}
	//
	//				const auto* const g_buffer = job_data.rcs->_render_mgr->getGBuffer(id, device);
	//
	//				if (!g_buffer) {
	//					// $TODO: Log error.
	//					return;
	//				}
	//
	//				Gleam::IRenderTarget* const render_target = g_buffer->render_target.get();
	//
	//				if (num_objects > 0) {
	//					const Gleam::IVec2& size = render_target->getSize();
	//					const Gleam::Mat4x4 projection = glm::perspectiveFovLH(
	//						camera.GetVerticalFOV() * Gaff::TurnsToRad,
	//						static_cast<float>(size.x),
	//						static_cast<float>(size.y),
	//						camera.z_near, camera.z_far
	//					);
	//
	//					const Gleam::Vec3 euler_angles = cam_rot.value * Gaff::TurnsToRad;
	//					Gleam::Mat4x4 camera_transform = glm::yawPitchRoll(euler_angles.y, euler_angles.x, euler_angles.z);
	//					camera_transform[3] = Gleam::Vec4(cam_pos.value, 1.0f);
	//
	//					const Gleam::Mat4x4 final_camera = projection * glm::inverse(camera_transform);
	//
	//					for (int32_t i = 0; i < num_objects; ++i) {
	//						const int32_t cache_index = cache_index;
	//						const int32_t cmd_list_end = job_data.rcs->_cmd_list_end[cache_index];
	//						Gleam::ICommandList* cmd_list = nullptr;
	//
	//						// Grab command list from the cache.
	//						if (job_data.rcs->_cmd_lists[cache_index].size() > static_cast<size_t>(cmd_list_end)) {
	//							cmd_list = job_data.rcs->_cmd_lists[cache_index][cmd_list_end].get();
	//
	//						// Cache is full, create a new one and add it to the cache.
	//						} else {
	//							cmd_list = job_data.rcs->_render_mgr->createCommandList();
	//							job_data.rcs->_cmd_lists[cache_index].emplace_back(cmd_list);
	//						}
	//
	//						++job_data.rcs->_cmd_list_end[cache_index];
	//
	//						render_cmds.lock.Lock();
	//						auto& cmd = render_cmds.command_list.emplace_back();
	//						cmd.cmd_list.reset(cmd_list);
	//						cmd.owns_command = false;
	//						render_cmds.lock.Unlock();
	//
	//						RenderJobData& render_data = job_data.render_job_data_cache.emplace_back();
	//						render_data.rcs = job_data.rcs;
	//						render_data.index = i;
	//						render_data.cmd_list = cmd_list;
	//						render_data.device = &device;
	//						render_data.target = render_target;
	//						render_data.view_projection = final_camera;
	//					}
	//				}
	//			}
	//		);
	//	}

	// Do a second loop so that none of our job_data.render_job_data_cache pointers get invalidated.
	if (job_data.job_data_cache.size() != job_data.render_job_data_cache.size()) {
		job_data.job_data_cache.resize(job_data.render_job_data_cache.size());

		for (int32_t i = 0; i < static_cast<int32_t>(job_data.render_job_data_cache.size()); ++i) {
			job_data.job_data_cache[i].job_data = &job_data.render_job_data_cache[i];
			job_data.job_data_cache[i].job_func = GenerateCommandListJob;
		}
	}

	if (job_data.job_data_cache.size() > 0) {
		job_data.rcs->_job_pool->addJobs(job_data.job_data_cache.data(), static_cast<int32_t>(job_data.job_data_cache.size()), job_data.job_counter);

		// $TODO: If the cache is significantly higher than the current usage, shrink the cache.

		job_data.rcs->_job_pool->helpWhileWaiting(thread_id, job_data.job_counter);
	}

	// Clear this for next run.
	job_data.rcs->_cmd_list_end[cache_index] = 0;
}

NS_END
