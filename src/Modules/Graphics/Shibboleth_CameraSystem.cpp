/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_CameraSystem.h"
#include "Shibboleth_RenderManagerBase.h"
#include "Shibboleth_CameraComponent.h"
#include <Shibboleth_ResourceManager.h>

SHIB_REFLECTION_DEFINE_BEGIN(CameraPreRenderSystem)
	.BASE(ISystem)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(CameraPreRenderSystem)

SHIB_REFLECTION_DEFINE_BEGIN(CameraPostRenderSystem)
	.BASE(ISystem)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(CameraPostRenderSystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(CameraPreRenderSystem)
SHIB_REFLECTION_CLASS_DEFINE(CameraPostRenderSystem)


bool CameraPreRenderSystem::init(void)
{
	ECSQuery camera_query;
	camera_query.add<Camera>(_camera);

	_render_mgr = &GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	_ecs_mgr = &GetApp().getManagerTFast<ECSManager>();
	_ecs_mgr->registerQuery(std::move(camera_query));

	return true;
}

void CameraPreRenderSystem::update(void)
{
	const int32_t num_cameras = static_cast<int32_t>(_camera.size());

	for (int32_t camera_index = 0; camera_index < num_cameras; ++camera_index) {
		_ecs_mgr->iterate<Camera>([&](EntityID id, const Camera& camera) -> void
		{
			if (_render_mgr->hasGBuffer(id)) {
				return;
			}

			glm::ivec2 size(0, 0);

			if (const Gleam::IWindow* const window = _render_mgr->getWindow(camera.device_tag)) {
				size = window->getSize();
			} /*else if (check for texture target) {
			}*/ else {
				// $TODO: Log error.
				return;
			}

			_render_mgr->createGBuffer(id, camera.device_tag, size);
		},
		_camera[camera_index]);
	}
}



bool CameraPostRenderSystem::init(void)
{
	ECSQuery camera_query;
	camera_query.add<Camera>(_camera);

	_render_mgr = &GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	_ecs_mgr = &GetApp().getManagerTFast<ECSManager>();
	_ecs_mgr->registerQuery(std::move(camera_query));

	constexpr const char* const k_camera_material = "Materials/CameraToTexture/camera_to_texture.material";

	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();
	_camera_material = res_mgr.requestResourceT<MaterialResource>(k_camera_material);

	res_mgr.waitForResource(*_camera_material);

	if (_camera_material->hasFailed()) {
		// $TODO: Log error.
		return false;
	}

	return true;
}

void CameraPostRenderSystem::update(void)
{
	const int32_t starting_index = static_cast<int32_t>(_job_data_cache.size());
	const int32_t num_devices = _render_mgr->getNumDevices();

	if (num_devices > starting_index) {

		_camera_job_data_cache.resize(num_devices);
		_job_data_cache.resize(num_devices);

		for (int32_t i = starting_index; i < num_devices; ++i) {
			Gleam::IRenderDevice& device = _render_mgr->getDevice(i);

			_camera_job_data_cache[i].program_buffers.reset(_render_mgr->createProgramBuffers());
			_camera_job_data_cache[i].raster_state.reset(_render_mgr->createRasterState());
			_camera_job_data_cache[i].sampler.reset(_render_mgr->createSamplerState());
			_camera_job_data_cache[i].device = &device;
			_camera_job_data_cache[i].system = this;

			Gleam::ISamplerState::SamplerSettings sampler_settings = {
				Gleam::ISamplerState::FILTER_NEAREST_NEAREST_NEAREST,
				Gleam::ISamplerState::WRAP_CLAMP,
				Gleam::ISamplerState::WRAP_CLAMP,
				Gleam::ISamplerState::WRAP_CLAMP,
				0.0f, 0.0f,
				0.0f,
				1
			};

			Gleam::IRasterState::RasterSettings raster_settings;
			raster_settings.depth_clip_enabled = false;

			const bool raster_success = _camera_job_data_cache[i].raster_state->init(device, raster_settings);
			const bool sampler_success = _camera_job_data_cache[i].sampler->init(device, sampler_settings);

			if (!sampler_success || !raster_success) {
				// $TODO: Log error.

			} else {
				_camera_job_data_cache[i].program_buffers->addSamplerState(
					Gleam::IShader::SHADER_PIXEL,
					_camera_job_data_cache[i].sampler.get()
				);
			}

			_job_data_cache[i].job_data = &_camera_job_data_cache[i];
			_job_data_cache[i].job_func = RenderCameras;
		}
	}

	auto& job_pool = GetApp().getJobPool();
	
	job_pool.addJobs(_job_data_cache.data(), _job_data_cache.size(), _job_counter);
	job_pool.helpWhileWaiting(_job_counter);

	_render_mgr->presentAllOutputs();
}

void CameraPostRenderSystem::RenderCameras(void* data)
{
	CameraRenderData& job_data = *reinterpret_cast<CameraRenderData*>(data);
	const int32_t num_cameras = static_cast<int32_t>(job_data.system->_camera.size());

	Gleam::IProgram* const program = job_data.system->_camera_material->getProgram(*job_data.device);

	if (!program) {
		return;
	}

	job_data.raster_state->bind(*job_data.device);
	program->bind(*job_data.device);

	for (int32_t camera_index = 0; camera_index < num_cameras; ++camera_index) {
		job_data.system->_ecs_mgr->iterate<Camera>([&](EntityID id, const Camera& camera) -> void
		{
			const auto* const devices = job_data.system->_render_mgr->getDevicesByTag(camera.device_tag);

			if (!devices || Gaff::Find(*devices, job_data.device) == devices->end()) {
				return;
			}

			const auto* const g_buffer = job_data.system->_render_mgr->getGBuffer(id, *job_data.device);

			if (!g_buffer) {
				return;
			}

			Gleam::IProgramBuffers& pb = *job_data.program_buffers;
			pb.clearResourceViews();

			pb.addResourceView(Gleam::IShader::SHADER_PIXEL, g_buffer->diffuse_srv.get());
			pb.addResourceView(Gleam::IShader::SHADER_PIXEL, g_buffer->specular_srv.get());
			pb.addResourceView(Gleam::IShader::SHADER_PIXEL, g_buffer->normal_srv.get());
			pb.addResourceView(Gleam::IShader::SHADER_PIXEL, g_buffer->position_srv.get());
			pb.addResourceView(Gleam::IShader::SHADER_PIXEL, g_buffer->depth_srv.get());

			pb.bind(*job_data.device);

			if (Gleam::IRenderOutput* const output = job_data.system->_render_mgr->getOutput(camera.device_tag)) {
				output->getRenderTarget().bind(*job_data.device);
			} /*else if (check for texture target) {
			}*/ else {
				// $TODO: Log error.
				return;
			}

			job_data.device->renderNoVertexInput(6);
		},
		job_data.system->_camera[camera_index]);
	}
}

NS_END
