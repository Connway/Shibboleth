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

#include "Shibboleth_MainLoop.h"
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

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

SHIB_REFLECTION_DEFINE(MainLoop)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(MainLoop)
	.BASE(IMainLoop)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(MainLoop)

bool MainLoop::init(void)
{
	RenderManagerBase& rm = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	_render_mgr = &rm;

	return true;
}

void MainLoop::destroy(void)
{
}

void MainLoop::update(void)
{
	_render_mgr->updateWindows(); // This has to happen in the main thread.
	//_update_mgr->update();

	//std::this_thread::yield();

	static ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();

	// This is not safe. Doing for easy testing. Will remove later.
	static const auto camera_material_res = res_mgr.requestResourceT<MaterialResource>("CameraToScreen/camera_to_screen.material");
	static const auto sampler_state_res = res_mgr.requestResourceT<SamplerStateResource>("SamplerStates/anisotropic_16x.sampler_state");
	static const auto raster_state_res = res_mgr.requestResourceT<RasterStateResource>("RasterStates/opaque.raster_state");
	static const auto material_res = res_mgr.requestResourceT<MaterialResource>("Materials/test.material");
	static const auto texture_res = res_mgr.requestResourceT<TextureResource>("Models/nskinwh.png");
	static const auto scene_res = res_mgr.requestResourceT<ECSSceneResource>("Scenes/test.scene");
	static const auto model_res = res_mgr.requestResourceT<ModelResource>("Models/ninja.model");
	static auto* const program_buffers = _render_mgr->createProgramBuffers();
	static bool first_run = true;

	auto& rd = _render_mgr->getDevice(0);
	auto& out = *_render_mgr->getOutput("main");

	if (first_run) {
		first_run = false;

		res_mgr.waitForResource(*camera_material_res);
		res_mgr.waitForResource(*sampler_state_res);
		res_mgr.waitForResource(*raster_state_res);
		res_mgr.waitForResource(*material_res);
		res_mgr.waitForResource(*texture_res);
		res_mgr.waitForResource(*scene_res);
		res_mgr.waitForResource(*model_res);

		if (camera_material_res->hasFailed()) {
			GetApp().quit();
			return;
		}

		if (sampler_state_res->hasFailed()) {
			GetApp().quit();
			return;
		}

		if (raster_state_res->hasFailed()) {
			GetApp().quit();
			return;
		}

		if (material_res->hasFailed()) {
			GetApp().quit();
			return;
		}

		if (texture_res->hasFailed()) {
			GetApp().quit();
			return;
		}

		if (scene_res->hasFailed()) {
			GetApp().quit();
			return;
		}

		if (model_res->hasFailed()) {
			GetApp().quit();
			return;
		}

		const Gleam::IWindow* const window = _render_mgr->getWindow("main");
		const glm::mat4x4 projection = glm::perspectiveFovLH_ZO(90.0f, static_cast<float>(window->getWidth()), static_cast<float>(window->getHeight()), 0.0f, 100.0f);
		const glm::mat4x4 camera = glm::lookAtLH(glm::vec3(0.0f, 5.0f, -5.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4x4 result = projection * camera;

		Gleam::IBuffer::BufferSettings buffer_settings =
		{
			&result,
			sizeof(glm::mat4x4),
			sizeof(glm::mat4x4),
			Gleam::IBuffer::BT_STRUCTURED_DATA,
			Gleam::IBuffer::MT_WRITE,
			true
		};

		auto* const camera_buffer = _render_mgr->createBuffer();
		camera_buffer->init(rd, buffer_settings);

		auto* const camera_srv = _render_mgr->createShaderResourceView();
		camera_srv->init(rd, camera_buffer);

		program_buffers->addSamplerState(Gleam::IShader::SHADER_PIXEL, sampler_state_res->getSamplerState(rd));
		program_buffers->addResourceView(Gleam::IShader::SHADER_PIXEL, texture_res->getShaderResourceView(rd));
		program_buffers->addResourceView(Gleam::IShader::SHADER_VERTEX, camera_srv);
	}

	rd.frameBegin(out);

	out.getRenderTarget().bind(rd);
	out.getRenderTarget().clear(rd, Gleam::IRenderTarget::ClearFlags::CLEAR_ALL, 1.0f, 0, Gleam::COLOR_BLACK);

	raster_state_res->getRasterState(rd)->set(rd);
	material_res->getProgram(rd)->bind(rd);
	material_res->getLayout(rd)->bind(rd);
	program_buffers->bind(rd);

	for (int32_t i = 0; i < model_res->getNumMeshes(); ++i) {
		model_res->getMesh(i)->getMesh(rd)->renderInstanced(rd, 1);
	}

	rd.frameEnd(out);

	//camera_material_res->getProgram(rd)->bind(rd);
	//rd.resetRenderState();
	//rd.renderNoVertexInput(6);

	//GetApp().quit();
}

NS_END
