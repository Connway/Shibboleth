/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_SetupFunctions.h"

//#include <Shibboleth_IRenderPipelineManager.h>
//#include <Shibboleth_IResourceManager.h>
//#include <Shibboleth_INuklearManager.h>
//#include <Shibboleth_IRenderManager.h>
//#include <Shibboleth_ISchemaManager.h>
//#include <Shibboleth_IInputManager.h>
//
//#include <Shibboleth_ProgramBuffersCreator.h>
//#include <Shibboleth_ShaderProgramLoader.h>
//#include <Shibboleth_SamplerStateLoader.h>
//#include <Shibboleth_RenderTargetLoader.h>
//#include <Shibboleth_BufferCreator.h>
//#include <Shibboleth_PhysicsLoader.h>
//#include <Shibboleth_TextureLoader.h>
//#include <Shibboleth_HoldingLoader.h>
//#include <Shibboleth_ShaderLoader.h>
//#include <Shibboleth_ModelLoader.h>
//#include <Shibboleth_ImageLoader.h>
//#include <Shibboleth_LuaLoader.h>
//
//#include <Shibboleth_Utilities.h>
//#include <Shibboleth_String.h>
//#include <Shibboleth_IApp.h>
//
//#include <Gleam_IRenderDevice.h>
//#include <Gaff_File.h>

NS_SHIBBOLETH

//void GenerateDefaultConfig(Gaff::JSON& cfg);
//
//bool CreateResourceLoaders(void)
//{
//	IApp& app = GetApp();
//
//	app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(),
//		"\n==================================================\n"
//		"==================================================\n"
//		"Creating Resource Loaders...\n"
//	);
//
//	IRenderManager& render_mgr = app.getManagerT<IRenderManager>();
//	IResourceManager& res_mgr = app.getManagerT<IResourceManager>();
//
//	// PROGRAM BUFFERS CREATOR
//	{
//		ProgramBuffersCreator* program_buffers_creator = SHIB_ALLOCT(ProgramBuffersCreator, GetAllocator());
//
//		if (!program_buffers_creator) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create program buffers creator.\n");
//			app.quit();
//			return false;
//		}
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Program Buffers Creator\n");
//		res_mgr.registerResourceLoader(program_buffers_creator, "ProgramBuffers");
//	}
//
//	// BUFFER CREATOR
//	{
//		BufferCreator* buffer_creator = SHIB_ALLOCT(BufferCreator, GetAllocator());
//
//		if (!buffer_creator) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create buffer creator.\n");
//			app.quit();
//			return false;
//		}
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Buffer Creator\n");
//		res_mgr.registerResourceLoader(buffer_creator, "Buffer");
//	}
//
//	// TEXTURE LOADER
//	{
//		TextureLoader* texture_loader = SHIB_ALLOCT(TextureLoader, GetAllocator(), render_mgr);
//
//		if (!texture_loader) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create texture loader.\n");
//			app.quit();
//			return false;
//		}
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Texture Loader\n");
//		res_mgr.registerResourceLoader(texture_loader, ".texture");
//	}
//
//	// IMAGE LOADER
//	{
//		ImageLoader* image_loader = SHIB_ALLOCT(ImageLoader, GetAllocator());
//
//		if (!image_loader) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create image loader.\n");
//			app.quit();
//			return false;
//		}
//
//		Array<U8String> extensions;
//		extensions.emplacePush(".tga");
//		extensions.emplacePush(".jpeg");
//		extensions.emplacePush(".jpg");
//		extensions.emplacePush(".dds");
//		extensions.emplacePush(".png");
//		extensions.emplacePush(".bmp");
//		extensions.emplacePush(".hdr");
//		extensions.emplacePush(".tif");
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Image Loader\n");
//		res_mgr.registerResourceLoader(image_loader, extensions);
//	}
//
//	// SAMPLER STATE LOADER
//	{
//		SamplerStateLoader* sampler_loader = SHIB_ALLOCT(SamplerStateLoader, GetAllocator(), render_mgr);
//
//		if (!sampler_loader) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create sampler state loader.\n");
//			app.quit();
//			return false;
//		}
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Sampler State Loader\n");
//		res_mgr.registerResourceLoader(sampler_loader, ".sampler");
//	}
//
//	// SHADER LOADER
//	{
//		ShaderLoader* shader_loader = SHIB_ALLOCT(ShaderLoader, GetAllocator(), render_mgr);
//
//		if (!shader_loader) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create shader loader.\n");
//			app.quit();
//			return false;
//		}
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Shader Loader\n");
//		res_mgr.registerResourceLoader(shader_loader, render_mgr.getShaderExtension());
//	}
//
//	// SHADER PROGRAM LOADER
//	{
//		ISchemaManager& schema_mgr = app.getManagerT<ISchemaManager>();
//
//		ShaderProgramLoader* shader_program_loader = SHIB_ALLOCT(ShaderProgramLoader, GetAllocator(), res_mgr, schema_mgr, render_mgr);
//
//		if (!shader_program_loader) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create shader program loader.\n");
//			app.quit();
//			return false;
//		}
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Shader Program Loader\n");
//		res_mgr.registerResourceLoader(shader_program_loader, ".material");
//	}
//
//	// LUA LOADER
//	{
//		LuaLoader* lua_loader = SHIB_ALLOCT(LuaLoader, GetAllocator());
//
//		if (!lua_loader) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create Lua loader.\n");
//			app.quit();
//			return false;
//		}
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Lua Loader\n");
//		res_mgr.registerResourceLoader(lua_loader, ".lua");
//	}
//
//	// HOLDING LOADER
//	{
//		HoldingLoader* holding_loader = SHIB_ALLOCT(HoldingLoader, GetAllocator());
//
//		if (!holding_loader) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create Holding loader.\n");
//			app.quit();
//			return false;
//		}
//
//		Array<U8String> extensions;
//		extensions.emplacePush(".b3d");
//		extensions.emplacePush(".dae");
//		extensions.emplacePush(".blend");
//		extensions.emplacePush(".3ds");
//		extensions.emplacePush(".ase");
//		extensions.emplacePush(".obj");
//		extensions.emplacePush(".ifc");
//		extensions.emplacePush(".xgl");
//		extensions.emplacePush(".zgl");
//		extensions.emplacePush(".ply");
//		extensions.emplacePush(".lwo");
//		extensions.emplacePush(".lws");
//		extensions.emplacePush(".lxo");
//		extensions.emplacePush(".stl");
//		extensions.emplacePush(".dxf");
//		extensions.emplacePush(".x");
//		extensions.emplacePush(".ac");
//		extensions.emplacePush(".ms3d");
//		extensions.emplacePush(".cob");
//		extensions.emplacePush(".scn");
//		extensions.emplacePush(".bvh");
//		extensions.emplacePush(".csm");
//		extensions.emplacePush(".irrmesh");
//		extensions.emplacePush(".irr");
//		extensions.emplacePush(".mdl");
//		extensions.emplacePush(".md2");
//		extensions.emplacePush(".md3");
//		extensions.emplacePush(".pk3");
//		extensions.emplacePush(".mdc");
//		extensions.emplacePush(".md5");
//		extensions.emplacePush(".smd");
//		extensions.emplacePush(".vta");
//		extensions.emplacePush(".m3");
//		extensions.emplacePush(".3d");
//		extensions.emplacePush(".q3d");
//		extensions.emplacePush(".q3s");
//		extensions.emplacePush(".nff");
//		extensions.emplacePush(".off");
//		extensions.emplacePush(".raw");
//		extensions.emplacePush(".ter");
//		extensions.emplacePush(".hmp");
//		extensions.emplacePush(".ndo");
//		extensions.emplacePush(".fbx");
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Holding Loader\n");
//		res_mgr.registerResourceLoader(holding_loader, extensions);
//	}
//
//	// MODEL LOADER
//	{
//		ModelLoader* model_loader = SHIB_ALLOCT(ModelLoader, GetAllocator(), render_mgr, res_mgr, *app.getFileSystem());
//
//		if (!model_loader) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create Model loader.\n");
//			app.quit();
//			return false;
//		}
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Model Loader\n");
//		res_mgr.registerResourceLoader(model_loader, ".model");
//	}
//
//	// RENDER TARGET LOADER
//	{
//		RenderTargetLoader* render_target_loader = SHIB_ALLOCT(RenderTargetLoader, GetAllocator());
//
//		if (!render_target_loader) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create Render Target loader.\n");
//			app.quit();
//			return false;
//		}
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Render Target Loader\n");
//		res_mgr.registerResourceLoader(render_target_loader, ".rendertarget");
//	}
//
//	// PHYSICS LOADER
//	{
//		PhysicsLoader* physics_loader = SHIB_ALLOCT(PhysicsLoader, GetAllocator());
//
//		if (!physics_loader) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create Physics loader.\n");
//			app.quit();
//			return false;
//		}
//
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Render Target Loader\n");
//		res_mgr.registerResourceLoader(physics_loader, ".physics");
//	}
//
//	app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Finished Creating Resource Loaders\n\n");
//	return true;
//}
//
//bool LoadGraphicsModule(void)
//{
//	IApp& app = GetApp();
//
//	IRenderManager& render_manager = app.getManagerT<IRenderManager>();
//	bool file_exists = false;
//	Gaff::JSON cfg;
//
//	// Open file and see if it succeeded, scope will close file
//	{
//		Gaff::File file(GRAPHICS_CFG);
//		file_exists = file.isOpen();
//	}
//
//	if (file_exists) {
//		if (!cfg.parseFile(GRAPHICS_CFG)) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to parse '" GRAPHICS_CFG "'.\n");
//			app.quit();
//			return false;
//		}
//	}
//	else {
//		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "No config file found at '" GRAPHICS_CFG "'. Generating default config.\n");
//		GenerateDefaultConfig(cfg);
//		cfg.dumpToFile(GRAPHICS_CFG);
//	}
//
//	Gaff::JSON module = cfg["module"];
//
//	if (!module.isString()) {
//		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Malformed graphics config file '" GRAPHICS_CFG "'. 'module' field is not a string.\n");
//		app.quit();
//		return false;
//	}
//
//	if (!render_manager.init(module.getString())) {
//		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to initialize the Render Manager using module '%s'.\n", module.getString());
//		app.quit();
//		return false;
//	}
//
//	return true;
//}
//
//bool SetupDevices(void)
//{
//	IApp& app = GetApp();
//	IRenderManager& render_manager = app.getManagerT<IRenderManager>();
//
//	Gaff::JSON cfg;
//
//	if (!cfg.parseFile(GRAPHICS_CFG)) {
//		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to parse '" GRAPHICS_CFG "'.\n");
//		app.quit();
//		return false;
//	}
//
//	// Validate schema
//
//	Gaff::JSON windows = cfg["windows"];
//
//	if (!windows.isArray()) {
//		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
//		app.quit();
//		return false;
//	}
//
//	Gaff::JSON icon = cfg["icon"];
//
//	if (!icon.isNull() && !icon.isString()) {
//		app.getLogManager().logMessage(LogManager::LOG_WARNING, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'. Value at 'icon' is not a string.\n");
//		icon = Gaff::JSON();
//	}
//
//	Gaff::JSON initial_pipeline = cfg["initial_pipeline"];
//
//	if (!initial_pipeline.isString()) {
//		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
//		app.quit();
//		return false;
//	}
//
//	bool failed = windows.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool
//	{
//		if (!value.isObject()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
//			return true;
//		}
//
//		Gaff::JSON x = value["x"];
//		Gaff::JSON y = value["y"];
//		Gaff::JSON width = value["width"];
//		Gaff::JSON height = value["height"];
//		Gaff::JSON refresh_rate = value["refresh_rate"];
//		Gaff::JSON device_name = value["device_name"];
//		Gaff::JSON window_name = value["window_name"];
//		Gaff::JSON window_mode = value["window_mode"];
//		Gaff::JSON adapter_id = value["adapter_id"];
//		Gaff::JSON display_id = value["display_id"];
//		Gaff::JSON vsync = value["vsync"];
//		Gaff::JSON tags = value["tags"];
//
//		if (!x.isInt()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
//			return true;
//		}
//
//		if (!y.isInt()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
//			return true;
//		}
//
//		if (!width.isInt()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return true;
//		}
//
//		if (!height.isInt()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return true;
//		}
//
//		if (!refresh_rate.isInt()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return true;
//		}
//
//		if (!device_name.isString()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return true;
//		}
//
//		if (!window_name.isString()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return true;
//		}
//
//		if (!window_mode.isString()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return true;
//		}
//
//		if (!adapter_id.isInt()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return true;
//		}
//
//		if (!display_id.isInt()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return true;
//		}
//
//		if (!vsync.isBool()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return true;
//		}
//
//		if (!tags.isArray()) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return true;
//		}
//
//		uint16_t disp_tags = render_manager.getDislayTags(tags);
//
//		if (!disp_tags) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file or no tags found.\n");
//			return nullptr;
//		}
//
//		if (!disp_tags) {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return nullptr;
//		}
//
//		const char* wnd_name = nullptr;
//		wnd_name = window_name.getString();
//
//		Gleam::IWindow::MODE wnd_mode;
//
//		if (!strncmp(window_mode.getString(), "Fullscreen", strlen("Fullscreen"))) {
//			wnd_mode = Gleam::IWindow::FULLSCREEN;
//		}
//		else if (!strncmp(window_mode.getString(), "Windowed", strlen("Windowed"))) {
//			wnd_mode = Gleam::IWindow::WINDOWED;
//		}
//		else if (!strncmp(window_mode.getString(), "Fullscreen-Windowed", strlen("Fullscreen-Windowed"))) {
//			wnd_mode = Gleam::IWindow::FULLSCREEN_WINDOWED;
//		}
//		else {
//			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
//			return true;
//		}
//
//		if (!render_manager.createWindow(wnd_name, wnd_mode,
//			static_cast<unsigned int>(x.getInt()), static_cast<unsigned int>(y.getInt()),
//			static_cast<unsigned int>(width.getInt()), static_cast<unsigned int>(height.getInt()),
//			static_cast<unsigned int>(refresh_rate.getInt()), device_name.getString(),
//			static_cast<unsigned int>(adapter_id.getInt()), static_cast<unsigned int>(display_id.getInt()),
//			vsync.isTrue(), disp_tags)) {
//
//			app.getLogManager().logMessage(
//				LogManager::LOG_ERROR, app.getLogFileName(),
//				"Failed to create window with values\n"
//				"X: %i\n"
//				"Y: %i\n"
//				"Width: %i\n"
//				"Height: %i\n"
//				"Refresh Rate: %i\n"
//				"Vsync: %s\n"
//				"Device Name: %s\n"
//				"Adapter ID: %i\n"
//				"Display ID: %i\n"
//				"Tags: %uh\n",
//				x.getInt(), y.getInt(),
//				width.getInt(), height.getInt(),
//				refresh_rate.getInt(), (vsync.isTrue()) ? "true" : "false",
//				device_name.getString(), adapter_id.getInt(),
//				display_id.getInt(), disp_tags
//			);
//
//			return true;
//		}
//
//		if (!icon.isNull()) {
//			if (!render_manager.getWindowData(static_cast<unsigned int>(render_manager.getNumWindows() - 1)).window->setIcon(icon.getString())) {
//				app.getLogManager().logMessage(LogManager::LOG_WARNING, app.getLogFileName(), "Failed to set window icon to '%s'.\n", icon.getString());
//			}
//		}
//
//		return false;
//	});
//
//	if (failed) {
//		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to create windows.\n");
//		app.quit();
//		return false;
//	}
//
//	if (!render_manager.initThreadData()) {
//		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to create thread data for Render Manager.\n");
//		app.quit();
//		return false;
//	}
//
//	IRenderPipelineManager& rp_mgr = app.getManagerT<IRenderPipelineManager>();
//
//	if (!rp_mgr.init(initial_pipeline.getString())) {
//		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to initialize Render Pipeline Manager.\n");
//		app.quit();
//		return false;
//	}
//
//	IInputManager& input_mgr = app.getManagerT<IInputManager>();
//
//	if (!input_mgr.init()) {
//		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to initialize Input Manager.\n");
//		app.quit();
//		return false;
//	}
//
//	INuklearManager& nk_mgr = app.getManagerT<INuklearManager>();
//
//	if (!nk_mgr.init()) {
//		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to initialize Nuklear Manager.\n");
//		app.quit();
//		return false;
//	}
//
//	return true;
//}
//
//
//void GenerateDefaultConfig(Gaff::JSON& cfg)
//{
//	IApp& app = GetApp();
//
//	cfg = Gaff::JSON::CreateObject();
//
//#if !defined(_WIN32) && !defined(_WIN64)
//	cfg.setObject("module", Gaff::JSON::CreateString("Graphics_OpenGL"));
//#else
//	cfg.setObject("module", Gaff::JSON::CreateString("Graphics_Direct3D11"));
//#endif
//
//	IRenderManager& render_manager = app.getManagerT<IRenderManager>();
//	Gleam::IRenderDevice::AdapterList adapters = render_manager.getRenderDevice().getDisplayModes();
//	GAFF_ASSERT(!adapters.empty());
//
//	Gaff::JSON windows = Gaff::JSON::CreateArray();
//	Gaff::JSON window_entry = Gaff::JSON::CreateObject();
//	Gaff::JSON tags = Gaff::JSON::CreateArray();
//	tags.setObject(size_t(0), Gaff::JSON::CreateString(GetEnumRefDef<DisplayTags>().getName(DT_1)));
//
//	window_entry.setObject("x", Gaff::JSON::CreateInt(0));
//	window_entry.setObject("y", Gaff::JSON::CreateInt(0));
//	window_entry.setObject("width", Gaff::JSON::CreateInt(800));
//	window_entry.setObject("height", Gaff::JSON::CreateInt(600));
//	window_entry.setObject("refresh_rate", Gaff::JSON::CreateInt(60));
//	window_entry.setObject("window_name", Gaff::JSON::CreateString("Shibboleth"));
//	window_entry.setObject("windowed_mode", Gaff::JSON::CreateString("Windowed"));
//	window_entry.setObject("adapter_id", Gaff::JSON::CreateInt(0));
//	window_entry.setObject("display_id", Gaff::JSON::CreateInt(0));
//	window_entry.setObject("vsync", Gaff::JSON::CreateFalse());
//	window_entry.setObject("device_name", Gaff::JSON::CreateString(adapters[0].adapter_name));
//	window_entry.setObject("tags", tags);
//
//	windows.setObject(size_t(0), window_entry);
//
//	cfg.setObject("windows", windows);
//}

NS_END
