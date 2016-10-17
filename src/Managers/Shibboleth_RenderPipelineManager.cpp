/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Shibboleth_RenderPipelineManager.h"
#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_IRenderPipeline.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

#include <Gleam_IShaderResourceView.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_IProgram.h>
#include <Gaff_Utils.h>
#include <Gaff_File.h>

NS_SHIBBOLETH

const char* gRenderToScreenVertexShader = "";
const char* gRenderToScreenPixelShader = "";


REF_IMPL_REQ(RenderPipelineManager);
SHIB_REF_IMPL(RenderPipelineManager)
.addBaseClassInterfaceOnly<RenderPipelineManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IRenderPipelineManager)
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

RenderPipelineManager::RenderPipelineManager(void):
	_active_pipeline(SIZE_T_FAIL), _render_mgr(nullptr)
{
}

RenderPipelineManager::~RenderPipelineManager(void)
{
	for (auto it = _pipelines.begin(); it != _pipelines.end(); ++it) {
		_pipeline_map[(*it)->getName()](*it);
	}
}

const char* RenderPipelineManager::getName(void) const
{
	return GetFriendlyName();
}

void RenderPipelineManager::allManagersCreated(void)
{
}

void RenderPipelineManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.emplacePush(U8String("Render Pipeline Manager: Generate Command Lists"), Gaff::Bind(this, &RenderPipelineManager::generateCommandLists));
	entries.emplacePush(U8String("Render Pipeline Manager: Render To Screen"), Gaff::Bind(this, &RenderPipelineManager::renderToScreen));
}

bool RenderPipelineManager::init(const char* initial_pipeline)
{
	IResourceManager& res_mgr = GetApp().getManagerT<IResourceManager>();
	_camera_to_screen_program_buffers = res_mgr.requestResource("ProgramBuffers", "render_pipeline_manager_program_buffers");
	_camera_to_screen_sampler = res_mgr.requestResource("Resources/Samplers/anisotropic_16x.sampler");
	_camera_to_screen_shader = res_mgr.requestResource("Resources/Camera To Screen/camera_to_screen.material");

	GetApp().getLogManager().logMessage(
		LogManager::LOG_NORMAL, GetApp().getLogFileName(),
		"\n==================================================\n"
		"==================================================\n"
		"Loading Render Pipelines...\n"
	);

	bool early_out = Gaff::ForEachTypeInDirectory<Gaff::FDT_RegularFile>("./Render Pipelines", [&](const char* name, size_t) -> bool
	{
#ifdef PLATFORM_WINDOWS
		U8String rel_path = U8String("../Render Pipelines/") + name;
#else
		U8String rel_path = U8String("./Render Pipelines/") + name;
#endif

		// Error out if it's not a dynamic module
		if (!Gaff::File::CheckExtension(name, DYNAMIC_EXTENSION)) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "'%s' is not a dynamic module.\n", rel_path.getBuffer());
			GetApp().quit();
			return true;

			// It is a dynamic module, but not compiled for our architecture.
			// Or not compiled in our build mode. Just skip over it.
		} else if (!Gaff::File::CheckExtension(name, BIT_EXTENSION DYNAMIC_EXTENSION)) {
			return false;
		}

		DynamicLoader::ModulePtr module = GetApp().loadModule(rel_path.getBuffer(), name);

		if (!module.valid()) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Could not load dynamic module '%s'.\n", rel_path.getBuffer());
			GetApp().quit();
			return true;
		}

		GetApp().getLogManager().logMessage(LogManager::LOG_NORMAL, GetApp().getLogFileName(), "Loading render pipelines from module '%s'\n", rel_path.getBuffer());

		if (!addRenderPipelines(module)) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Could not load render pipelines in dynamic module '%s'.\n", rel_path.getBuffer());
			GetApp().quit();
			return true;
		}

		return false;
	});

	if (early_out) {
		return false;
	}

	for (size_t i = 0; i < _pipelines.size(); ++i) {
		if (!strcmp(_pipelines[i]->getName(), initial_pipeline)) {
			_active_pipeline = i;
			break;
		}
	}

	if (_active_pipeline == SIZE_T_FAIL) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Could not find initial render pipeline '%s'.\n", initial_pipeline);
		GetApp().quit();
	}

	_render_mgr = &GetApp().getManagerT<IRenderManager>();

	// Wait for resources to finish loading
	while (!_camera_to_screen_program_buffers.getResourcePtr()->isLoaded() &&
			!_camera_to_screen_program_buffers.getResourcePtr()->hasFailed()) {
	}

	while (!_camera_to_screen_sampler.getResourcePtr()->isLoaded() &&
			!_camera_to_screen_sampler.getResourcePtr()->hasFailed()) {
	}

	while (!_camera_to_screen_shader.getResourcePtr()->isLoaded() &&
			!_camera_to_screen_shader.getResourcePtr()->hasFailed()) {
	}

	if (_camera_to_screen_program_buffers.getResourcePtr()->hasFailed()) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Render Pipeline Manager failed to create 'ProgramBuffers'.\n");
		return false;
	}

	if (_camera_to_screen_sampler.getResourcePtr()->hasFailed()) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Render Pipeline Manager failed to load resource 'Resources/Samplers/anisotropic_16x.sampler'.\n");
		return false;
	}

	if (_camera_to_screen_shader.getResourcePtr()->hasFailed()) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Render Pipeline Manager failed to load resource 'Resources/Camera To Screen/camera_to_screen.material'.\n");
		return false;
	}

	// Add the sampler to the program buffers
	for (size_t i = 0; i < _camera_to_screen_program_buffers->data.size(); ++i) {
		_camera_to_screen_program_buffers->data[i]->addSamplerState(Gleam::IShader::SHADER_PIXEL, _camera_to_screen_sampler->data[i].get());
	}

	refreshMonitors();

	return true;
}

void RenderPipelineManager::setOutputCamera(ICameraComponent* camera)
{
	GAFF_ASSERT(!_output_cameras.empty());

	// Use the data from the render target information to find the display we belong to.
	auto windows = (camera->getRenderTarget()->any_display_with_tags) ?
		_render_mgr->getWindowsWithTagsAny(camera->getRenderTarget()->tags) :
		_render_mgr->getWindowsWithTags(camera->getRenderTarget()->tags);

	GAFF_ASSERT(windows.size() == 1);

	for (auto it = _output_cameras.begin(); it != _output_cameras.end(); ++it) {
		if (it->device == windows[0]->device && it->output == windows[0]->output) {
			it->camera = camera;
			break;
		}
	}
}

ICameraComponent* RenderPipelineManager::getOutputCamera(unsigned int monitor) const
{
	GAFF_ASSERT(monitor < _output_cameras.size());
	return _output_cameras[monitor].camera;
}

void RenderPipelineManager::refreshMonitors(void)
{
	const auto& windows = _render_mgr->getWindowData();
	_output_cameras.resize(windows.size());

	for (unsigned int i = 0; i < windows.size(); ++i) {
		_output_cameras[i].device = windows[i].device;
		_output_cameras[i].output = windows[i].output;
	}
}

size_t RenderPipelineManager::getActivePipeline(void) const
{
	return _active_pipeline;
}

void RenderPipelineManager::setActivePipeline(size_t pipeline)
{
	GAFF_ASSERT(pipeline < _pipelines.size());
	_active_pipeline = pipeline;
}

size_t RenderPipelineManager::getPipelineIndex(const char* name) const
{
	for (size_t i = 0; i < _pipelines.size(); ++i) {
		if (_pipelines[i]->getName() == name) {
			return i;
		}
	}

	return SIZE_T_FAIL;
}

bool RenderPipelineManager::addRenderPipelines(DynamicLoader::ModulePtr& module)
{
	GAFF_ASSERT(module.valid());
	CreateRenderPipelineFunc create_pipeline = module->getFunc<CreateRenderPipelineFunc>("CreateRenderPipeline");
	DestroyRenderPipelineFunc destroy_pipeline = module->getFunc<DestroyRenderPipelineFunc>("DestroyRenderPipeline");
	GetNumRenderPipelinesFunc num_pipelines = module->getFunc<GetNumRenderPipelinesFunc>("GetNumRenderPipelines");

	InitFunc init_func = module->getFunc<InitFunc>("InitModule");

	if (!init_func) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Could not find function named 'InitModule'.\n");
		return false;
	}

	if (!create_pipeline) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Could not find function named 'CreateRenderPipeline'.\n");
		return false;
	}

	if (!destroy_pipeline) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Could not find function named 'DestroyRenderPipeline'.\n");
		return false;
	}

	if (!num_pipelines) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Could not find function named 'GetNumRenderPipelines'.\n");
		return false;
	}

	if (!init_func(GetApp())) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Failed to initialize render pipeline module.\n");
	}

	for (size_t i = 0; i < num_pipelines(); ++i) {
		IRenderPipeline* pipeline = create_pipeline(i);

		if (!pipeline) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "Failed to create pipeline with ID '%zu'", i);
			return false;
		}

		_pipeline_map[pipeline->getName()] = destroy_pipeline;
		_pipelines.push(pipeline);

		GetApp().getLogManager().logMessage(LogManager::LOG_NORMAL, GetApp().getLogFileName(), "Loaded render pipeline '%s'\n", pipeline->getName());
	}

	return true;
}

void RenderPipelineManager::generateCommandLists(double dt, void* frame_data)
{
	_pipelines[_active_pipeline]->run(dt, frame_data);
}

void RenderPipelineManager::renderToScreen(double, void*)
{
	Gleam::IRenderDevice& rd = _render_mgr->getRenderDevice();

	for (size_t i = 0; i < _output_cameras.size(); ++i) {
		CameraData& camera_data = _output_cameras[i];

		if (!camera_data.camera) {
			continue;
		}

		auto& program_buffers = _camera_to_screen_program_buffers->data[camera_data.device];
		auto& render_target = camera_data.camera->getRenderTarget();
		auto* texture_srvs = render_target->texture_srvs[camera_data.device];
		auto& depth_stencil_srv = render_target->depth_stencil_srvs[camera_data.device];

		size_t num_g_buffers = render_target->texture_srvs.width();

		// Add the g-buffers
		for (size_t j = 0; j < num_g_buffers; ++j) {
			program_buffers->addResourceView(Gleam::IShader::SHADER_PIXEL, texture_srvs[j].get());
		}

		if (depth_stencil_srv) {
			program_buffers->addResourceView(Gleam::IShader::SHADER_PIXEL, depth_stencil_srv.get());
			++num_g_buffers;
		}

		// Render the result to the screen
		rd.setCurrentDevice(camera_data.device);
		rd.setCurrentOutput(camera_data.output);

		rd.beginFrame();

		rd.getActiveOutputRenderTarget()->bind(rd);
		_camera_to_screen_shader->programs[camera_data.device]->bind(rd);
		program_buffers->bind(rd);

		rd.renderNoVertexInput(3); // Render fullscreen quad

		rd.endFrame();

		// Pop the g-buffers
		program_buffers->popResourceView(Gleam::IShader::SHADER_PIXEL, num_g_buffers);
	}
}

NS_END
