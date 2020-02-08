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

#include "Shibboleth_MainLoop.h"
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_ISystem.h>
#include <Shibboleth_IApp.h>
#include <EAThread/eathread.h>

#include <Shibboleth_RenderManagerBase.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_SamplerStateResource.h>
#include <Shibboleth_RasterStateResource.h>
#include <Shibboleth_MaterialResource.h>
#include <Shibboleth_ECSSceneResource.h>
#include <Shibboleth_TextureResource.h>
#include <Shibboleth_ModelResource.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_IRenderTarget.h>
#include <Gleam_IncludeMatrix.h>

SHIB_REFLECTION_DEFINE_BEGIN_NEW(MainLoop)
	.BASE(IMainLoop)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END_NEW(MainLoop)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_NEW(MainLoop)

static void UpdateSystemJob(void* data)
{
	ISystem* const system = reinterpret_cast<ISystem*>(data);
	system->update();
}

bool MainLoop::init(void)
{
	IApp& app = GetApp();

	RenderManagerBase& rm = app.GETMANAGERT(RenderManagerBase, RenderManager);
	_render_mgr = &rm;

	const auto* const systems = app.getReflectionManager().getTypeBucket(CLASS_HASH(ISystem));

	if (!systems || systems->empty()) {
		return true;
	}

	ProxyAllocator allocator("MainLoop");
	SerializeReaderWrapper readerWrapper(allocator);

	if (!OpenJSONOrMPackFile(readerWrapper, "cfg/update_phases.cfg")) {
		LogErrorDefault("Failed to read cfg/update_phases.cfg[.bin].");
		return false;
	}

	const Gaff::ISerializeReader& reader = *readerWrapper.getReader();

	if (!reader.isArray()) {
		LogErrorDefault("MainLoop: Malformed cfg/update_phases.cfg[.bin]. Root is not a 3-dimensional array of strings.");
		return false;
	}

	ReflectionManager& refl_mgr = GetApp().getReflectionManager();
	const auto* bucket = refl_mgr.getTypeBucket(CLASS_HASH(ISystem));

	_systems.set_allocator(allocator);
	_systems.resize(reader.size());

	_job_data.set_allocator(allocator);
	_job_data.resize(reader.size());

	for (int32_t i = 0; i < reader.size(); ++i) {
		const auto guard_1 = reader.enterElementGuard(i);

		if (!reader.isArray()) {
			LogErrorDefault("MainLoop: Malformed cfg/update_phases.cfg[.bin]. Config is not a 3-dimensional array of strings.");
			return false;
		}

		_systems[i].set_allocator(allocator);
		_systems[i].resize(reader.size());

		_job_data[i].set_allocator(allocator);
		_job_data[i].resize(reader.size());

		for (int32_t j = 0; j < reader.size(); ++j) {
			const auto guard_2 = reader.enterElementGuard(j);

			if (!reader.isArray()) {
				LogErrorDefault("MainLoop: Malformed cfg/update_phases.cfg[.bin]. Config is not a 3-dimensional array of strings.");
				return false;
			}

			_systems[i][j].set_allocator(allocator);
			_systems[i][j].resize(reader.size());

			_job_data[i][j].set_allocator(allocator);
			_job_data[i][j].resize(reader.size());

			for (int32_t k = 0; k < reader.size(); ++k) {
				const auto guard_3 = reader.enterElementGuard(k);

				if (!reader.isString()) {
					LogErrorDefault("MainLoop: Malformed cfg/update_phases.cfg[.bin]. Config is not a 3-dimensional array of strings.");
					return false;
				}

				const char* system_name = reader.readString();
				const Gaff::Hash64 hash = Gaff::FNV1aHash64String(system_name);

				const auto it = eastl::lower_bound(bucket->begin(), bucket->end(), hash, ReflectionManager::CompareRefHash);

				if (it == bucket->end() || hash != (*it)->getReflectionInstance().getHash()) {
					LogErrorDefault("MainLoop: Could not find system '%s'.", system_name);
					reader.freeString(system_name);
					return false;
				}

				ISystem* const system = (*it)->createT<ISystem>(CLASS_HASH(ISystem), allocator);

				if (!system) {
					LogErrorDefault("MainLoop: Failed to create system '%s'", system_name);
					reader.freeString(system_name);
					return false;
				}

				if (!system->init()) {
					LogErrorDefault("MainLoop: Failed to initialize system '%s'", system_name);
					reader.freeString(system_name);
					return false;
				}

				reader.freeString(system_name);
				_systems[i][j][k].reset(system);

				_job_data[i][j][k].job_func = UpdateSystemJob;
				_job_data[i][j][k].job_data = system;
			}
		}
	}

	return true;
}

void MainLoop::destroy(void)
{
}

void MainLoop::update(void)
{
	_render_mgr->updateWindows(); // This has to happen in the main thread.

	if (_counter) {
		GetApp().getJobPool().doAJob();

	// Jobs are done. Move up to the next section.
	} else {
		++_system_group;

		while (_system_group >= static_cast<int32_t>(_systems[_update_block].size())) {
			_system_group = 0;
			++_update_block;

			if (_update_block >= static_cast<int32_t>(_systems.size())) {
				_update_block = 0;
			}
		}

		const auto& job_data = _job_data[_update_block][_system_group];
		GetApp().getJobPool().addJobs(job_data.data(), job_data.size(), _counter);
	}

	// Give some time to other threads.
	EA::Thread::ThreadSleep();


	//static ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();

	//// This is not safe. Doing for easy testing. Will remove later.
	////static const auto camera_material_res = res_mgr.requestResourceT<MaterialResource>("CameraToScreen/camera_to_screen.material");
	//static const auto sampler_state_res = res_mgr.requestResourceT<SamplerStateResource>("SamplerStates/anisotropic_16x.sampler_state");
	//static const auto raster_state_res = res_mgr.requestResourceT<RasterStateResource>("RasterStates/opaque.raster_state");
	//static const auto material_res = res_mgr.requestResourceT<MaterialResource>("Materials/test.material");
	//static const auto texture_res = res_mgr.requestResourceT<TextureResource>("Models/ninja.texture");
	////static const auto scene_res = res_mgr.requestResourceT<ECSSceneResource>("Scenes/test.scene");
	//static const auto model_res = res_mgr.requestResourceT<ModelResource>("Models/ninja.model");
	//static auto* const program_buffers = _render_mgr->createProgramBuffers();
	//static bool first_run = true;

	//auto& rd = _render_mgr->getDevice(0);
	//auto& out = *_render_mgr->getOutput("main");

	//if (first_run) {
	//	first_run = false;

	//	//res_mgr.waitForResource(*camera_material_res);
	//	res_mgr.waitForResource(*sampler_state_res);
	//	res_mgr.waitForResource(*raster_state_res);
	//	res_mgr.waitForResource(*material_res);
	//	res_mgr.waitForResource(*texture_res);
	//	//res_mgr.waitForResource(*scene_res);
	//	res_mgr.waitForResource(*model_res);

	//	//if (camera_material_res->hasFailed()) {
	//	//	GetApp().quit();
	//	//	return;
	//	//}

	//	if (sampler_state_res->hasFailed()) {
	//		GetApp().quit();
	//		return;
	//	}

	//	if (raster_state_res->hasFailed()) {
	//		GetApp().quit();
	//		return;
	//	}

	//	if (material_res->hasFailed()) {
	//		GetApp().quit();
	//		return;
	//	}

	//	if (texture_res->hasFailed()) {
	//		GetApp().quit();
	//		return;
	//	}

	//	//if (scene_res->hasFailed()) {
	//	//	GetApp().quit();
	//	//	return;
	//	//}

	//	if (model_res->hasFailed()) {
	//		GetApp().quit();
	//		return;
	//	}

	//	const Gleam::IWindow* const window = _render_mgr->getWindow("main");
	//	const glm::mat4x4 projection = glm::perspectiveFovLH_ZO(90.0f, static_cast<float>(window->getWidth()), static_cast<float>(window->getHeight()), 0.0f, 100.0f);
	//	const glm::mat4x4 camera = glm::lookAtLH(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//	const glm::mat4x4 result = projection * camera;

	//	Gleam::IBuffer::BufferSettings buffer_settings =
	//	{
	//		&result,
	//		sizeof(glm::mat4x4),
	//		sizeof(glm::mat4x4),
	//		Gleam::IBuffer::BT_STRUCTURED_DATA,
	//		Gleam::IBuffer::MT_WRITE,
	//		true
	//	};

	//	auto* const camera_buffer = _render_mgr->createBuffer();
	//	camera_buffer->init(rd, buffer_settings);

	//	auto* const camera_srv = _render_mgr->createShaderResourceView();
	//	camera_srv->init(rd, camera_buffer);

	//	program_buffers->addSamplerState(Gleam::IShader::SHADER_PIXEL, sampler_state_res->getSamplerState(rd));
	//	program_buffers->addResourceView(Gleam::IShader::SHADER_PIXEL, texture_res->getShaderResourceView(rd));
	//	program_buffers->addResourceView(Gleam::IShader::SHADER_VERTEX, camera_srv);
	//}

	//rd.frameBegin(out);

	//out.getRenderTarget().bind(rd);
	//out.getRenderTarget().clear(rd, Gleam::IRenderTarget::CLEAR_ALL, 1.0f, 0, Gleam::COLOR_BLACK);

	//raster_state_res->getRasterState(rd)->bind(rd);
	//material_res->getProgram(rd)->bind(rd);
	//material_res->getLayout(rd)->bind(rd);
	//program_buffers->bind(rd);

	//for (int32_t i = 0; i < model_res->getNumMeshes(); ++i) {
	//	model_res->getMesh(i)->getMesh(rd)->renderInstanced(rd, 1);
	//}

	//rd.frameEnd(out);

	////camera_material_res->getProgram(rd)->bind(rd);
	////rd.resetRenderState();
	////rd.renderNoVertexInput(6);

	////GetApp().quit();
}

NS_END
