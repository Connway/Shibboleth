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
#include <Shibboleth_RenderPipelineManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_InputManager.h>
#include <Shibboleth_IApp.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

SetupDevicesState::SetupDevicesState(IApp& app):
	_app(app)
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
	RenderManager& render_manager = _app.getManagerT<RenderManager>();

	Gaff::JSON cfg;

	if (!cfg.parseFile(GRAPHICS_CFG)) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to parse '" GRAPHICS_CFG "'.\n");
		_app.quit();
		return;
	}

	// Validate schema

	Gaff::JSON windows = cfg["windows"];

	if (!windows.isArray()) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
		_app.quit();
		return;
	}

	Gaff::JSON icon = cfg["icon"];

	if (icon && !icon.isString()) {
		_app.getLogManager().logMessage(LogManager::LOG_WARNING, _app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'. Value at 'icon' is not a string.\n");
		icon = Gaff::JSON();
	}

	Gaff::JSON initial_pipeline = cfg["initial_pipeline"];

	if (!initial_pipeline.isString()) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
		_app.quit();
		return;
	}

	bool failed = windows.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool
	{
		if (!value.isObject()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
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
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
			return true;
		}

		if (!y.isInteger()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file '" GRAPHICS_CFG "'.\n");
			return true;
		}

		if (!width.isInteger()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!height.isInteger()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!refresh_rate.isInteger()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!device_name.isString()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!window_name.isString()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!window_mode.isString()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!adapter_id.isInteger()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!display_id.isInteger()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!vsync.isBoolean()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!tags.isArray()) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		unsigned short disp_tags = 0;

		if (EXTRACT_DISPLAY_TAGS(tags, disp_tags)) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return nullptr;
		}

		if (!disp_tags) {
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
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
			_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Malformed config file.\n");
			return true;
		}

		if (!render_manager.createWindow(wnd_name, wnd_mode,
			static_cast<unsigned int>(x.getInteger()), static_cast<unsigned int>(y.getInteger()),
			static_cast<unsigned int>(width.getInteger()), static_cast<unsigned int>(height.getInteger()),
			static_cast<unsigned int>(refresh_rate.getInteger()), device_name.getString(),
			static_cast<unsigned int>(adapter_id.getInteger()), static_cast<unsigned int>(display_id.getInteger()),
			vsync.isTrue(), disp_tags)) {

			_app.getLogManager().logMessage(
				LogManager::LOG_ERROR, _app.getLogFileName(),
				"Failed to create window with values\n"
				"X: %lli\n"
				"Y: %lli\n"
				"Width: %lli\n"
				"Height: %lli\n"
				"Refresh Rate: %lli\n"
				"Vsync: %s\n"
				"Device Name: %s\n"
				"Adapter ID: %lli\n"
				"Display ID: %lli\n"
				"Tags: %uh\n",
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
				_app.getLogManager().logMessage(LogManager::LOG_WARNING, _app.getLogFileName(), "Failed to set window icon to '%s'.\n", icon.getString());
			}
		}

		return false;
	});

	if (failed) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to create windows.\n");
		_app.quit();
		return;
	}

	if (!render_manager.initThreadData()) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to create thread data for Render Manager.\n");
		_app.quit();
		return;
	}

	RenderPipelineManager& rp_mgr = _app.getManagerT<RenderPipelineManager>();

	if (!rp_mgr.init(initial_pipeline.getString())) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to initialize Render Pipeline Manager.\n");
		_app.quit();
		return;
	}

	InputManager& input_mgr = _app.getManagerT<InputManager>();

	if (!input_mgr.init()) {
		_app.getLogManager().logMessage(LogManager::LOG_ERROR, _app.getLogFileName(), "Failed to initialize Input Manager.\n");
		_app.quit();
		return;
	}

	_app.switchState(_transitions[0]);
}

void SetupDevicesState::exit(void)
{
}

NS_END