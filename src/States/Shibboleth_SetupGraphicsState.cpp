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

#pragma once

#include "Shibboleth_SetupGraphicsState.h"
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IRenderDevice.h>
#include <Gaff_JSON.h>

#define GRAPHICS_CFG "graphics.cfg"

NS_SHIBBOLETH

SetupGraphicsState::SetupGraphicsState(IApp& app):
	_app(app)
{
}

SetupGraphicsState::~SetupGraphicsState(void)
{
}

bool SetupGraphicsState::init(unsigned int)
{
	return true;
}

void SetupGraphicsState::enter(void)
{
}

void SetupGraphicsState::update(void)
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
		cfg.parseFile(GRAPHICS_CFG);

	} else {
		log.first.printf("No config file found at '%s'. Generating default config.\n", GRAPHICS_CFG);
		generateDefaultConfig(cfg);
		cfg.dumpToFile(GRAPHICS_CFG);
	}

	Gaff::JSON module = cfg["module"];

	// Instead of asserting, I'm going to log the errors and fail gracefully.
	if (!module.isString()) {
		log.first.printf("ERROR - Malformed graphics config file '%s'. 'module' field is not a string.\n", GRAPHICS_CFG);
		_app.quit();
		return;
	}

	if (!render_manager.init(module.getString())) {
		log.first.printf("ERROR - Failed to initialize the Render Manager using module '%s'.\n", module.getString());
		_app.quit();
		return;
	}

	Gaff::JSON windows = cfg["windows"];

	if (!windows.isArray()) {
		log.first.writeString("ERROR - Malformed config file.\n");
		_app.quit();
		return;
	}

	Gaff::JSON icon = cfg["icon"];

	if (icon && !icon.isString()) {
		log.first.writeString("WARNING - Malformed config file. Value at 'icon' is not a string.\n");
		icon = Gaff::JSON();
	}

	bool failed = windows.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool
	{
		if (!value.isObject()) {
			log.first.writeString("ERROR - Malformed config file.\n");
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
		Gaff::JSON tags = value["tags"];

		if (!x.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!y.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!width.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!height.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!refresh_rate.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!device_name.isString()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!window_name.isString()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!window_mode.isString()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!adapter_id.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!display_id.isInteger()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!vsync.isBoolean()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!tags.isArray()) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		unsigned short disp_tags = 0;

		if (EXTRACT_DISPLAY_TAGS(tags, disp_tags)) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return nullptr;
		}

		if (!disp_tags) {
			log.first.writeString("ERROR - Malformed config file.\n");
			return nullptr;
		}

		const char* wnd_name = nullptr;
		wnd_name = window_name.getString();

		Gleam::IWindow::MODE wnd_mode;

		if (!strncmp(window_mode.getString(), "Fullscreen", strlen("Fullscreen"))) {
			wnd_mode = Gleam::IWindow::FULLSCREEN;
		} else if (!strncmp(window_mode.getString(), "Windowed", strlen("Windowed"))) {
			wnd_mode = Gleam::IWindow::WINDOWED;
		} else if (!strncmp(window_mode.getString(), "Fullscreen-Windowed", strlen("Fullscreen-Windowed"))) {
			wnd_mode = Gleam::IWindow::FULLSCREEN_WINDOWED;
		} else {
			log.first.writeString("ERROR - Malformed config file.\n");
			return true;
		}

		if (!render_manager.createWindow(wnd_name, wnd_mode,
			static_cast<unsigned int>(x.getInteger()), static_cast<unsigned int>(y.getInteger()),
			static_cast<unsigned int>(width.getInteger()), static_cast<unsigned int>(height.getInteger()),
			static_cast<unsigned int>(refresh_rate.getInteger()), device_name.getString(),
			static_cast<unsigned int>(adapter_id.getInteger()), static_cast<unsigned int>(display_id.getInteger()),
			vsync.isTrue(), disp_tags)) {

			log.first.printf("ERROR - Failed to create window with values\n"
				"X: %i\n"
				"Y: %i\n"
				"Width: %i\n"
				"Height: %i\n"
				"Refresh Rate: %i\n"
				"Vsync: %s\n"
				"Device Name: %s\n"
				"Adapter ID: %i\n"
				"Display ID: %i\n"
				"Tags: %i\n",
				x.getInteger(), y.getInteger(),
				width.getInteger(), height.getInteger(),
				refresh_rate.getInteger(), (vsync.isTrue()) ? "true" : "false",
				device_name.getString(), adapter_id.getInteger(),
				display_id.getInteger(), disp_tags
			);

			return true;
		}

		if (icon) {
			if (!render_manager.getWindowData(static_cast<unsigned int>(render_manager.getNumWindows() - 1)).window->setIcon(icon.getString())) {
				log.first.printf("WARNING - Failed to set window icon to '%s'.\n", icon.getString());
			}
		}

		return false;
	});

	if (failed) {
		log.first.writeString("ERROR - Failed to create windows.\n");
		_app.quit();
		return;
	}

	if (!render_manager.initThreadData()) {
		_app.quit();
		return;
	}

	_app.switchState(_transitions[0]);
}

void SetupGraphicsState::exit(void)
{
}

void SetupGraphicsState::generateDefaultConfig(Gaff::JSON& cfg)
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
