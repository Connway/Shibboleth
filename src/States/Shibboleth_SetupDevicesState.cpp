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

#include "Shibboleth_SetupDevicesState.h"
#include <Shibboleth_IRenderPipelineManager.h>
#include <Shibboleth_INuklearManager.h>
#include <Shibboleth_IRenderManager.h>
#include <Shibboleth_IInputManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_IRequestableInterface.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

const char* SetupDevicesState::GetFriendlyName(void)
{
	return "Setup Devices State";
}

SetupDevicesState::SetupDevicesState(void)
{
}

SetupDevicesState::~SetupDevicesState(void)
{
}

bool SetupDevicesState::init(unsigned int)
{
	return true;
}

void SetupDevicesState::enter(void)
{
}

void SetupDevicesState::update(void)
{
	IApp& app = GetApp();
	IRenderManager& render_manager = app.getManagerT<IRenderManager>();

	Gaff::JSON cfg;

	if (!cfg.parseFile(GRAPHICS_CFG)) {
		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to parse '" GRAPHICS_CFG "'.\n");
		app.quit();
		return;
	}

	// Validate schema

	Gaff::JSON windows = cfg["windows"];

	if (!windows.isArray()) {
		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
		app.quit();
		return;
	}

	Gaff::JSON icon = cfg["icon"];

	if (!icon.isNull() && !icon.isString()) {
		app.getLogManager().logMessage(LogManager::LOG_WARNING, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'. Value at 'icon' is not a string.\n");
		icon = Gaff::JSON();
	}

	Gaff::JSON initial_pipeline = cfg["initial_pipeline"];

	if (!initial_pipeline.isString()) {
		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
		app.quit();
		return;
	}

	bool failed = windows.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool
	{
		if (!value.isObject()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
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

		if (!x.isInt()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
			return true;
		}

		if (!y.isInt()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
			return true;
		}

		if (!width.isInt()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!height.isInt()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!refresh_rate.isInt()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!device_name.isString()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!window_name.isString()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!window_mode.isString()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!adapter_id.isInt()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!display_id.isInt()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!vsync.isBool()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!tags.isArray()) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		unsigned short disp_tags = 0;

		if (EXTRACT_DISPLAY_TAGS(tags, disp_tags)) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return nullptr;
		}

		if (!disp_tags) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
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
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!render_manager.createWindow(wnd_name, wnd_mode,
			static_cast<unsigned int>(x.getInt()), static_cast<unsigned int>(y.getInt()),
			static_cast<unsigned int>(width.getInt()), static_cast<unsigned int>(height.getInt()),
			static_cast<unsigned int>(refresh_rate.getInt()), device_name.getString(),
			static_cast<unsigned int>(adapter_id.getInt()), static_cast<unsigned int>(display_id.getInt()),
			vsync.isTrue(), disp_tags)) {

			app.getLogManager().logMessage(
				LogManager::LOG_ERROR, app.getLogFileName(),
				"Failed to create window with values\n"
				"X: %i\n"
				"Y: %i\n"
				"Width: %i\n"
				"Height: %i\n"
				"Refresh Rate: %i\n"
				"Vsync: %s\n"
				"Device Name: %s\n"
				"Adapter ID: %i\n"
				"Display ID: %i\n"
				"Tags: %uh\n",
				x.getInt(), y.getInt(),
				width.getInt(), height.getInt(),
				refresh_rate.getInt(), (vsync.isTrue()) ? "true" : "false",
				device_name.getString(), adapter_id.getInt(),
				display_id.getInt(), disp_tags
			);

			return true;
		}

		if (!icon.isNull()) {
			if (!render_manager.getWindowData(static_cast<unsigned int>(render_manager.getNumWindows() - 1)).window->setIcon(icon.getString())) {
				app.getLogManager().logMessage(LogManager::LOG_WARNING, app.getLogFileName(), "Failed to set window icon to '%s'.\n", icon.getString());
			}
		}

		return false;
	});

	if (failed) {
		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to create windows.\n");
		app.quit();
		return;
	}

	if (!render_manager.initThreadData()) {
		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to create thread data for Render Manager.\n");
		app.quit();
		return;
	}

	IRenderPipelineManager& rp_mgr = app.getManagerT<IRenderPipelineManager>();

	if (!rp_mgr.init(initial_pipeline.getString())) {
		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to initialize Render Pipeline Manager.\n");
		app.quit();
		return;
	}

	IInputManager& input_mgr = app.getManagerT<IInputManager>();

	if (!input_mgr.init()) {
		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to initialize Input Manager.\n");
		app.quit();
		return;
	}

	INuklearManager& nk_mgr = app.getManagerT<INuklearManager>();

	if (!nk_mgr.init()) {
		app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "Failed to initialize Nuklear Manager.\n");
		app.quit();
		return;
	}

	app.switchState(_transitions[0]);
}

void SetupDevicesState::exit(void)
{
}

NS_END
