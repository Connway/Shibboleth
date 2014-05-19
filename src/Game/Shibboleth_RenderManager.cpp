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

RenderManager::RenderManager(App& app):
	_app(app)
{
}

RenderManager::~RenderManager(void)
{
}

const char* RenderManager::getName(void) const
{
	return "Render Manager";
}

bool RenderManager::init(const char* cfg_file)
{
	// open and store loading log file

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
		generateDefaultConfig(cfg);
		cfg.dumpToFile(cfg_file);
	}

	if (!cacheGleamFunctions(_app, cfg["module"], cfg_file)) {
		return false;
	}

	// Hopefully won't need this
	Gleam::SetAllocator(&_app.getAllocator());

	_render_device = _graphics_functions.create_renderdevice();

	TextureLoader* texture_loader = _app.getAllocator().template allocT<TextureLoader>(*this);

	if (!texture_loader) {
		return false;
	}

	ResourceManager& res_mgr = _app.getManager<ResourceManager>("Resource Manager");

	Array<AString> extensions;
	extensions.movePush(AString(".png"));
	extensions.movePush(AString(".jpeg"));
	extensions.movePush(AString(".jpg"));
	extensions.movePush(AString(".bmp"));
	extensions.movePush(AString(".tiff"));
	extensions.movePush(AString(".tif"));
	extensions.movePush(AString(".dds"));
	extensions.movePush(AString(".tga"));

	res_mgr.registerResourceLoader(texture_loader, extensions);

	return _render_device;
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

Gleam::IShaderResourceView* RenderManager::createShaderResourceView(void)
{
	assert(_graphics_functions.create_shaderresourceview);
	return _graphics_functions.create_shaderresourceview();
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

void RenderManager::generateDefaultConfig(Gaff::JSON& cfg)
{
	cfg = Gaff::JSON::createObject();
#if !defined(_WIN32) && !defined(_WIN64)
	cfg.setObject("module", Gaff::JSON::createString("Graphics_OpenGL" BIT_EXTENSION));
#else
	cfg.setObject("module", Gaff::JSON::createString("Graphics_Direct3D" BIT_EXTENSION));
#endif
	cfg.setObject("pos_x", Gaff::JSON::createInteger(0));
	cfg.setObject("pos_y", Gaff::JSON::createInteger(0));
	cfg.setObject("res_x", Gaff::JSON::createInteger(800));
	cfg.setObject("res_y", Gaff::JSON::createInteger(600));
}

// Still single-threaded at this point, so ok that we're not using the spinlock
bool RenderManager::cacheGleamFunctions(App& app, const Gaff::JSON& module, const char* cfg_file)
{
	// Instead of asserting, I'm going to log the errors and fail gracefully.
	if (!module.isString()) {
		app.getGameLogFile().first.printf("ERROR - Malformed graphics config file '%s'. 'module' field is not a string.", cfg_file);
		return false;
	}

	_gleam_module = app.getDynamicLoader().loadModule(module.getString(), module.getString());

	if (!_gleam_module) {
		app.getGameLogFile().first.printf("ERROR - Failed to find graphics module '%s'.", module.getString());
		return false;
	}

	// cache graphics function pointers
	GraphicsFunctions::InitGraphics init_graphics = _gleam_module->GetFunc<GraphicsFunctions::InitGraphics>("InitGraphics");

	if (!init_graphics) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'InitGraphics' in graphics module '%s'.", module.getString());
		return false;
	}

	char log_file_name[64] = { 0 };
	Gaff::GetCurrentTimeString(log_file_name, 64, "Logs/GleamLog %m-%d-%Y %H-%M-%S.txt");

	if (!Gaff::CreateDir("./Logs", 0777)) {
		return false;
	}

	if (!init_graphics(app, log_file_name)) {
		app.getGameLogFile().first.printf("ERROR - Graphics module '%s' failed to initialize.", module.getString());
		return false;
	}

	_graphics_functions.shutdown = _gleam_module->GetFunc<GraphicsFunctions::ShutdownGraphics>("ShutdownGraphics");

	if (!_graphics_functions.shutdown) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'ShutdownGraphics' in graphics module '%s'.", module.getString());
		return false;
	}

	_graphics_functions.create_shaderresourceview = _gleam_module->GetFunc<GraphicsFunctions::CreateShaderResourceView>("CreateShaderResourceView");
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
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateShaderResourceView' in graphics module '%s'.", module.getString());
		return false;
	}

	if (!_graphics_functions.create_renderdevice) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateRenderDevice' in graphics module '%s'.", module.getString());
		return false;
	}

	if (!_graphics_functions.create_rendertarget) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateRenderTarget' in graphics module '%s'.", module.getString());
		return false;
	}

	if (!_graphics_functions.create_samplerstate) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateSamplerState' in graphics module '%s'.", module.getString());
		return false;
	}

	if (!_graphics_functions.create_renderstate) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateRenderState' in graphics module '%s'.", module.getString());
		return false;
	}

	if (!_graphics_functions.create_texture) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateTexture' in graphics module '%s'.", module.getString());
		return false;
	}

	if (!_graphics_functions.create_layout) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateLayout' in graphics module '%s'.", module.getString());
		return false;
	}

	if (!_graphics_functions.create_program) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateProgram' in graphics module '%s'.", module.getString());
		return false;
	}

	if (!_graphics_functions.create_shader) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateShader' in graphics module '%s'.", module.getString());
		return false;
	}

	if (!_graphics_functions.create_buffer) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateBuffer' in graphics module '%s'.", module.getString());
		return false;
	}

	if (!_graphics_functions.create_model) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateModel' in graphics module '%s'.", module.getString());
		return false;
	}

	if (!_graphics_functions.create_mesh) {
		app.getGameLogFile().first.printf("ERROR - Failed to find function 'CreateMesh' in graphics module '%s'.", module.getString());
		return false;
	}

	return true;
}

NS_END
