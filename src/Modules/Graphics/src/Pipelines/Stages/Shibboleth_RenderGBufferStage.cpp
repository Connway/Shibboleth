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

#pragma once

#include "Pipelines/Stages/Shibboleth_RenderGBufferStage.h"
#include "Camera/Shibboleth_CameraPipelineData.h"
#include "Shibboleth_GraphicsLogging.h"
#include "Shibboleth_RenderManager.h"
#include <Ptrs/Shibboleth_ManagerRef.h>
#include <Shibboleth_ResourceManager.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_Program.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::RenderGBufferStage)
	.template base<Shibboleth::IRenderPipelineStage>()

	.var("render_g_buffer_material", &Shibboleth::RenderGBufferStage::_render_g_buffer_material)
SHIB_REFLECTION_DEFINE_END(Shibboleth::RenderGBufferStage)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(RenderGBufferStage)

bool RenderGBufferStage::init(RenderManager& render_mgr)
{
	_camera_data = render_mgr.getRenderPipeline().getOrAddRenderData<CameraPipelineData>();
	_job_pool = &GetApp().getJobPool();

	ManagerRef<ResourceManager> res_mgr;
	res_mgr->waitForResource(*_render_g_buffer_material);

	if (_render_g_buffer_material->hasFailed()) {
		LogErrorGraphics("RenderGBufferStage::init: Failed to load material '%s'.", _render_g_buffer_material->getFilePath().getBuffer());
		return false;
	}

	// Fill device job cache.
	const auto& devices = render_mgr.getDevices();

	_device_job_data_cache.reserve(devices.size());
	_job_data_cache.reserve(devices.size());

	ProxyAllocator allocator = GRAPHICS_ALLOCATOR;

	for (auto& device : devices) {
		DeviceJobData job_data;
		job_data.device = device.get();
		job_data.rgbs = this;

		job_data.program_buffers.reset(SHIB_ALLOCT(Gleam::ProgramBuffers, allocator));
		job_data.raster_state.reset(SHIB_ALLOCT(Gleam::RasterState, allocator));
		job_data.sampler.reset(SHIB_ALLOCT(Gleam::SamplerState, allocator));

		Gleam::IProgram* const program = _render_g_buffer_material->getProgram(*device);
		GAFF_ASSERT(program->getRendererType() == job_data.program_buffers->getRendererType());

		job_data.program = static_cast<Gleam::Program*>(program);

		const Gleam::ISamplerState::Settings sampler_settings = {
			Gleam::ISamplerState::Filter::NearestNearestNearest,
			Gleam::ISamplerState::Wrap::Clamp,
			Gleam::ISamplerState::Wrap::Clamp,
			Gleam::ISamplerState::Wrap::Clamp,
			0.0f, 0.0f,
			0.0f,
			1
		};

		if (!job_data.sampler->init(*device, sampler_settings)) {
			LogErrorGraphics("RenderGBufferStage::init: Failed to initialize sampler.");
			return false;
		}

		Gleam::IRasterState::Settings raster_settings;
		raster_settings.depth_clip_enabled = false;

		if (!job_data.raster_state->init(*device, raster_settings)) {
			LogErrorGraphics("RenderGBufferStage::init: Failed to initialize raster state.");
			return false;
		}

		job_data.program_buffers->addSamplerState(
			Gleam::IShader::Type::Pixel,
			*job_data.sampler
		);

		for (int32_t i = 0; i < k_cache_index_count; ++i) {
			auto& cmd_list = _render_commands.getCommandList(*device, i);
			cmd_list.command_list.reserve(1);

			cmd_list.command_list.emplace_back(UniquePtr<Gleam::CommandList>(SHIB_ALLOCT(Gleam::CommandList, allocator)));
		}

		_device_job_data_cache.emplace_back(std::move(job_data));
		_job_data_cache.emplace_back(Gaff::JobData{ DeviceJob, &_device_job_data_cache.back() });
	}

	return true;
}

void RenderGBufferStage::update(uintptr_t thread_id_int)
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

const RenderCommandData* RenderGBufferStage::getRenderCommands(void) const
{
	return &_render_commands;
}

void RenderGBufferStage::DeviceJob(uintptr_t thread_id_int, void* data)
{
	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	DeviceJobData& job_data = *reinterpret_cast<DeviceJobData*>(data);

	const int32_t cache_index = job_data.rgbs->_render_mgr->getRenderPipeline().getRenderCacheIndex();
	auto& camera_render_data = job_data.rgbs->_camera_data->getRenderData();
	Gleam::RenderDevice& device = *job_data.device;
	RenderCommandList& cmd_list = job_data.rgbs->_render_commands.getCommandList(device, cache_index);

	Gleam::RenderDevice* const deferred_device = job_data.rgbs->_render_mgr->getDeferredDevice(device, thread_id);

	job_data.raster_state->bind(*deferred_device);
	job_data.program->bind(*deferred_device);

	// $TODO: If camera_render_data is not dirty, we don't need to regenerate the command list.
	// $TODO: Need a dynamic way of determining camera render order.
	for (CameraRenderData& camera_data : camera_render_data) {
		const auto it = camera_data.g_buffers.find(&device);

		if (it == camera_data.g_buffers.end()) {
			continue;
		}

		GBuffer& g_buffer = it->second;

		Gleam::ProgramBuffers& pb = *job_data.program_buffers;
		pb.clearResourceViews();

		pb.addResourceView(Gleam::IShader::Type::Pixel, *g_buffer.diffuse_srv);
		pb.addResourceView(Gleam::IShader::Type::Pixel, *g_buffer.specular_srv);
		pb.addResourceView(Gleam::IShader::Type::Pixel, *g_buffer.normal_srv);
		pb.addResourceView(Gleam::IShader::Type::Pixel, *g_buffer.position_srv);
		pb.addResourceView(Gleam::IShader::Type::Pixel, *g_buffer.depth_srv);

		pb.bind(*deferred_device);

		g_buffer.to_output_render_target->bind(*deferred_device);
		deferred_device->renderNoVertexInput(6);
	}

	if (!deferred_device->finishCommandList(*cmd_list.command_list.back().commands)) {
		// $TODO: Log error.
	}
}

NS_END
