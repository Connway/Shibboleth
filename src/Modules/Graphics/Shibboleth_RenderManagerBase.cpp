/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_SerializeReader.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_Keyboard_MessagePump.h>
#include <Gleam_Mouse_MessagePump.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IRenderDevice.h>
#include <Gaff_Assert.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

static ProxyAllocator g_allocator("Graphics");

// Change this if the game supports more than one monitor.
// "main" display is implicit. Not necessary to explicitly reference it.
static constexpr const char* g_supported_displays[] = { nullptr };

const VectorMap< Gaff::Hash32, Vector<const char*> > g_display_tags = {
	{ Gaff::FNV1aHash32Const("gameplay"), { "main" } }
};

static constexpr char* const g_graphics_cfg_schema =
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
		LogErrorDefault("Faild to parse config file with error - %s.", error);
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
			LogErrorDefault("Invalid 'adapter_id' for window '%s'.", key);
			return false;
		}

		const auto& adapter_info = display_modes[adapter_id];

		if (display_id >= static_cast<int32_t>(adapter_info.displays.size())) {
			LogErrorDefault("Invalid 'display_id' for window '%s'.", key);
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
			rd = createRenderDevice(adapter_id);
			
			if (!rd) {
				LogErrorDefault("Failed to create render device for window '%s'.", key);
				return false;
			}
		}

		int32_t x = Gaff::Max(0, value["x"].getInt32(0));
		int32_t y = Gaff::Max(0, value["y"].getInt32(0));
		int32_t width = value["width"].getInt32(0);
		int32_t height = value["height"].getInt32(0);
		const int32_t refresh_rate = value["refresh_rate"].getInt32(0);
		const bool vsync = value["vsync"].getBool();
		Gleam::IWindow::WindowMode window_mode = Gleam::IWindow::WindowMode::Windowed;

		SerializeReader<Gaff::JSON> reader(value["window_mode"], ProxyAllocator("Graphics"));
		
		// Log the error, but leave default to windowed mode.
		if (!Reflection<Gleam::IWindow::WindowMode>::Load(reader, window_mode)) {
			LogWarningDefault("Failed to serialize window mode for window '%s'. Defaulting to 'Windowed' mode.", key);
		}

		if (width == 0 || height == 0) {
			width = display_info.curr_width;
			height = display_info.curr_height;
		}

		if (window_mode == Gleam::IWindow::WindowMode::Fullscreen) {
			x = 0;
			y = 0;
		}

		x += display_info.curr_x;
		y += display_info.curr_y;

		Gleam::IWindow* const window = createWindow();

		if (!window->init(key, window_mode, width, height, x, y)) {
			LogErrorDefault("Failed to create window '%s'.", key);
			SHIB_FREET(window, GetAllocator());
			return false;
		}

		// Add the device to the window tag.
		const Gaff::Hash32 window_hash = Gaff::FNV1aHash32String(key);
		_render_device_tags[window_hash].emplace_back(rd);

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
			LogErrorDefault("Failed to create render output for window '%s'.", key);
			SHIB_FREET(output, GetAllocator());
			SHIB_FREET(window, GetAllocator());
			return false;
		}

		_window_outputs[window_hash] = eastl::make_pair(std::move(WindowPtr(window)), std::move(OutputPtr(output)));

		if (!createGBuffer(key)) {
			_render_device_tags.erase(window_hash);
			_window_outputs.erase(window_hash);
		}

		return false;
	});

	fs.closeFile(file);
	return true;
}

void RenderManagerBase::allModulesLoaded(void)
{
	IFileSystem& fs = GetApp().getFileSystem();
	const IFile* const file = fs.openFile("cfg/graphics.cfg");

	if (!file) {
		return;
	}

	Gaff::JSON config;

	if (!config.parse(reinterpret_cast<const char*>(file->getBuffer()), g_graphics_cfg_schema)) {
		const char* const error = config.getErrorText();
		LogErrorDefault("Faild to parse config file with error - %s.", error);
		fs.closeFile(file);
		return;
	}

	fs.closeFile(file);

	const Gaff::JSON sampler = config["texture_filtering"];

	if (sampler.isString()) {
		ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();

		_default_sampler = res_mgr.requestResourceT<SamplerStateResource>(sampler.getString());
		res_mgr.waitForResource(*_default_sampler);

		if (_default_sampler->hasFailed()) {
			// $TODO: Log error.
			GetApp().quit();
		}
	}
}

Gleam::IKeyboard* RenderManagerBase::createKeyboard(void) const
{
	return SHIB_ALLOCT(Gleam::KeyboardMP, g_allocator);
}

Gleam::IMouse* RenderManagerBase::createMouse(void) const
{
	return SHIB_ALLOCT(Gleam::MouseMP, g_allocator);
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

	for (auto& pair : _deferred_devices) {
		pair.second.emplace_back(device->createDeferredRenderDevice());
	}
}

const Vector<Gleam::IRenderDevice*>* RenderManagerBase::getDevicesByTag(Gaff::Hash32 tag) const
{
	const auto it = _render_device_tags.find(tag);
	return (it == _render_device_tags.end()) ? nullptr : &it->second;
}

const Vector<Gleam::IRenderDevice*>* RenderManagerBase::getDevicesByTag(const char* tag) const
{
	const Gaff::Hash32 hash = Gaff::FNV1aHash32String(tag);
	return getDevicesByTag(hash);
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

Vector<Gleam::IRenderDevice*> RenderManagerBase::getOrCreateThreadContexts(EA::Thread::ThreadId id)
{
	Vector<Gleam::IRenderDevice*> out;

	auto& devices = _deferred_devices[id];

	if (devices.empty()) {
		for (auto& device : _render_devices) {
			Gleam::IRenderDevice* const rd = device->createDeferredRenderDevice();
			devices.emplace_back(rd);
			out.emplace_back(rd);
		}
	}

	return out;
}

Gleam::IRenderOutput* RenderManagerBase::getOutput(Gaff::Hash32 tag) const
{
	const auto it = _window_outputs.find(tag);
	return (it == _window_outputs.end()) ? nullptr : it->second.second.get();
}

Gleam::IWindow* RenderManagerBase::getWindow(Gaff::Hash32 tag) const
{
	const auto it = _window_outputs.find(tag);
	return (it == _window_outputs.end()) ? nullptr : it->second.first.get();
}

Gleam::IProgramBuffers* RenderManagerBase::getCameraProgramBuffers(Gaff::Hash32 tag) const
{
	const auto it = _g_buffers.find(tag);
	return (it == _g_buffers.end()) ? nullptr : it->second.program_buffers.get();
}

Gleam::IRenderTarget* RenderManagerBase::getCameraRenderTarget(Gaff::Hash32 tag) const
{
	const auto it = _g_buffers.find(tag);
	return (it == _g_buffers.end()) ? nullptr : it->second.render_target.get();
}

const SamplerStateResourcePtr& RenderManagerBase::getDefaultSamplerState(void) const
{
	return _default_sampler;
}

SamplerStateResourcePtr& RenderManagerBase::getDefaultSamplerState(void)
{
	return _default_sampler;
}

Gleam::IRenderDevice* RenderManagerBase::createRenderDevice(int32_t adapter_id)
{
	Gleam::IRenderDevice* const rd = createRenderDevice();

	if (!rd->init(adapter_id)) {
		LogErrorDefault("Failed to create render device.");
		SHIB_FREET(rd, GetAllocator());
		return nullptr;
	}

	manageRenderDevice(rd);

	Gleam::ISamplerState* const sampler_state = createSamplerState();
	const Gleam::ISamplerState::SamplerSettings sampler_settings = {
		Gleam::ISamplerState::FILTER_NEAREST_NEAREST_NEAREST,
		Gleam::ISamplerState::WRAP_CLAMP,
		Gleam::ISamplerState::WRAP_CLAMP,
		Gleam::ISamplerState::WRAP_CLAMP,
		0.0f, 0.0f,
		0.0f,
		1,
		Gleam::COLOR_BLACK,
		Gleam::ComparisonFunc::NEVER
	};

	if (!sampler_state->init(*rd, sampler_settings)) {
		LogErrorDefault("Failed to create sampler state for render device.");
		SHIB_FREET(sampler_state, GetAllocator());
		SHIB_FREET(rd, GetAllocator());
		return nullptr;
	}

	_to_screen_samplers[rd].reset(sampler_state);

	return rd;
}

bool RenderManagerBase::createGBuffer(const char* window_name)
{
	const Gaff::Hash32 window_tag = Gaff::FNV1aHash32String(window_name);
	const Gleam::IWindow& window = *_window_outputs[window_tag].first;

	GBufferData data;
	data.program_buffers.reset(createProgramBuffers());
	data.render_target.reset(createRenderTarget());

	data.diffuse.reset(createTexture());
	data.specular.reset(createTexture());
	data.normal.reset(createTexture());
	data.position.reset(createTexture());
	data.depth.reset(createTexture());

	data.diffuse_srv.reset(createShaderResourceView());
	data.specular_srv.reset(createShaderResourceView());
	data.normal_srv.reset(createShaderResourceView());
	data.position_srv.reset(createShaderResourceView());
	data.depth_srv.reset(createShaderResourceView());

	Gleam::IRenderDevice& rd = *_render_device_tags[window_tag][0];
	const glm::ivec2& size = window.getSize();

	if (!data.diffuse->init2D(rd, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
		LogErrorDefault("Failed to create diffuse texture for window '%s'.", window_name);
		return false;
	}

	if (!data.specular->init2D(rd, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
		LogErrorDefault("Failed to create specular texture for window '%s'.", window_name);
		return false;
	}

	if (!data.normal->init2D(rd, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
		LogErrorDefault("Failed to create normal texture for window '%s'.", window_name);
		return false;
	}

	if (!data.position->init2D(rd, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
		LogErrorDefault("Failed to create position texture for window '%s'.", window_name);
		return false;
	}

	if (!data.depth->initDepthStencil(rd, size.x, size.y, Gleam::ITexture::Format::DEPTH_32_F)) {
		LogErrorDefault("Failed to create depth texture for window '%s'.", window_name);
		return false;
	}

	if (!data.diffuse_srv->init(rd, data.diffuse.get())) {
		LogErrorDefault("Failed to create diffuse srv for window '%s'.", window_name);
		return false;
	}

	if (!data.specular_srv->init(rd, data.specular.get())) {
		LogErrorDefault("Failed to create specular srv for window '%s'.", window_name);
		return false;
	}

	if (!data.normal_srv->init(rd, data.normal.get())) {
		LogErrorDefault("Failed to create normal srv for window '%s'.", window_name);
		return false;
	}

	if (!data.position_srv->init(rd, data.position.get())) {
		LogErrorDefault("Failed to create position srv for window '%s'.", window_name);
		return false;
	}

	if (!data.depth_srv->init(rd, data.depth.get())) {
		LogErrorDefault("Failed to create depth srv for window '%s'.", window_name);
		return false;
	}

	data.program_buffers->addResourceView(Gleam::IShader::SHADER_PIXEL, data.diffuse_srv.get());
	data.program_buffers->addResourceView(Gleam::IShader::SHADER_PIXEL, data.specular_srv.get());
	data.program_buffers->addResourceView(Gleam::IShader::SHADER_PIXEL, data.normal_srv.get());
	data.program_buffers->addResourceView(Gleam::IShader::SHADER_PIXEL, data.position_srv.get());
	data.program_buffers->addResourceView(Gleam::IShader::SHADER_PIXEL, data.depth_srv.get());
	data.program_buffers->addSamplerState(Gleam::IShader::SHADER_PIXEL, _to_screen_samplers[&rd].get());

	data.render_target->addTexture(rd, data.diffuse.get());
	data.render_target->addTexture(rd, data.specular.get());
	data.render_target->addTexture(rd, data.normal.get());
	data.render_target->addTexture(rd, data.position.get());
	data.render_target->addDepthStencilBuffer(rd, data.depth.get());

	_g_buffers[window_tag] = std::move(data);
	return true;
}

NS_END
