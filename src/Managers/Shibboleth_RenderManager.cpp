/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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
#include "Shibboleth_ResourceManager.h"
#include "Shibboleth_FrameManager.h"
#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_ModelComponent.h>
#include <Shibboleth_TextureLoader.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>

#include <Gleam_IRenderDevice.h>
#include <Gleam_Global.h>

#include <Gaff_ScopedLock.h>
#include <Gaff_Utils.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

REF_IMPL_REQ(RenderManager);
SHIB_REF_IMPL(RenderManager)
.addBaseClassInterfaceOnly<RenderManager>()
;

// Default values
SHIB_ENUM_REF_IMPL(DisplayTags)
.addValue("DT_1", DT_1)
.addValue("DT_2", DT_2)
.addValue("DT_3", DT_3)
.addValue("DT_4", DT_4)
.addValue("DT_5", DT_5)
.addValue("DT_6", DT_6)
.addValue("DT_7", DT_7)
.addValue("DT_8", DT_8)
.addValue("DT_9", DT_9)
.addValue("DT_10", DT_10)
.addValue("DT_11", DT_11)
.addValue("DT_12", DT_12)
.addValue("DT_13", DT_13)
.addValue("DT_14", DT_14)
.addValue("DT_15", DT_15)
.addValue("DT_16", DT_16)
;

SHIB_ENUM_REF_IMPL(RenderModes)
.addValue("None", RM_NONE)
.addValue("Opaque", RM_OPAQUE)
.addValue("Transparent", RM_TRANSPARENT)
.addValue("User1", RM_USER1)
.addValue("User2", RM_USER2)
.addValue("User3", RM_USER3)
.addValue("User4", RM_USER4)
.addValue("User5", RM_USER5)
.addValue("User6", RM_USER6)
.addValue("User7", RM_USER7)
.addValue("User8", RM_USER8)
.addValue("User9", RM_USER9)
.addValue("User10", RM_USER10)
.addValue("User11", RM_USER11)
.addValue("User12", RM_USER12)
.addValue("User13", RM_USER13)
.addValue("User14", RM_USER14)
.addValue("User15", RM_USER15)
.addValue("User16", RM_USER16)
;

static DisplayTags gDisplayTagsValues[] = {
	DT_1, DT_2, DT_3, DT_4, DT_5, DT_6, DT_7, DT_8, DT_9, DT_10,
	DT_11, DT_12, DT_13, DT_14, DT_15, DT_16
};

SHIB_ENUM_REF_IMPL_EMBEDDED(Gleam_ITexture_Format, Gleam::ITexture::FORMAT)
.addValue("R_8_UNORM", Gleam::ITexture::R_8_UNORM)
.addValue("R_16_UNORM", Gleam::ITexture::R_16_UNORM)
.addValue("RG_8_UNORM", Gleam::ITexture::RG_8_UNORM)
.addValue("RG_16_UNORM", Gleam::ITexture::RG_16_UNORM)
.addValue("RGBA_8_UNORM", Gleam::ITexture::RGBA_8_UNORM)
.addValue("RGBA_16_UNORM", Gleam::ITexture::RGBA_16_UNORM)
.addValue("R_8_SNORM", Gleam::ITexture::R_8_UNORM)
.addValue("R_16_SNORM", Gleam::ITexture::R_16_SNORM)
.addValue("RG_8_SNORM", Gleam::ITexture::RG_8_SNORM)
.addValue("RG_16_SNORM", Gleam::ITexture::RG_16_SNORM)
.addValue("RGBA_8_SNORM", Gleam::ITexture::RGBA_8_SNORM)
.addValue("R_8_I", Gleam::ITexture::R_8_I)
.addValue("R_16_I", Gleam::ITexture::R_16_I)
.addValue("R_32_I", Gleam::ITexture::R_32_I)
.addValue("RG_8_I", Gleam::ITexture::RG_8_I)
.addValue("RG_16_I", Gleam::ITexture::RG_16_I)
.addValue("RG_32_I", Gleam::ITexture::RG_32_I)
.addValue("RGB_32_I", Gleam::ITexture::RGB_32_I)
.addValue("RGBA_8_I", Gleam::ITexture::RGBA_8_I)
.addValue("RGBA_16_I", Gleam::ITexture::RGBA_16_I)
.addValue("RGBA_32_I", Gleam::ITexture::RGBA_32_I)
.addValue("R_8_UI", Gleam::ITexture::R_8_UI)
.addValue("R_16_UI", Gleam::ITexture::R_16_UI)
.addValue("R_32_UI", Gleam::ITexture::R_32_UI)
.addValue("RG_8_UI", Gleam::ITexture::RG_8_UI)
.addValue("RG_16_UI", Gleam::ITexture::RG_16_UI)
.addValue("RG_32_UI", Gleam::ITexture::RG_32_UI)
.addValue("RGB_32_UI", Gleam::ITexture::RGB_32_UI)
.addValue("RGBA_8_UI", Gleam::ITexture::RGBA_8_UI)
.addValue("RGBA_16_UI", Gleam::ITexture::RGBA_16_UI)
.addValue("RGBA_32_UI", Gleam::ITexture::RGBA_32_UI)
.addValue("R_16_F", Gleam::ITexture::R_16_F)
.addValue("R_32_F", Gleam::ITexture::R_32_F)
.addValue("RG_16_F", Gleam::ITexture::RG_16_F)
.addValue("RG_32_F", Gleam::ITexture::RG_32_F)
.addValue("RGB_16_F", Gleam::ITexture::RGB_16_F)
.addValue("RGB_32_F", Gleam::ITexture::RGB_32_F)
.addValue("RGBA_16_F", Gleam::ITexture::RGBA_16_F)
.addValue("RGBA_32_F", Gleam::ITexture::RGBA_32_F)
.addValue("RGB_11_11_10_F", Gleam::ITexture::RGB_11_11_10_F)
.addValue("RGBE_9_9_9_5", Gleam::ITexture::RGBE_9_9_9_5)
.addValue("RGBA_10_10_10_2_UNORM", Gleam::ITexture::RGBA_10_10_10_2_UNORM)
.addValue("RGBA_10_10_10_2_UI", Gleam::ITexture::RGBA_10_10_10_2_UI)
.addValue("SRGBA_8_UNORM", Gleam::ITexture::SRGBA_8_UNORM)
.addValue("DEPTH_16_UNORM", Gleam::ITexture::DEPTH_16_UNORM)
.addValue("DEPTH_32_F", Gleam::ITexture::DEPTH_32_F)
.addValue("DEPTH_STENCIL_24_8_UNORM_UI", Gleam::ITexture::DEPTH_STENCIL_24_8_UNORM_UI)
.addValue("DEPTH_STENCIL_32_8_F", Gleam::ITexture::DEPTH_STENCIL_32_8_F)
;

SHIB_ENUM_REF_IMPL_EMBEDDED(Gleam_IShader_Type, Gleam::IShader::SHADER_TYPE)
.addValue("Vertex", Gleam::IShader::SHADER_VERTEX)
.addValue("Pixel", Gleam::IShader::SHADER_PIXEL)
.addValue("Domain", Gleam::IShader::SHADER_DOMAIN)
.addValue("Geometry", Gleam::IShader::SHADER_GEOMETRY)
.addValue("Hull", Gleam::IShader::SHADER_HULL)
;


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

	if (_graphics_functions.destroy_window) {
		for (auto it = _windows.begin(); it != _windows.end(); ++it) {
			_graphics_functions.destroy_window(it->window);
		}
	}

	_windows.clear();

	if (_graphics_functions.shutdown) {
		_graphics_functions.shutdown();
	}
}

const char* RenderManager::getName(void) const
{
	return "Render Manager";
}

bool RenderManager::initThreadData(void)
{
	Array<unsigned int> thread_ids;
	_app.getWorkerThreadIDs(thread_ids);

	if (!_render_device->initThreadData(thread_ids.getArray(), thread_ids.size())) {
		_app.getGameLogFile().first.writeString("ERROR - Failed to create render device thread data.\n");
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

	LogManager::FileLockPair& log = _app.getGameLogFile();
	log.first.writeString("Initializing Render Manager...\n");

	if (!cacheGleamFunctions(_app, module)) {
		log.first.writeString("ERROR - Failed to cache Gleam functions.\n");
		return false;
	}

	_render_device = _graphics_functions.create_renderdevice();

	if (!_render_device) {
		log.first.writeString("ERROR - Failed to create render device.\n");
		return false;
	}

	_windows.reserve(8); // Reserve 8 windows to be safe. Will probably never hit this many though.

	getDisplayTags();
	getRenderModes();

	return true;
}

Array<RenderManager::RenderDevicePtr>& RenderManager::getDeferredRenderDevices(unsigned int thread_id)
{
	assert(_deferred_devices.hasElementWithKey(thread_id));
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
	assert(_render_device && _graphics_functions.create_window && _graphics_functions.destroy_window);

	Gleam::IWindow* window = _graphics_functions.create_window();
	LogManager::FileLockPair& log = _app.getGameLogFile();

	if (!window) {
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(*log.second);
		log.first.writeString("ERROR - Failed to allocate memory for Window!\n");
		return false;
	}

	if (!window->init(app_name, window_mode, width, height, x, y, device_name)) {
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(*log.second);
		log.first.printf(
			"ERROR - Failed to initialize window with settings\n"
			"X: %i\n"
			"Y: %i\n"
			"Width: %i\n"
			"Height: %i\n"
			"Device Name: %s\n",
			x, y, width, height, device_name
		);

		_graphics_functions.destroy_window(window);
		return false;
	}

	int display_mode_id = getDisplayModeID(width, height, refresh_rate, adapter_id, display_id);

	if (display_mode_id < 0) {
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(*log.second);
		log.first.printf(
			"ERROR - Failed to find display mode with settings\n"
			"Width: %i\n"
			"Height: %i\n"
			"Adapter ID: %i\n"
			"Display ID: %i\n",
			width, height, adapter_id, display_id
		);

		_graphics_functions.destroy_window(window);
		return false;
	}

	if (!_render_device->init(*window, adapter_id, display_id, (unsigned int)display_mode_id, vsync)) {
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(*log.second);
		log.first.writeString("ERROR - Failed to initialize render device with window!\n");
		_graphics_functions.destroy_window(window);
		return false;
	}

	unsigned int device_id = static_cast<unsigned int>(_render_device->getDeviceForAdapter(adapter_id));

	WindowData wnd_data = { window, device_id, _render_device->getNumOutputs(device_id) - 1, tags };
	_windows.push(wnd_data);
	return true;
}

void RenderManager::updateWindows(void)
{
	_graphics_functions.update_windows();
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
	assert(window < _windows.size());
	return _windows[window];
}

const char* RenderManager::getShaderExtension(void) const
{
	assert(_graphics_functions.get_shader_extension);
	return _graphics_functions.get_shader_extension();
}

Gleam::IShaderResourceView* RenderManager::createShaderResourceView(void)
{
	assert(_graphics_functions.create_shaderresourceview);
	return _graphics_functions.create_shaderresourceview();
}

Gleam::IProgramBuffers* RenderManager::createProgramBuffers(void)
{
	assert(_graphics_functions.create_programbuffers);
	return _graphics_functions.create_programbuffers();
}

Gleam::IRenderDevice* RenderManager::createRenderDevice(void)
{
	assert(_graphics_functions.create_renderdevice);
	return _graphics_functions.create_renderdevice();
}

Gleam::IRenderTarget* RenderManager::createRenderTarget(void)
{
	assert(_graphics_functions.create_rendertarget);
	return _graphics_functions.create_rendertarget();
}

Gleam::ISamplerState* RenderManager::createSamplerState(void)
{
	assert(_graphics_functions.create_samplerstate);
	return _graphics_functions.create_samplerstate();
}

Gleam::IRenderState* RenderManager::createRenderState(void)
{
	assert(_graphics_functions.create_renderstate);
	return _graphics_functions.create_renderstate();
}

Gleam::ICommandList* RenderManager::createCommandList(void)
{
	assert(_graphics_functions.create_commandlist);
	return _graphics_functions.create_commandlist();
}

Gleam::ITexture* RenderManager::createTexture(void)
{
	assert(_graphics_functions.create_texture);
	return _graphics_functions.create_texture();
}

Gleam::ILayout* RenderManager::createLayout(void)
{
	assert(_graphics_functions.create_layout);
	return _graphics_functions.create_layout();
}

Gleam::IProgram* RenderManager::createProgram(void)
{
	assert(_graphics_functions.create_program);
	return _graphics_functions.create_program();
}

Gleam::IShader* RenderManager::createShader(void)
{
	assert(_graphics_functions.create_shader);
	return _graphics_functions.create_shader();
}

Gleam::IBuffer* RenderManager::createBuffer(void)
{
	assert(_graphics_functions.create_buffer);
	return _graphics_functions.create_buffer();
}

Gleam::IModel* RenderManager::createModel(void)
{
	assert(_graphics_functions.create_model);
	return _graphics_functions.create_model();
}

Gleam::IMesh* RenderManager::createMesh(void)
{
	assert(_graphics_functions.create_mesh);
	return _graphics_functions.create_mesh();
}

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

		wrt.rts.emplaceMovePush(Gaff::Move(rt));
		wrt.diffuse.emplacePush(Gaff::Move(tex[0]));
		wrt.specular.emplacePush(Gaff::Move(tex[1]));
		wrt.normal.emplacePush(Gaff::Move(tex[2]));
		wrt.position.emplacePush(Gaff::Move(tex[3]));
		wrt.depth.emplaceMovePush(Gaff::Move(depth));

		wrt.diffuse_srvs.emplacePush(Gaff::Move(srv[0]));
		wrt.specular_srvs.emplacePush(Gaff::Move(srv[1]));
		wrt.normal_srvs.emplacePush(Gaff::Move(srv[2]));
		wrt.position_srvs.emplacePush(Gaff::Move(srv[3]));
		wrt.depth_srvs.emplaceMovePush(Gaff::Move(depth_srv));
	}

	return wrt;
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
	LogManager::FileLockPair& log = _app.getGameLogFile();

	AString module_path(module);
	module_path += BIT_EXTENSION;

	_gleam_module = app.loadModule(module_path.getBuffer(), module);

	if (!_gleam_module) {
		log.first.printf("ERROR - Failed to find graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	// cache graphics function pointers
	GraphicsFunctions::InitGraphics init_graphics = _gleam_module->GetFunc<GraphicsFunctions::InitGraphics>("InitGraphics");

	if (!init_graphics) {
		log.first.printf("ERROR - Failed to find function 'InitGraphics' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	char log_file_name[64] = { 0 };
	Gaff::GetCurrentTimeString(log_file_name, 64, "Logs/GleamLog %Y-%m-%d %H-%M-%S.txt");

	if (!Gaff::CreateDir("./Logs", 0777)) {
		return false;
	}

	if (!init_graphics(app, log_file_name)) {
		log.first.printf("ERROR - Graphics module '%s' failed to initialize.", module_path.getBuffer());
		return false;
	}

	_graphics_functions.shutdown = _gleam_module->GetFunc<GraphicsFunctions::ShutdownGraphics>("ShutdownGraphics");

	if (!_graphics_functions.shutdown) {
		log.first.printf("ERROR - Failed to find function 'ShutdownGraphics' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	_graphics_functions.create_window = _gleam_module->GetFunc<GraphicsFunctions::CreateWindow>("CreateWindowS");
	_graphics_functions.destroy_window = _gleam_module->GetFunc<GraphicsFunctions::DestroyWindow>("DestroyWindowS");
	_graphics_functions.update_windows = _gleam_module->GetFunc<GraphicsFunctions::UpdateWindows>("UpdateWindows");
	_graphics_functions.get_shader_extension = _gleam_module->GetFunc<GraphicsFunctions::GetShaderExtension>("GetShaderExtension");
	_graphics_functions.create_shaderresourceview = _gleam_module->GetFunc<GraphicsFunctions::CreateShaderResourceView>("CreateShaderResourceView");
	_graphics_functions.create_programbuffers = _gleam_module->GetFunc<GraphicsFunctions::CreateProgramBuffers>("CreateProgramBuffers");
	_graphics_functions.create_renderdevice = _gleam_module->GetFunc<GraphicsFunctions::CreateRenderDevice>("CreateRenderDevice");
	_graphics_functions.create_rendertarget = _gleam_module->GetFunc<GraphicsFunctions::CreateRenderTarget>("CreateRenderTarget");
	_graphics_functions.create_samplerstate = _gleam_module->GetFunc<GraphicsFunctions::CreateSamplerState>("CreateSamplerState");
	_graphics_functions.create_renderstate = _gleam_module->GetFunc<GraphicsFunctions::CreateRenderState>("CreateRenderState");
	_graphics_functions.create_commandlist = _gleam_module->GetFunc<GraphicsFunctions::CreateCommandList>("CreateCommandList");
	_graphics_functions.create_texture = _gleam_module->GetFunc<GraphicsFunctions::CreateTexture>("CreateTexture");
	_graphics_functions.create_layout = _gleam_module->GetFunc<GraphicsFunctions::CreateLayout>("CreateLayout");
	_graphics_functions.create_program = _gleam_module->GetFunc<GraphicsFunctions::CreateProgram>("CreateProgram");
	_graphics_functions.create_shader = _gleam_module->GetFunc<GraphicsFunctions::CreateShader>("CreateShader");
	_graphics_functions.create_buffer = _gleam_module->GetFunc<GraphicsFunctions::CreateBuffer>("CreateBuffer");
	_graphics_functions.create_model = _gleam_module->GetFunc<GraphicsFunctions::CreateModel>("CreateModel");
	_graphics_functions.create_mesh = _gleam_module->GetFunc<GraphicsFunctions::CreateMesh>("CreateMesh");

	if (!_graphics_functions.create_window) {
		log.first.printf("ERROR - Failed to find function 'CreateWindowS' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.destroy_window) {
		log.first.printf("ERROR - Failed to find function 'DestroyWindowS' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.update_windows) {
		log.first.printf("ERROR - Failed to find function 'UpdateWindows' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.get_shader_extension) {
		log.first.printf("ERROR - Failed to find function 'GetShaderExtension' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_shaderresourceview) {
		log.first.printf("ERROR - Failed to find function 'CreateShaderResourceView' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_programbuffers) {
		log.first.printf("ERROR - Failed to find function 'CreateProgramBuffers' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_renderdevice) {
		log.first.printf("ERROR - Failed to find function 'CreateRenderDevice' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_rendertarget) {
		log.first.printf("ERROR - Failed to find function 'CreateRenderTarget' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_samplerstate) {
		log.first.printf("ERROR - Failed to find function 'CreateSamplerState' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_renderstate) {
		log.first.printf("ERROR - Failed to find function 'CreateRenderState' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_texture) {
		log.first.printf("ERROR - Failed to find function 'CreateTexture' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_layout) {
		log.first.printf("ERROR - Failed to find function 'CreateLayout' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_program) {
		log.first.printf("ERROR - Failed to find function 'CreateProgram' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_shader) {
		log.first.printf("ERROR - Failed to find function 'CreateShader' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_buffer) {
		log.first.printf("ERROR - Failed to find function 'CreateBuffer' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_model) {
		log.first.printf("ERROR - Failed to find function 'CreateModel' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	if (!_graphics_functions.create_mesh) {
		log.first.printf("ERROR - Failed to find function 'CreateMesh' in graphics module '%s'.", module_path.getBuffer());
		return false;
	}

	return true;
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
	LogManager::FileLockPair& log = _app.getGameLogFile();

	if (file) {
		log.first.writeString("Loading names for Display Tags from 'Resources/display_tags.json'.\n");

		Gaff::JSON display_tags;

		if (!display_tags.parse(file->getBuffer())) {
			log.first.writeString("ERROR - Could not parse 'Resources/display_tags.json'.\n");
			_app.getFileSystem()->closeFile(file);
			return false;
		}

		_app.getFileSystem()->closeFile(file);

		if (!display_tags.isArray()) {
			log.first.writeString("ERROR - 'Resources/display_tags.json' is improperly formatted. Root object is not an array.\n");
			return false;
		}

		// Reset reflection definition
		EnumReflectionDefinition<DisplayTags>& dp_ref_def = const_cast<EnumReflectionDefinition<DisplayTags>&>(GetEnumRefDef<DisplayTags>());
		dp_ref_def = Gaff::EnumRefDef<DisplayTags, ProxyAllocator>("DisplayTags", ProxyAllocator("Reflection"));

		bool ret = display_tags.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
		{
			if (!value.isString()) {
				log.first.printf("ERROR - Index '%i' of 'Resources/display_tags.json' is not a string.\n", index);
				return true;
			}

			dp_ref_def.addValue(value.getString(), gDisplayTagsValues[index]);

			return false;
		});

		if (ret) {
			return false;
		}

	} else {
		log.first.writeString("Using default names for Display Tags.\n");
	}

	return true;
}

bool RenderManager::getRenderModes(void)
{
	IFile* file = _app.getFileSystem()->openFile("Resources/render_modes.json");
	LogManager::FileLockPair& log = _app.getGameLogFile();

	if (file) {
		log.first.writeString("Loading Render Modes from 'Resources/render_modes.json'.\n");

		Gaff::JSON render_modes;

		if (!render_modes.parse(file->getBuffer())) {
			log.first.writeString("ERROR - Could not parse 'Resources/render_modes.json'.\n");
			_app.getFileSystem()->closeFile(file);
			return false;
		}

		_app.getFileSystem()->closeFile(file);

		if (!render_modes.isArray()) {
			log.first.writeString("ERROR - 'Resources/render_modes.json' is improperly formatted. Root object is not an array.\n");
			return false;
		}

		// Reset reflection definition
		EnumReflectionDefinition<RenderModes>& rm_ref_def = const_cast<EnumReflectionDefinition<RenderModes>&>(GetEnumRefDef<RenderModes>());
		rm_ref_def = Gaff::EnumRefDef<RenderModes, ProxyAllocator>("RenderModes", ProxyAllocator("Reflection"));

		rm_ref_def.addValue("None", RM_NONE);

		bool ret = render_modes.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
		{
			if (!value.isString()) {
				log.first.printf("ERROR - Index '%i' of 'Resources/render_modes.json' is not a string.\n", index);
				return true;
			}

			rm_ref_def.addValue(value.getString(), static_cast<RenderModes>(index));

			return false;
		});

		if (ret) {
			return false;
		}

	} else {
		log.first.writeString("Using default Render Modes.\n");
	}

	return true;
}

NS_END