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

#include "Shibboleth_RenderManager.h"
#include "Shibboleth_IFrameManager.h"
#include <Shibboleth_ModelComponent.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

#include <Gleam_RawInputRegisterFunction.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_Global.h>

#include <Gaff_Utils.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

REF_IMPL_REQ(RenderManager);
SHIB_REF_IMPL(RenderManager)
.addBaseClassInterfaceOnly<RenderManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IRenderManager)
;

static DisplayTags g_display_tags_values[] = {
	DT_1, DT_2, DT_3, DT_4, DT_5, DT_6, DT_7, DT_8, DT_9, DT_10,
	DT_11, DT_12, DT_13, DT_14, DT_15, DT_16
};

#define GRAPHICS_FUNCTION_IMPL(Class, Func) \
	Gleam::I##Class* RenderManager::create##Class(void) \
	{ \
		GAFF_ASSERT(_graphics_functions.Func); \
		return _graphics_functions.Func(); \
	}


RenderManager::RenderManager(void):
	_render_device(nullptr, ProxyAllocator("Graphics")),
	_proxy_allocator("Graphics"), _app(GetApp())
{
	memset(&_graphics_functions, 0, sizeof(GraphicsFunctions));
}

RenderManager::~RenderManager(void)
{
	_deferred_devices.clear();
	_render_device = nullptr;

	for (auto it = _windows.begin(); it != _windows.end(); ++it) {
		Gleam::IWindow* window = it->window;
		it->window = nullptr;
		SHIB_FREET(window, _proxy_allocator);
	}

	_windows.clear();

	if (_graphics_functions.shutdown) {
		_graphics_functions.shutdown();
	}
}

const char* RenderManager::getName(void) const
{
	return GetFriendlyName();
}

bool RenderManager::initThreadData(void)
{
	Array<unsigned int> thread_ids;
	_app.getWorkerThreadIDs(thread_ids);

	if (!_render_device->initThreadData(thread_ids.getArray(), thread_ids.size())) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to create render device thread data.\n");
		return false;
	}

	_deferred_devices.reserve(thread_ids.size());
	_context_locks.resize(_render_device->getNumDevices());

	for (size_t j = 0; j < thread_ids.size(); ++j) {
		auto& drds = _deferred_devices[thread_ids[j]];
		drds.reserve(_render_device->getNumDevices());

		for (unsigned int i = 0; i < _render_device->getNumDevices(); ++i) {
			_render_device->setCurrentDevice(i);
			drds.emplacePush(_render_device->createDeferredRenderDevice(), _proxy_allocator);
		}
	}

	return true;
}

bool RenderManager::init(const char* module)
{
	Gleam::SetAllocator(&_proxy_allocator);

	_app.getLogManager().logMessage(LogManager::LOG_NORMAL, _app.getLogFileName(), "Initializing Render Manager...\n");

	if (!cacheGleamFunctions(_app, module)) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to cache Gleam functions.\n");
		return false;
	}

	_render_device = _graphics_functions.create_renderdevice();

	if (!_render_device) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to create render device.\n");
		return false;
	}

	_windows.reserve(8); // Reserve 8 windows to be safe. Will probably never hit this many though.

	getDisplayTags();

	return true;
}

Array<RenderDevicePtr>& RenderManager::getDeferredRenderDevices(unsigned int thread_id)
{
	GAFF_ASSERT(_deferred_devices.hasElementWithKey(thread_id));
	return _deferred_devices[thread_id];
}

Gleam::IRenderDevice& RenderManager::getRenderDevice(void)
{
	return *_render_device;
}

Array<Gaff::SpinLock>& RenderManager::getContextLocks(void)
{
	return _context_locks;
}

bool RenderManager::createWindow(
	const char* app_name, Gleam::IWindow::MODE window_mode,
	int x, int y, unsigned int width, unsigned int height,
	unsigned int refresh_rate, const char* device_name,
	unsigned int adapter_id, unsigned int display_id, bool vsync,
	unsigned short tags)
{
	GAFF_ASSERT(_render_device && _graphics_functions.create_window);

	Gleam::IWindow* window = _graphics_functions.create_window();

	if (!window) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to allocate memory for Window!\n");
		return false;
	}

	if (!window->init(app_name, window_mode, width, height, x, y, device_name)) {
		_app.getLogManager().logMessage(
			LogManager::LOG_ERROR, _app.getLogFileName(),
			"Failed to initialize window with settings\n"
			"X: %i\n"
			"Y: %i\n"
			"Width: %i\n"
			"Height: %i\n"
			"Device Name: %s\n",
			x, y, width, height, device_name
		);

		SHIB_FREET(window, _proxy_allocator);
		return false;
	}

	int display_mode_id = getDisplayModeID(width, height, refresh_rate, adapter_id, display_id);

	if (display_mode_id < 0) {
		_app.getLogManager().logMessage(
			LogManager::LOG_ERROR, _app.getLogFileName(),
			"Failed to find display mode with settings\n"
			"Width: %u\n"
			"Height: %u\n"
			"Adapter ID: %u\n"
			"Display ID: %u\n",
			width, height, adapter_id, display_id
		);

		SHIB_FREET(window, _proxy_allocator);
		return false;
	}

	if (!_render_device->init(*window, adapter_id, display_id, static_cast<unsigned int>(display_mode_id), vsync)) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to initialize render device with window!\n");
		SHIB_FREET(window, _proxy_allocator);
		return false;
	}

	auto wmh_func = Gaff::Bind(this, &RenderManager::windowMessageHandler);
	window->addWindowMessageHandler(wmh_func);

	unsigned int device_id = static_cast<unsigned int>(_render_device->getDeviceForAdapter(adapter_id));

	WindowData wnd_data = { window, device_id, _render_device->getNumOutputs(device_id) - 1, tags };
	_windows.push(wnd_data);

	return Gleam::RegisterForRawInput(RAW_INPUT_KEYBOARD, *window) &&
			Gleam::RegisterForRawInput(RAW_INPUT_MOUSE, *window);
}

void RenderManager::updateWindows(void)
{
	_graphics_functions.update_windows();

	for (auto it = _remove_windows.begin(); it != _remove_windows.end(); ++it) {
		Gleam::IWindow* window = _windows[*it].window;
		_windows.fastErase(*it);
		SHIB_FREET(window, _proxy_allocator);
	}

	_remove_windows.clearNoFree();

	if (_windows.empty()) {
		_app.quit();
	}
}

Array<const RenderManager::WindowData*> RenderManager::getWindowsWithTagsAny(unsigned short tags) const
{
	Array<const WindowData*> out;

	for (auto it = _windows.begin(); it != _windows.end(); ++it) {
		if (it->tags & tags) {
			out.push(it);
		}
	}

	return out;
}

Array<const RenderManager::WindowData*> RenderManager::getWindowsWithTags(unsigned short tags) const
{
	Array<const WindowData*> out;

	for (auto it = _windows.begin(); it != _windows.end(); ++it) {
		if ((it->tags & tags) == tags) {
			out.push(it);
		}
	}

	return out;
}

Array<unsigned int> RenderManager::getDevicesWithTagsAny(unsigned short tags) const
{
	Array<const WindowData*> windows = getWindowsWithTagsAny(tags);
	Array<unsigned int> devices;

	for (auto it = windows.begin(); it != windows.end(); ++it) {
		if (devices.linearSearch((*it)->device) == devices.end()) {
			devices.emplacePush((*it)->device);
		}
	}

	return devices;
}

Array<unsigned int> RenderManager::getDevicesWithTags(unsigned short tags) const
{
	Array<const WindowData*> windows = getWindowsWithTags(tags);
	Array<unsigned int> devices;

	for (auto it = windows.begin(); it != windows.end(); ++it) {
		if (devices.linearSearch((*it)->device) == devices.end()) {
			devices.emplacePush((*it)->device);
		}
	}

	return devices;
}

size_t RenderManager::getNumWindows(void) const
{
	return _windows.size();
}

const RenderManager::WindowData& RenderManager::getWindowData(unsigned int window) const
{
	GAFF_ASSERT(window < _windows.size());
	return _windows[window];
}

const Array<RenderManager::WindowData>& RenderManager::getWindowData(void) const
{
	return _windows;
}

const char* RenderManager::getShaderExtension(void) const
{
	GAFF_ASSERT(_graphics_functions.get_shader_extension);
	return _graphics_functions.get_shader_extension();
}

GRAPHICS_FUNCTION_IMPL(ShaderResourceView, create_shaderresourceview);
GRAPHICS_FUNCTION_IMPL(DepthStencilState, create_depthstencilstate);
GRAPHICS_FUNCTION_IMPL(ProgramBuffers, create_programbuffers);
GRAPHICS_FUNCTION_IMPL(RenderDevice, create_renderdevice);
GRAPHICS_FUNCTION_IMPL(RenderTarget, create_rendertarget);
GRAPHICS_FUNCTION_IMPL(SamplerState, create_samplerstate);
GRAPHICS_FUNCTION_IMPL(RasterState, create_rasterstate);
GRAPHICS_FUNCTION_IMPL(CommandList, create_commandlist);
GRAPHICS_FUNCTION_IMPL(BlendState, create_blendstate);
GRAPHICS_FUNCTION_IMPL(Texture, create_texture);
GRAPHICS_FUNCTION_IMPL(Layout, create_layout);
GRAPHICS_FUNCTION_IMPL(Program, create_program);
GRAPHICS_FUNCTION_IMPL(Shader, create_shader);
GRAPHICS_FUNCTION_IMPL(Buffer, create_buffer);
GRAPHICS_FUNCTION_IMPL(Model, create_model);
GRAPHICS_FUNCTION_IMPL(Mesh, create_mesh);
GRAPHICS_FUNCTION_IMPL(Keyboard, create_keyboard);
GRAPHICS_FUNCTION_IMPL(Mouse, create_mouse);

RenderManager::WindowRenderTargets RenderManager::createRenderTargetsForEachWindow(void)
{
	WindowRenderTargets wrt;

	wrt.rts.reserve(_windows.size());

	wrt.diffuse.reserve(_windows.size());
	wrt.specular.reserve(_windows.size());
	wrt.normal.reserve(_windows.size());
	wrt.position.reserve(_windows.size());
	wrt.depth.reserve(_windows.size());

	wrt.diffuse_srvs.reserve(_windows.size());
	wrt.specular_srvs.reserve(_windows.size());
	wrt.normal_srvs.reserve(_windows.size());
	wrt.position_srvs.reserve(_windows.size());
	wrt.depth_srvs.reserve(_windows.size());

	for (auto it = _windows.begin(); it != _windows.end(); ++it) {
		_render_device->setCurrentDevice(it->device);

		RenderTargetPtr rt(_graphics_functions.create_rendertarget());
		TexturePtr tex[4];
		SRVPtr srv[4];

		TexturePtr depth;
		SRVPtr depth_srv;

		if (!rt) {
			// log error
			return WindowRenderTargets();
		}

		// Create each of the g-buffers
		for (size_t i = 0; i < sizeof(tex) / sizeof(TexturePtr); ++i) {
			if (!createWindowRenderable(it->window->getWidth(), it->window->getHeight(), Gleam::ITexture::RGBA_8_UNORM, tex[i], srv[i])) {
				// log error
				return WindowRenderTargets();
			}

			if (!rt->addTexture(*_render_device, tex[i].get())) {
				// log error
				return WindowRenderTargets();
			}
		}

		if (!createWindowRenderable(it->window->getWidth(), it->window->getHeight(), Gleam::ITexture::DEPTH_STENCIL_24_8_UNORM_UI, depth, depth_srv)) {
			// log error
			return WindowRenderTargets();
		}

		if (!rt->addDepthStencilBuffer(*_render_device, depth.get())) {
			// log error
			return WindowRenderTargets();
		}

		wrt.rts.emplacePush(std::move(rt));
		wrt.diffuse.emplacePush(std::move(tex[0]));
		wrt.specular.emplacePush(std::move(tex[1]));
		wrt.normal.emplacePush(std::move(tex[2]));
		wrt.position.emplacePush(std::move(tex[3]));
		wrt.depth.emplacePush(std::move(depth));

		wrt.diffuse_srvs.emplacePush(std::move(srv[0]));
		wrt.specular_srvs.emplacePush(std::move(srv[1]));
		wrt.normal_srvs.emplacePush(std::move(srv[2]));
		wrt.position_srvs.emplacePush(std::move(srv[3]));
		wrt.depth_srvs.emplacePush(std::move(depth_srv));
	}

	return wrt;
}

Array<RasterStatePtr>& RenderManager::getOrCreateRasterStates(unsigned int hash, const Gleam::IRasterState::RasterStateSettings& settings)
{
	_rs_lock.lock();

	Array<RasterStatePtr>& raster_states = _raster_states[hash];
	raster_states.reserve(_render_device->getNumDevices());

	if (raster_states.empty()) {
		for (unsigned int i = 0; i < _render_device->getNumDevices(); ++i) {
			_render_device->setCurrentDevice(i);
			Gleam::IRasterState* rs = _graphics_functions.create_rasterstate();

			if (!rs || !rs->init(*_render_device, settings)) {
				raster_states.clear();
				return raster_states;
			}

			raster_states.emplacePush(rs);
		}
	}

	_rs_lock.unlock();

	return raster_states;
}

int RenderManager::getDisplayModeID(unsigned int width, unsigned int height, unsigned int refresh_rate, unsigned int adapter_id, unsigned int display_id)
{
	Gleam::IRenderDevice::AdapterList adpt_list = _render_device->getDisplayModes();

	auto adpt_it = adpt_list.linearSearch(adapter_id, [](const Gleam::IRenderDevice::Adapter& lhs, unsigned int rhs) -> bool
	{
		return lhs.id == rhs;
	});

	if (adpt_it == adpt_list.end()) {
		return -1;
	}

	auto display_it = adpt_it->displays.linearSearch(display_id, [](const Gleam::IRenderDevice::Display& lhs, unsigned int rhs) -> bool
	{
		return lhs.id == rhs;
	});

	if (display_it == adpt_it->displays.end()) {
		return -1;
	}

	auto dm_it = display_it->display_modes.begin();

	for (; dm_it != display_it->display_modes.end(); ++dm_it) {
		if (dm_it->width == width && dm_it->height == height && dm_it->refresh_rate == refresh_rate) {
			break;
		}
	}

	if (dm_it == display_it->display_modes.end()) {
		return -1;
	}

	return dm_it->id;
}

// Still single-threaded at this point, so ok that we're not using the spinlock
bool RenderManager::cacheGleamFunctions(IApp& app, const char* module)
{
	AString module_path(module);
	module_path += BIT_EXTENSION;

	_gleam_module = app.loadModule(module_path.getBuffer(), module);

	if (!_gleam_module) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	// cache graphics function pointers
	GraphicsFunctions::InitGraphics init_graphics = _gleam_module->getFunc<GraphicsFunctions::InitGraphics>("InitGraphics");

	if (!init_graphics) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'InitGraphics' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	char log_file_name[64] = { 0 };
	Gaff::GetCurrentTimeString(log_file_name, 64, "logs/GleamLog %Y-%m-%d %H-%M-%S.txt");

	if (!Gaff::CreateDir("./logs", 0777)) {
		return false;
	}

	if (!init_graphics(app, log_file_name)) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Graphics module '%s' failed to initialize.", module_path.getBuffer());
		return false;
	}

	bool ret = true;

	_graphics_functions.shutdown = _gleam_module->getFunc<GraphicsFunctions::ShutdownGraphics>("ShutdownGraphics");
	_graphics_functions.create_window = _gleam_module->getFunc<GraphicsFunctions::CreateWindowS>("CreateWindowS");
	_graphics_functions.update_windows = _gleam_module->getFunc<GraphicsFunctions::UpdateWindows>("UpdateWindows");
	_graphics_functions.get_shader_extension = _gleam_module->getFunc<GraphicsFunctions::GetShaderExtension>("GetShaderExtension");
	_graphics_functions.create_shaderresourceview = _gleam_module->getFunc<GraphicsFunctions::CreateShaderResourceView>("CreateShaderResourceView");
	_graphics_functions.create_depthstencilstate = _gleam_module->getFunc<GraphicsFunctions::CreateDepthStencilState>("CreateDepthStencilState");
	_graphics_functions.create_programbuffers = _gleam_module->getFunc<GraphicsFunctions::CreateProgramBuffers>("CreateProgramBuffers");
	_graphics_functions.create_renderdevice = _gleam_module->getFunc<GraphicsFunctions::CreateRenderDevice>("CreateRenderDevice");
	_graphics_functions.create_rendertarget = _gleam_module->getFunc<GraphicsFunctions::CreateRenderTarget>("CreateRenderTarget");
	_graphics_functions.create_samplerstate = _gleam_module->getFunc<GraphicsFunctions::CreateSamplerState>("CreateSamplerState");
	_graphics_functions.create_rasterstate = _gleam_module->getFunc<GraphicsFunctions::CreateRasterState>("CreateRasterState");
	_graphics_functions.create_commandlist = _gleam_module->getFunc<GraphicsFunctions::CreateCommandList>("CreateCommandList");
	_graphics_functions.create_blendstate = _gleam_module->getFunc<GraphicsFunctions::CreateBlendState>("CreateBlendState");
	_graphics_functions.create_texture = _gleam_module->getFunc<GraphicsFunctions::CreateTexture>("CreateTexture");
	_graphics_functions.create_layout = _gleam_module->getFunc<GraphicsFunctions::CreateLayout>("CreateLayout");
	_graphics_functions.create_program = _gleam_module->getFunc<GraphicsFunctions::CreateProgram>("CreateProgram");
	_graphics_functions.create_shader = _gleam_module->getFunc<GraphicsFunctions::CreateShader>("CreateShader");
	_graphics_functions.create_buffer = _gleam_module->getFunc<GraphicsFunctions::CreateBuffer>("CreateBuffer");
	_graphics_functions.create_model = _gleam_module->getFunc<GraphicsFunctions::CreateModel>("CreateModel");
	_graphics_functions.create_mesh = _gleam_module->getFunc<GraphicsFunctions::CreateMesh>("CreateMesh");
	_graphics_functions.create_keyboard = _gleam_module->getFunc<GraphicsFunctions::CreateKeyboard>("CreateKeyboard");
	_graphics_functions.create_mouse = _gleam_module->getFunc<GraphicsFunctions::CreateMouse>("CreateMouse");

	if (!_graphics_functions.shutdown) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'ShutdownGraphics' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_window) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateWindowS' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.update_windows) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'UpdateWindows' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.get_shader_extension) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'GetShaderExtension' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_shaderresourceview) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateShaderResourceView' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_depthstencilstate) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateDepthStencilState' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_programbuffers) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateProgramBuffers' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_renderdevice) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateRenderDevice' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_rendertarget) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateRenderTarget' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_samplerstate) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateSamplerState' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_rasterstate) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateRasterState' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_commandlist) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateCommandList' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_blendstate) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateBlendState' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_texture) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateTexture' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_layout) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateLayout' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_program) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateProgram' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_shader) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateShader' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_buffer) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateBuffer' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_model) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateModel' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_mesh) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateMesh' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_keyboard) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateKeyboard' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	if (!_graphics_functions.create_mouse) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to find function 'CreateMouse' in graphics module '%s'.", module_path.getBuffer());
		ret = false;
	}

	return ret;
}

bool RenderManager::createWindowRenderable(int width, int height, Gleam::ITexture::FORMAT format, TexturePtr& tex_out, SRVPtr& srv_out)
{
	srv_out = _graphics_functions.create_shaderresourceview();
	tex_out = _graphics_functions.create_texture();

	if (!tex_out) {
		// log error
		return false;
	}

	if (!srv_out) {
		// log error
		return false;
	}

	if (!tex_out->init2D(*_render_device, width, height, format)) {
		// log error
		return false;
	}

	if (!srv_out->init(*_render_device, tex_out.get())) {
		// log error
		return false;
	}

	return true;
}

bool RenderManager::getDisplayTags(void)
{
	IFile* file = _app.getFileSystem()->openFile("Resources/display_tags.json");

	if (file) {
		_app.getLogManager().logMessage(LogManager::LOG_NORMAL, _app.getLogFileName(), "Loading names for Display Tags from 'Resources/display_tags.json'.\n");

		Gaff::JSON display_tags;

		if (!display_tags.parse(file->getBuffer())) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Could not parse 'Resources/display_tags.json'.\n");
			_app.getFileSystem()->closeFile(file);
			return false;
		}

		_app.getFileSystem()->closeFile(file);

		if (!display_tags.isArray()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "'Resources/display_tags.json' is improperly formatted. Root object is not an array.\n");
			return false;
		}

		// Reset reflection definition
		EnumReflectionDefinition<DisplayTags>& dp_ref_def = const_cast<EnumReflectionDefinition<DisplayTags>&>(GetEnumRefDef<DisplayTags>());
		dp_ref_def = Gaff::EnumRefDef<DisplayTags, ProxyAllocator>("DisplayTags", ProxyAllocator("Reflection"));

		bool ret = display_tags.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
		{
			if (!value.isString()) {
				_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Index '%zu' of 'Resources/display_tags.json' is not a string.\n", index);
				return true;
			}

			dp_ref_def.addValue(value.getString(), g_display_tags_values[index]);

			return false;
		});

		if (ret) {
			return false;
		}

	} else {
		_app.getLogManager().logMessage(LogManager::LOG_NORMAL, _app.getLogFileName(), "Using default names for Display Tags.\n");
	}

	return true;
}

bool RenderManager::windowMessageHandler(const Gleam::AnyMessage& msg)
{
	switch (msg.base.type) {
		case Gleam::WND_DESTROYED:
			for (size_t i = 0; i < _windows.size(); ++i) {
				if (_windows[i].window == msg.base.window) {
					_remove_windows.emplacePush(i);
					break;
				}
			}

			return true;

		default:
			break;
	}

	return false;
}

NS_END
