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
REF_IMPL_SHIB(RenderManager);

RenderManager::RenderManager(IApp& app):
	_render_device(nullptr, ProxyAllocator(GetAllocator(), "Graphics Allocations")),
	_proxy_allocator(GetAllocator(), "Graphics Allocations"), _app(app)
{
	memset(&_graphics_functions, 0, sizeof(GraphicsFunctions));
}

RenderManager::~RenderManager(void)
{
	_render_device = nullptr;
	_windows.clear();

	if (_graphics_functions.shutdown) {
		_graphics_functions.shutdown();
	}
}

const char* RenderManager::getName(void) const
{
	return "Render Manager";
}

bool RenderManager::init(const char* cfg_file)
{
	Gleam::SetAllocator(&_proxy_allocator);

	LogManager::FileLockPair& log = _app.getGameLogFile();
	log.first.writeString("Initializing Render Manager...\n");

	bool file_exists = false;
	Gaff::JSON cfg;

	// Open file and see if it succeeded, scope will close file
	{
		Gaff::File file(cfg_file);
		file_exists = file.isOpen();
	}

	if (file_exists) {
		cfg.parseFile(cfg_file);

	} else {
		log.first.printf("No config file found at '%s'. Generating default config.\n", cfg_file);
		generateDefaultConfig(cfg);
		cfg.dumpToFile(cfg_file);
	}

	if (!cacheGleamFunctions(_app, cfg["module"], cfg_file)) {
		log.first.writeString("ERROR - Failed to cache Gleam functions.\n");
		return false;
	}

	_render_device = _graphics_functions.create_renderdevice();

	if (!_render_device) {
		log.first.writeString("ERROR - Failed to create render device.\n");
		return false;
	}

	Gaff::JSON windows = cfg["windows"];
	
	if (!windows.isArray()) {
		log.first.writeString("ERROR - Malformed config file.\n");
		return false;
	}

	_windows.reserve(8); // Reserve 8 windows to be safe. Will probably never hit this many though.

	bool failed = false;
	windows.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool
	{
		if (!value.isObject()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		Gaff::JSON x = value["x"];
		Gaff::JSON y = value["y"];
		Gaff::JSON width = value["width"];
		Gaff::JSON height = value["height"];
		Gaff::JSON refresh_rate = value["refresh_rate"];
		Gaff::JSON device_name = value["device_name"];
		Gaff::JSON window_name = value["window_name"];
		Gaff::JSON window_mode = value["window_mode"];
		Gaff::JSON adapter_id = value["adapter_id"];
		Gaff::JSON display_id = value["display_id"];
		Gaff::JSON vsync = value["vsync"];

		if (!x.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		if (!y.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		if (!width.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		if (!height.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		if (!refresh_rate.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		if (!device_name.isString()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		if (!window_name.isString()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		if (!window_mode.isString()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		if (!adapter_id.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		if (!display_id.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		if (!vsync.isBoolean()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		const GChar* wnd_name = nullptr;

#ifdef _UNICODE
		wchar_t temp[64] = { 0 };
		mbstowcs(temp, window_name.getString(), 64);
		wnd_name = temp;
#else
		wnd_name = window_name.getString();
#endif

		Gleam::Window::MODE wnd_mode;

		if (!strncmp(window_mode.getString(), "Fullscreen", strlen("Fullscreen"))) {
			wnd_mode = Gleam::Window::FULLSCREEN;
		} else if (!strncmp(window_mode.getString(), "Windowed", strlen("Windowed"))) {
			wnd_mode = Gleam::Window::WINDOWED;
		} else if (!strncmp(window_mode.getString(), "Fullscreen-Windowed", strlen("Fullscreen-Windowed"))) {
			wnd_mode = Gleam::Window::FULLSCREEN_WINDOWED;
		} else {
			log.first.writeString("ERROR - Malformed config file.\n");
			failed = true;
			return true;
		}

		if (!createWindow(wnd_name, wnd_mode,
			(unsigned int)x.getInteger(), (unsigned int)y.getInteger(),
			(unsigned int)width.getInteger(), (unsigned int)height.getInteger(),
			(unsigned int)refresh_rate.getInteger(), device_name.getString(),
			(unsigned int)adapter_id.getInteger(), (unsigned int)display_id.getInteger(),
			vsync.isTrue())) {

			log.first.printf("ERROR - Failed to create window with values\n"
				"X: %i\n"
				"Y: %i\n"
				"Width: %i\n"
				"Height: %i\n"
				"Refresh Rate: %i\n"
				"Device Name: %s\n"
				"Adapter ID: %i\n"
				"Display ID: %i\n",
				x.getInteger(), y.getInteger(),
				width.getInteger(), height.getInteger(),
				refresh_rate.getInteger(), device_name.getString(),
				adapter_id.getInteger(), display_id.getInteger()
			);

			failed = true;
			return true;
		}

		return false;
	});

	return !failed;
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

//void RenderManager::addRenderPacket()
//{
//}

bool RenderManager::createWindow(
	const wchar_t* app_name, Gleam::Window::MODE window_mode,
	int x, int y, unsigned int width, unsigned int height,
	unsigned int refresh_rate, const char* device_name,
	unsigned int adapter_id, unsigned int display_id, bool vsync)
{
	LogManager::FileLockPair& log = _app.getGameLogFile();

	assert(_render_device);
	Gleam::Window* window = _proxy_allocator.template allocT<Gleam::Window>();

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

		_proxy_allocator.freeT(window);
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

		_proxy_allocator.freeT(window);
		return false;
	}

	if (!_render_device->init(*window, adapter_id, display_id, (unsigned int)display_mode_id, vsync)) {
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(*log.second);
		log.first.writeString("ERROR - Failed to initialize render device with window!\n");
		_proxy_allocator.freeT(window);
		return false;
	}

	unsigned int device_id = (unsigned int)_render_device->getDeviceForAdapter(adapter_id);

	WindowData wnd_data = { Gaff::SmartPtr<Gleam::Window, ProxyAllocator>(window, _proxy_allocator), device_id, _render_device->getNumOutputs(device_id) };
	_windows.push(wnd_data);
	return true;
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

void RenderManager::generateDefaultConfig(Gaff::JSON& cfg)
{
	cfg = Gaff::JSON::createObject();
#if !defined(_WIN32) && !defined(_WIN64)
	cfg.setObject("module", Gaff::JSON::createString("Graphics_OpenGL"));
#else
	cfg.setObject("module", Gaff::JSON::createString("Graphics_Direct3D"));
#endif

	Gaff::JSON windows = Gaff::JSON::createArray();
	Gaff::JSON window_entry = Gaff::JSON::createObject();

	window_entry.setObject("x", Gaff::JSON::createInteger(0));
	window_entry.setObject("y", Gaff::JSON::createInteger(0));
	window_entry.setObject("width", Gaff::JSON::createInteger(800));
	window_entry.setObject("height", Gaff::JSON::createInteger(600));
	window_entry.setObject("refresh_rate", Gaff::JSON::createInteger(60));
	window_entry.setObject("window_name", Gaff::JSON::createString("Shibboleth"));
	window_entry.setObject("windowed_mode", Gaff::JSON::createString("Windowed"));
	window_entry.setObject("adapter_id", Gaff::JSON::createInteger(0));
	window_entry.setObject("display_id", Gaff::JSON::createInteger(0));
	window_entry.setObject("vsync", Gaff::JSON::createFalse());

	windows.setObject(size_t(0), window_entry);

	cfg.setObject("windows", windows);
}

// Still single-threaded at this point, so ok that we're not using the spinlock
bool RenderManager::cacheGleamFunctions(IApp& app, const Gaff::JSON& module, const char* cfg_file)
{
	LogManager::FileLockPair& log = _app.getGameLogFile();

	// Instead of asserting, I'm going to log the errors and fail gracefully.
	if (!module.isString()) {
		log.first.printf("ERROR - Malformed graphics config file '%s'. 'module' field is not a string.", cfg_file);
		return false;
	}

	AString module_path(module.getString());
	module_path += BIT_EXTENSION;

	_gleam_module = app.loadModule(module_path.getBuffer(), module.getString());

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

void RenderManager::InitReflectionDefinition(void)
{
	if (!g_Ref_Def.isDefined()) {
		g_Ref_Def.setAllocator(ProxyAllocator());

		g_Ref_Def.markDefined();
	}
}

NS_END
