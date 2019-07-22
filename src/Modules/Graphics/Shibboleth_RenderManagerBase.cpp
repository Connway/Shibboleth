/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_RenderManagerBase.h"
#include "Shibboleth_GraphicsReflection.h"
#include <Shibboleth_SerializeReader.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IRenderDevice.h>
#include <Gaff_Assert.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

// Change this if the game supports more than one monitor.
// "main" display is implicit. Not necessary to explicitly reference it.
constexpr const char* g_supported_displays[] = { nullptr };

const VectorMap< Gaff::Hash32, Vector<const char*> > g_display_tags = {
	{ Gaff::FNV1aHash32Const("gameplay"), { "main" } }
};

const char* const g_graphics_cfg_schema =
R"({
	"type": "object",

	"properties":
	{
		"icon": { "type": "string" },
		"initial_pipeline": { "type": "string" },

		"windows":
		{
			"type": "object",
			"additionalProperties":
			{
				"type": "object",
				"properties":
				{
					"x": { "type": "number" },
					"y": { "type": "number" },
					"width": { "type": "number" },
					"height": { "type": "number" },
					"refresh_rate": { "type": "number" },
					"window_mode": { "type": "string" /*, "enum": ["windowed", "fullscreen", "borderless_windowed"]*/ },
					"adapter_id": { "type": "number" },
					"display_id": { "type": "number" },
					"vsync": { "type": "boolean" }
				},

				"requires": ["width", "height", "refresh_rate", "window_mode", "adapter_id", "display_id", "vsync"],
				"additionalProperties": false
			}
		},

		"required": ["initial_pipeline", "windows"]
	}
})";

RenderManagerBase::RenderManagerBase(void)
{
	_render_device_tags.reserve(ARRAY_SIZE(g_supported_displays) + g_display_tags.size());
	
	const ProxyAllocator allocator("Graphics");

	if constexpr (g_supported_displays[0]) {
		for (const char* tag : g_supported_displays) {
			_render_device_tags[Gaff::FNV1aHash32StringConst(tag)] = Vector<Gleam::IRenderDevice*>(allocator);
		}
	}

	// Always have a main window. Even if not specified.
	_render_device_tags[Gaff::FNV1aHash32StringConst("main")] = Vector<Gleam::IRenderDevice*>(allocator);

	for (auto entry : g_display_tags) {
		_render_device_tags[entry.first] = Vector<Gleam::IRenderDevice*>(allocator);
	}
}

RenderManagerBase::~RenderManagerBase(void)
{
}

bool RenderManagerBase::init(void)
{
	IFileSystem& fs = GetApp().getFileSystem();
	const IFile* const file = fs.openFile("cfg/graphics.cfg");

	if (!file) {
		// $TODO: Generate default config file.
		return false;
	}

	Gaff::JSON config;

	if (!config.parse(reinterpret_cast<const char*>(file->getBuffer()), g_graphics_cfg_schema)) {
		const char* const error = config.getErrorText();
		GAFF_REF(error);
		// $TODO: Log error.
		fs.closeFile(file);
		return false;
	}

	const Gaff::JSON windows = config["windows"];

	windows.forEachInObject([&](const char* key, const Gaff::JSON& value) -> bool
	{
		const int32_t adapter_id = value["adapter_id"].getInt32();
		const int32_t display_id = value["display_id"].getInt32();

		const auto display_modes = getDisplayModes();

		if (adapter_id >= static_cast<int32_t>(display_modes.size())) {
			// $TODO: Log error
			return false;
		}

		const auto& adapter_info = display_modes[adapter_id];

		if (display_id >= static_cast<int32_t>(adapter_info.displays.size())) {
			// $TODO: Log error
			return false;
		}

		const auto& display_info = adapter_info.displays[display_id];

		Gleam::IRenderDevice* rd = nullptr;

		// Check if we've already created this device.
		for (const auto& render_device : _render_devices) {
			if (render_device->getAdapterID() == adapter_id) {
				rd = render_device.get();
				break;
			}
		}

		// Create it if we don't already have it.
		if (!rd) {
			rd = createRenderDevice();
			
			if (!rd->init(adapter_id)) {
				// $TODO: Log error
				return false;
			}

			_render_devices.emplace_back(rd);
		}

		int32_t x = Gaff::Max(0, value["x"].getInt32(0));
		int32_t y = Gaff::Max(0, value["y"].getInt32(0));
		int32_t width = value["width"].getInt32(0);
		int32_t height = value["height"].getInt32(0);
		const int32_t refresh_rate = value["refresh_rate"].getInt32(0);
		const bool vsync = value["vsync"].getBool();
		Gleam::IWindow::WindowMode window_mode = Gleam::IWindow::WM_FULLSCREEN;

		SerializeReader<Gaff::JSON> reader(value["window_mode"], ProxyAllocator("Graphics"));
		Reflection<Gleam::IWindow::WindowMode>::Load(reader, window_mode);

		if (width == 0 || height == 0) {
			width = display_info.curr_width;
			height = display_info.curr_height;
		}

		if (window_mode == Gleam::IWindow::WM_FULLSCREEN) {
			x = 0;
			y = 0;
		}

		x += display_info.curr_x;
		y += display_info.curr_y;

		Gleam::IWindow* const window = createWindow();

		if (!window->init(key, window_mode, width, height, x, y)) {
			// $TODO: Log error
			SHIB_FREET(window, GetAllocator());
			return false;
		}

		// Add the device to the window tag.
		_render_device_tags[Gaff::FNV1aHash32String(key)].emplace_back(rd);

		// Add render device to tag list if not already present.
		for (const auto& entry : g_display_tags) {
			const auto it = Gaff::Find(entry.second, key, [](const char* lhs, const char* rhs) -> bool
			{
				return eastl::string_view(lhs) == eastl::string_view(rhs);
			});

			if (it == entry.second.end()) {
				continue;
			}

			auto& render_devices = _render_device_tags[entry.first];

			if (Gaff::Find(render_devices, rd) == render_devices.end()) {
				render_devices.emplace_back(rd);
			}
		}

		Gleam::IRenderOutput* const output = createRenderOutput();

		if (!output->init(*rd, *window, display_id, refresh_rate, vsync)) {
			// $TODO: Log error
			SHIB_FREET(output, GetAllocator());
			SHIB_FREET(window, GetAllocator());
			return false;
		}

		_windows_output.emplace_back(std::move(WindowPtr(window)), std::move(OutputPtr(output)));
		return false;
	});

	fs.closeFile(file);
	return true;
}

void RenderManagerBase::addRenderDeviceTag(Gleam::IRenderDevice* device, const char* tag)
{
	const Gaff::Hash32 hash = Gaff::FNV1aHash32String(tag);
	auto& devices = _render_device_tags[hash];

	GAFF_ASSERT(Gaff::Find(devices, device) == devices.end());
	devices.emplace_back(device);
}

void RenderManagerBase::manageRenderDevice(Gleam::IRenderDevice* device)
{
	const auto it = Gaff::Find(_render_devices, device, [](const auto& lhs, const auto* rhs) -> bool { return lhs.get() == rhs; });
	GAFF_ASSERT(it == _render_devices.end());
	_render_devices.emplace_back(device);
}

const Vector<Gleam::IRenderDevice*>* RenderManagerBase::getDevicesByTag(const char* tag) const
{
	const Gaff::Hash32 hash = Gaff::FNV1aHash32String(tag);
	const auto it = _render_device_tags.find(hash);
	return (it == _render_device_tags.end()) ? nullptr : &it->second;
}

Gleam::IRenderDevice& RenderManagerBase::getDevice(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_render_devices.size()));
	return *_render_devices[index];
}

int32_t RenderManagerBase::getNumDevices(void) const
{
	return static_cast<int32_t>(_render_devices.size());
}

NS_END
