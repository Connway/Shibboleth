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

#pragma once

#include "Shibboleth_LoadGraphicsModuleState.h"
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IRenderDevice.h>
#include <Gaff_JSON.h>

#define GRAPHICS_CFG "graphics.cfg"

NS_SHIBBOLETH

LoadGraphicsModuleState::LoadGraphicsModuleState(IApp& app):
	_app(app)
{
}

LoadGraphicsModuleState::~LoadGraphicsModuleState(void)
{
}

bool LoadGraphicsModuleState::init(unsigned int)
{
	return true;
}

void LoadGraphicsModuleState::enter(void)
{
}

void LoadGraphicsModuleState::update(void)
{
	RenderManager& render_manager = _app.getManagerT<RenderManager>("Render Manager");
	LogManager::FileLockPair& log = _app.getGameLogFile();

	bool file_exists = false;
	Gaff::JSON cfg;

	// Open file and see if it succeeded, scope will close file
	{
		Gaff::File file(GRAPHICS_CFG);
		file_exists = file.isOpen();
	}

	if (file_exists) {
		if (!cfg.parseFile(GRAPHICS_CFG)) {
			log.first.printf("ERROR - Failed to parse '%s'.\n", GRAPHICS_CFG);
			_app.quit();
			return;
		}

	} else {
		log.first.printf("No config file found at '%s'. Generating default config.\n", GRAPHICS_CFG);
		generateDefaultConfig(cfg);
		cfg.dumpToFile(GRAPHICS_CFG);
	}

	Gaff::JSON module = cfg["module"];

	if (!module.isString()) {
		log.first.printf("ERROR - Malformed graphics config file '%s'. 'module' field is not a string.\n", GRAPHICS_CFG);
		_app.quit();
		return;
	}

	if (!render_manager.init(module.getString())) {
		log.first.printf("ERROR - Failed to initialize the Render Manager using module '%s'.\n", module.getString());
		_app.quit();
	}

	_app.switchState(_transitions[0]);
}

void LoadGraphicsModuleState::exit(void)
{
}

void LoadGraphicsModuleState::generateDefaultConfig(Gaff::JSON& cfg)
{
	cfg = Gaff::JSON::createObject();

#if !defined(_WIN32) && !defined(_WIN64)
	cfg.setObject("module", Gaff::JSON::createString("Graphics_OpenGL"));
#else
	cfg.setObject("module", Gaff::JSON::createString("Graphics_Direct3D"));
#endif

	RenderManager& render_manager = _app.getManagerT<RenderManager>("Render Manager");
	Gleam::IRenderDevice::AdapterList adapters = render_manager.getRenderDevice().getDisplayModes();
	assert(!adapters.empty());

	Gaff::JSON windows = Gaff::JSON::createArray();
	Gaff::JSON window_entry = Gaff::JSON::createObject();
	Gaff::JSON tags = Gaff::JSON::createArray();
	tags.setObject(size_t(0), Gaff::JSON::createString(GetEnumRefDef<DisplayTags>().getName(DT_1)));

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
	window_entry.setObject("device_name", Gaff::JSON::createString(adapters[0].adapter_name));
	window_entry.setObject("tags", tags);

	windows.setObject(size_t(0), window_entry);

	cfg.setObject("windows", windows);
}

NS_END
