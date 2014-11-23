/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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
#include "Shibboleth_TextureLoader.h"
#include <Gleam_Global.h>
#include <Gaff_JSON.h>

#include <Gleam_IRenderDevice.h>

NS_SHIBBOLETH

REF_IMPL_REQ(RenderManager);
REF_IMPL_ASSIGN_SHIB(RenderManager)
.addBaseClassInterfaceOnly<RenderManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

RenderManager::RenderManager(IApp& app):
	_render_device(nullptr, ProxyAllocator(GetAllocator(), "Graphics Allocations")),
	_proxy_allocator(GetAllocator(), "Graphics Allocations"), _app(app)
{
	memset(&_graphics_functions, 0, sizeof(GraphicsFunctions));
}

RenderManager::~RenderManager(void)
{
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

void RenderManager::requestUpdateEntries(Array<UpdateEntry>& entries)
{
	//entries.movePush(UpdateEntry(AString("Render Manager: Render"), Gaff::Bind(this, &RenderManager::update)));
}

// I should move the config file stuff out of this function.
// Arguement should be the module to load for cacheGleamFunctions().
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

	return true;
}

void RenderManager::update(double)
{
	// Multithread this at some point.
	// Need to add support for thread render devices.
	// Update functions will need to take in an IRenderDevice.
	for (unsigned int i = 0; i < _render_functions.size(); ++i) {
		_render_functions[i]();
	}
}

Gleam::IRenderDevice& RenderManager::getRenderDevice(void)
{
	return *_render_device;
}

Gaff::SpinLock& RenderManager::getSpinLock(void)
{
	return _rd_lock;
}

void RenderManager::printfLoadLog(const char* format, ...)
{
	assert(format);

	LogManager::FileLockPair& file_pair = _app.getGameLogFile();
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(*file_pair.second);

	va_list vl;
	va_start(vl, format);
	file_pair.first.printfVA(format, vl);
	va_end(vl);
}

bool RenderManager::createWindow(
	const wchar_t* app_name, Gleam::IWindow::MODE window_mode,
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

	unsigned int device_id = (unsigned int)_render_device->getDeviceForAdapter(adapter_id);

	WindowData wnd_data = { window, device_id, _render_device->getNumOutputs(device_id), tags };
	_windows.push(wnd_data);
	return true;
}

void RenderManager::updateWindows(void)
{
	_graphics_functions.update_windows();
}

Array<const RenderManager::WindowData*> RenderManager::getAllWindowsWithTagsAny(unsigned short tags) const
{
	Array<const WindowData*> out;

	for (auto it = _windows.begin(); it != _windows.end(); ++it) {
		if (!tags || (it->tags & tags)) {
			out.push(it);
		}
	}

	return out;
}

Array<const RenderManager::WindowData*> RenderManager::getAllWindowsWithTags(unsigned short tags) const
{
	Array<const WindowData*> out;

	for (auto it = _windows.begin(); it != _windows.end(); ++it) {
		if (!tags || ((it->tags & tags) == tags)) {
			out.push(it);
		}
	}

	return out;
}

unsigned int RenderManager::getNumWindows(void) const
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

unsigned int RenderManager::getNumRenderTargets(void) const
{
	return _render_target_data.size();
}

RenderManager::RenderData& RenderManager::getRenderData(unsigned int rt_index)
{
	assert(rt_index < _render_target_data.size());
	return _render_target_data[rt_index].render_data;
}

unsigned int RenderManager::createRT(unsigned int width, unsigned int height, unsigned int device, Gleam::ITexture::FORMAT format, const AString& name, unsigned short tags)
{
	// We're about to do stuff to the Render Devices, lock it so that no one can mess with it
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_rd_lock);
	//data.device_data.resize(_render_device->getNumDevices());

	RenderData rd;

	//for (auto it = data.device_data.begin(); it != data.device_data.end(); ++it) {
		//_render_device->setCurrentDevice(it - data.device_data.begin());
		_render_device->setCurrentDevice(device);
		rd.output = createTexture();

		if (!rd.output) {
			// log error
			return UINT_FAIL;
		}

		if (!rd.output->init2D(*_render_device, width, height, format)) {
			// log error
			return UINT_FAIL;
		}

		rd.output_srv = createShaderResourceView();

		if (!rd.output_srv) {
			// log error
			return UINT_FAIL;
		}

		if (!rd.output_srv->init(*_render_device, rd.output.get())) {
			// log error
			return UINT_FAIL;
		}

		rd.render_target = createRenderTarget();

		if (!rd.render_target) {
			// log error
			return UINT_FAIL;
		}

		if (!rd.render_target->addTexture(*_render_device, rd.output.get())) {
			// log error
			return UINT_FAIL;
		}
	//}

	RenderTargetData data = {
		rd, name, width,
		height, tags
	};

	_render_target_data.push(data);

	return _render_target_data.size() - 1;
}

bool RenderManager::createRTDepth(unsigned int rt_index, Gleam::ITexture::FORMAT format)
{
	assert(rt_index < _render_target_data.size());
	RenderTargetData& data = _render_target_data[rt_index];

	// We're about to do stuff to the Render Devices, lock it so that no one can mess with it
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_rd_lock);

	//for (auto it = data.device_data.begin(); it != data.device_data.end(); ++it) {
		//_render_device->setCurrentDevice(it - data.device_data.begin());
		_render_device->setCurrentDevice(data.device);
		data.render_data.depth = createTexture();

		if (!data.render_data.depth) {
			// log error
			return false;
		}

		if (!data.render_data.depth->init2D(*_render_device, data.width, data.height, format)) {
			// log error
			return false;
		}

		data.render_data.depth_srv = createShaderResourceView();

		if (!data.render_data.depth_srv) {
			// log error
			return false;
		}

		if (!data.render_data.depth_srv->init(*_render_device, data.render_data.depth.get())) {
			// log error
			return false;
		}

		if (!data.render_data.render_target->addDepthStencilBuffer(*_render_device, data.render_data.depth.get())) {
			// log error
			return false;
		}
	//}

	return true;
}

void RenderManager::deleteRenderTargets(void)
{
	_render_target_data.clear();
	_render_functions.clear();
}

void RenderManager::addRenderFunction(Gaff::FunctionBinder<void> render_func, unsigned int position)
{
	if (position == UINT_FAIL) {
		_render_functions.push(render_func);

	} else {
		// Presumably something else will fill the holes. If not, we will have an assert when we try and call them.
		if (_render_functions.size() < position) {
			_render_functions.resize(position);
		}

		_render_functions[position] = render_func;
	}
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
	Gaff::GetCurrentTimeString(log_file_name, 64, "Logs/GleamLog %m-%d-%Y %H-%M-%S.txt");

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

NS_END
