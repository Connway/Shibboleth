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

#include "Pipelines/Stages/Shibboleth_RenderCommandStage.h"
#include "Camera/Shibboleth_CameraPipelineData.h"
#include "Model/Shibboleth_ModelPipelineData.h"
#include "Shibboleth_GraphicsLogging.h"
#include "Shibboleth_RenderManager.h"
#include <Shibboleth_ITransformProvider.h>
#include <Shibboleth_ResourceManager.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_RasterState.h>
#include <Gleam_Mesh.h>
#include <Gaff_Function.h>

SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE(Shibboleth::RenderCommandStage, Shibboleth::IRenderPipelineStage)

namespace
{
	static Shibboleth::ProxyAllocator s_allocator{ GRAPHICS_ALLOCATOR };
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(RenderCommandStage);

bool RenderCommandStage::init(RenderManager& render_mgr)
{
	_camera_data = render_mgr.getRenderPipeline().getOrAddRenderData<CameraPipelineData>();
	_model_data = render_mgr.getRenderPipeline().getOrAddRenderData<ModelPipelineData>();
	_job_pool = &GetApp().getJobPool();
	_render_mgr = &render_mgr;

	// Fill device job cache.
	const auto& devices = render_mgr.getDevices();

	_device_job_data_cache.reserve(devices.size());
	_job_data_cache.reserve(devices.size());

	for (auto& device : devices) {
		_device_job_data_cache.emplace_back(DeviceJobData{ *this, *device });
		_job_data_cache.emplace_back(Gaff::JobData{ DeviceJob, &_device_job_data_cache.back() });
	}

	return true;
}

void RenderCommandStage::update(uintptr_t thread_id_int)
{
	if (_job_data_cache.size() > 1) {
		const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

		_job_pool->addJobs(_job_data_cache.data(), static_cast<int32_t>(_job_data_cache.size()), _job_counter);
		_job_pool->helpWhileWaiting(thread_id, _job_counter);

	// Only one device, just update in place.
	} else if (_job_data_cache.size() == 1) {
		DeviceJob(thread_id_int, _device_job_data_cache.data());
	}
}

const RenderCommandData* RenderCommandStage::getRenderCommands(void) const
{
	return &_render_commands;
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

	job_data.target->bind(*deferred_device);
	job_data.mesh_instance_data->raster_state->bind(*deferred_device);
	job_data.mesh_instance_data->program->bind(*deferred_device);
	job_data.mesh_instance_data->layout->bind(*deferred_device);


	// Update instance data buffers.
	const int32_t stride = job_data.mesh_instance_data->instance_data->pages[0].buffer->getStride();
	Vector<void*> buffer_cache(s_allocator);

	buffer_cache.reserve(job_data.mesh_instance_data->instance_data->pages.size());

	for (const auto& page : job_data.mesh_instance_data->instance_data->pages) {
		buffer_cache.emplace_back(page.buffer->map(*deferred_device));
	}

	Gleam::Mat4x4 center = glm::identity<Gleam::Mat4x4>();
	center[3] = Gleam::Vec4(job_data.model_bucket->model->getCenteringVector(), 1.0f);

	const int32_t num_objects = static_cast<int32_t>(job_data.model_bucket->transform_providers.size());

	for (int32_t i = 0; i < num_objects; ++i) {
		const int32_t instance_index = i % job_data.model_bucket->instances_per_buffer;
		const int32_t page_index = i / job_data.model_bucket->instances_per_buffer;

		Gleam::Mat4x4* const matrix = reinterpret_cast<Gleam::Mat4x4*>(reinterpret_cast<int8_t*>(buffer_cache[page_index]) + (stride * instance_index) + job_data.model_bucket->model_to_proj_offset);
		*matrix = job_data.view_projection * job_data.model_bucket->transform_providers[i]->provideTransform().toMatrix() * center;
	}

	for (const auto& page : job_data.mesh_instance_data->instance_data->pages) {
		page.buffer->unmap(*deferred_device);
	}


	// Iterate over all the instance buffers and set their resource views and render.
	Gleam::Mesh* const mesh = job_data.mesh_instance_data->mesh;
	Gleam::ProgramBuffers* const pb = job_data.mesh_instance_data->program_buffers.get();
	const int32_t num_pages = static_cast<int32_t>(job_data.mesh_instance_data->instance_data->pages.size());

	for (int32_t i = 0; i < num_pages; ++i) {
		for (int32_t j = 0; j < static_cast<int32_t>(std::size(job_data.mesh_instance_data->pipeline_data)); ++j) {
			auto& var_map = job_data.mesh_instance_data->pipeline_data[j].buffer_vars;

			for (auto& entry : var_map) {
				GAFF_ASSERT(entry.second.srv_index >= 0);

				pb->setResourceView(
					static_cast<Gleam::IShader::Type>(j),
					entry.second.srv_index,
					*entry.second.pages[i].srv
				);
			}
		}

		const int32_t objects_to_render = (i == (num_pages - 1)) ?
			num_objects % job_data.model_bucket->instances_per_buffer :
			job_data.model_bucket->instances_per_buffer;

		pb->bind(*deferred_device);
		mesh->renderInstanced(*deferred_device, objects_to_render);

		// $TODO: Add support and convert to render instanced indirect.
	}

	if (!deferred_device->finishCommandList(*job_data.cmd_list)) {
		// $TODO: Log error.
	}
}

void RenderCommandStage::DeviceJob(uintptr_t thread_id_int, void* data)
{
	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	DeviceJobData& job_data = *reinterpret_cast<DeviceJobData*>(data);

	const int32_t cache_index = job_data.rcs->_render_mgr->getRenderPipeline().getRenderCacheIndex();
	const auto& camera_render_data = job_data.rcs->_camera_data->getRenderData();
	Gleam::RenderDevice& device = *job_data.device;

	// $TODO: Resize command list to number of object types to render.
	// $TODO: Submit jobs as we process. Don't wait to generate data for all jobs and then run.
	RenderCommandList& cmd_list = job_data.rcs->_render_commands.getCommandList(device, cache_index);
	int32_t total_command_lists = 0;

	job_data.render_job_data_cache.clear();
	job_data.render_job_data_cache.reserve(static_cast<size_t>(camera_render_data.getValidSize()));

	// Render all cameras on this device.
	for (const CameraRenderData& camera_data : camera_render_data) {
		const auto it = camera_data.g_buffers.find(&device);

		if (it == camera_data.g_buffers.end()) {
			continue;
		}

		Gleam::RenderTarget* const render_target = it->second.render_target.get();

		const Gleam::IVec2& size = render_target->getSize();
		const Gleam::Mat4x4 projection = glm::perspectiveFovLH(
			camera_data.view.fov * Gaff::TurnsToRadians,
			static_cast<float>(size.x),
			static_cast<float>(size.y),
			camera_data.view.z_planes.x,
			camera_data.view.z_planes.y
		);

		const Gleam::Mat4x4 camera_transform = camera_data.view.transform.toMatrixTurns();
		const Gleam::Mat4x4 final_camera = projection * glm::inverse(camera_transform);

		RenderJobData render_data;
		render_data.rcs = job_data.rcs;
		render_data.device = &device;
		render_data.target = render_target;
		render_data.view_projection = final_camera;

		// $TODO: num_meshes is based on the scene(s) this camera is rendering.
		const int32_t num_meshes = job_data.rcs->_model_data->getMeshCount();

		total_command_lists += num_meshes;

		// Ensure we have enough command lists.
		if (cmd_list.command_list.size() < static_cast<size_t>(total_command_lists)) {
			cmd_list.command_list.reserve(static_cast<size_t>(total_command_lists));

			for (int32_t i = static_cast<int32_t>(cmd_list.command_list.size()); i < total_command_lists; ++i) {
				cmd_list.command_list.emplace_back(UniquePtr<Gleam::CommandList>(SHIB_ALLOCT(Gleam::CommandList, s_allocator)));
			}
		}

		// $TODO: Iterate over all models/meshes.
		// $TODO: Filter models/meshes by scene.
		// $TODO: Cache command list based on model/mesh.
		// $TODO: Only regenerate command list if there are dirty instances.
		// $TODO: Dirty instances should mainly only need to update transform buffer. Only if we are adding/removing instances pages should we need to regenerate the command list.
		// $TODO: Double check above assumption.
		const auto& models = job_data.rcs->_model_data->getRegisteredModels();
		int32_t cmd_list_index = 0;

		for (const auto& entry : models) {
			for (const auto& mesh_instance : entry.second.mesh_instances) {
				render_data.mesh_instance_data = &mesh_instance.device_data.at_key(&device);
				render_data.model_bucket = &entry.second;
				render_data.cmd_list = cmd_list.command_list[cmd_list_index].commands.get();

				job_data.render_job_data_cache.emplace_back(render_data);
				++cmd_list_index;
			}
		}
	}

	// Trim off unused command lists.
	if (cmd_list.command_list.size() > static_cast<size_t>(total_command_lists)) {
		cmd_list.command_list.resize(static_cast<size_t>(total_command_lists));
	}

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

		// $TODO: If the cache capacity is significantly higher than the current size, shrink to fit.

		job_data.rcs->_job_pool->helpWhileWaiting(thread_id, job_data.job_counter);
	}
}

NS_END
