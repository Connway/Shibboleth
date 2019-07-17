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
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IRenderDevice.h>
#include <Gaff_Assert.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

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

				"requires": ["x", "y", "width", "height", "refresh_rate", "window_mode", "adapter_id", "display_id", "vsync"],
				"additionalProperties": false
			}
		},

		"required": ["initial_pipeline", "windows"]
	}
})";

static Gleam::IWindow* CreateWindow(RenderManagerBase& rm, const char* window_tag, const Gaff::JSON& config)
{
	GAFF_REF(rm, window_tag, config);
	return nullptr;
}

RenderManagerBase::RenderManagerBase(void)
{
	_render_device_tags.reserve(ARRAY_SIZE(g_supported_displays));
	
	const ProxyAllocator allocator("Graphics");

	for (const char* tag : g_supported_displays) {
		_render_device_tags[Gaff::FNV1aHash32StringConst(tag)] = Vector<Gleam::IRenderDevice*>(allocator);
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
		Gleam::IWindow* const window = CreateWindow(*this, key, value);

		if (window) {
			_windows.emplace_back(window);
		}

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
