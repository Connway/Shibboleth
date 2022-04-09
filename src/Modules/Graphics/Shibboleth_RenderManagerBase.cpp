/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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
#include "Shibboleth_GraphicsLogging.h"
#include "Shibboleth_GraphicsConfigs.h"
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_SerializeReader.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_IApp.h>
#include <Gleam_Keyboard_MessagePump.h>
#include <Gleam_Mouse_MessagePump.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IRenderDevice.h>
#include <Gaff_Assert.h>
#include <Gaff_JSON.h>
#include <Gaff_Math.h>
#include <gtx/quaternion.hpp>

NS_SHIBBOLETH

static ProxyAllocator g_allocator("Graphics");

// Change this if the game supports more than one monitor.
// "main" display is implicit. Not necessary to explicitly reference it.
static constexpr const char8_t* g_supported_displays[] = { nullptr };

const VectorMap< Gaff::Hash32, Vector<const char8_t*> > g_display_tags = {
	{ Gaff::FNV1aHash32Const(u8"gameplay"), { u8"main" } }
};

static constexpr const char8_t* const g_graphics_cfg_schema =
u8R"({
	"type": "object",

	"properties":
	{
		"icon": { "type": "string" },

		"adapters":
		{
			"type": "object",
			"additionalProperties":
			{
				"type": "array",
				"items": { "type": "integer", "minimum": 0 }
			},
		},

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
					"vsync": { "type": "boolean" },
					"icon": { "type": "string" }
				},

				"required": ["width", "height", "refresh_rate", "window_mode", "adapter_id", "display_id", "vsync"],
				"additionalProperties": false
			}
		},
	}
})";

RenderManagerBase::RenderManagerBase(void)
{
	// $TODO: This needs to be rejiggered. This is confusing to follow.
	_render_device_tags.reserve(ARRAY_SIZE(g_supported_displays) + g_display_tags.size());
	
	const ProxyAllocator allocator("Graphics");

	if constexpr (g_supported_displays[0]) {
		for (const char8_t* tag : g_supported_displays) {
			_render_device_tags[Gaff::FNV1aHash32StringConst(tag)] = Vector<Gleam::IRenderDevice*>(allocator);
		}
	}

	// Always have a main window. Even if not specified.
	_render_device_tags[Gaff::FNV1aHash32StringConst(u8"main")] = Vector<Gleam::IRenderDevice*>(allocator);

	for (auto entry : g_display_tags) {
		_render_device_tags[entry.first] = Vector<Gleam::IRenderDevice*>(allocator);
	}
}

RenderManagerBase::~RenderManagerBase(void)
{
	for (int32_t i = 0; i < ARRAY_SIZE(_cached_render_commands); ++i) {
		for (int32_t j = 0; j < ARRAY_SIZE(_cached_render_commands[i]); ++j) {
			for (auto& pair : _cached_render_commands[i][j]) {
				for (auto& cmd : pair.second.command_list) {
					if (!cmd.owns_command) {
						cmd.cmd_list.release();
					}
				}
			}
		}
	}
}

bool RenderManagerBase::initAllModulesLoaded(void)
{
	IApp& app = GetApp();
	const Gaff::JSON& configs = app.getConfigs();
	const char8_t* const graphics_cfg_path = configs[k_config_graphics_cfg].getString(k_config_graphics_default_cfg);

	IFileSystem& fs = app.getFileSystem();
	const IFile* const file = fs.openFile(graphics_cfg_path);

	if (!file) {
		// $TODO: Log error.
		return false;
	}

	Gaff::JSON config;

	if (!config.parse(reinterpret_cast<const char8_t*>(file->getBuffer()), g_graphics_cfg_schema)) {
		LogErrorGraphics("Failed to parse config file with error - %s.", config.getErrorText());
		fs.closeFile(file);

		return false;
	}

	fs.closeFile(file);

	const Gaff::JSON sampler = config[u8"texture_filtering"];

	if (sampler.isString()) {
		ResourceManager& res_mgr = app.getManagerTFast<ResourceManager>();

		_default_sampler = res_mgr.requestResourceT<SamplerStateResource>(sampler.getString());

		if (!_default_sampler) {
			// $TODO: Log error.
			return false;
		}

		res_mgr.waitForResource(*_default_sampler);

		if (_default_sampler->hasFailed()) {
			// $TODO: Log error.
			return false;
		}
	}

	return true;
}

bool RenderManagerBase::init(void)
{
	IApp& app = GetApp();
	const Gaff::JSON& configs = app.getConfigs();
	const char8_t* const graphics_cfg_path = configs[k_config_graphics_cfg].getString(k_config_graphics_default_cfg);

	app.getLogManager().addChannel(HashStringView32<>(k_log_channel_name_graphics));


	IFileSystem& fs = app.getFileSystem();
	const IFile* const file = fs.openFile(graphics_cfg_path);

	if (!file) {
		// $TODO: Generate default config file.
		return false;
	}

	Gaff::JSON config;

	if (!config.parse(reinterpret_cast<const char8_t*>(file->getBuffer()), g_graphics_cfg_schema)) {
		const char8_t* const error = config.getErrorText();
		LogErrorGraphics("Failed to parse config file with error - %s.", error);
		fs.closeFile(file);
		return false;
	}

	if (configs[k_config_graphics_no_windows].getBool(false)) {
		const Gaff::JSON adapters = config[u8"adapters"];

		if (adapters.isObject() && adapters.size() > 0) {
			adapters.forEachInObject([&](const char8_t* key, const Gaff::JSON& value) -> bool
			{
				value.forEachInArray([&](int32_t /*index*/, const Gaff::JSON& adapter_id) -> bool
				{
					const int32_t aid = adapter_id.getInt32();
					Gleam::IRenderDevice* const rd = createRenderDevice(aid);

					if (!rd) {
						LogErrorGraphics("Failed to create render device with adapter id '%i'.", aid);
						return false;
					}

					// Nothing left to do, the render device is already part of the _render_device list.
					const Gaff::Hash32 tag_hash = Gaff::FNV1aHash32String(key);
					_render_device_tags[tag_hash].emplace_back(rd);

					_render_device_tags[Gaff::FNV1aHash32Const(u8"all")].emplace_back(rd);

					return false;
				});

				return false;
			});

		// Always attempt to create a render device at adapter ID 0.
		} else {
			Gleam::IRenderDevice* const rd = createRenderDevice(0);

			if (!rd) {
				LogErrorGraphics("Failed to create render device with adapter id '0'.");
				return false;
			}

			// Nothing left to do, the render device is already part of the _render_device list.
			const Gaff::Hash32 tag_hash = Gaff::FNV1aHash32String(u8"main");
			_render_device_tags[tag_hash].emplace_back(rd);

			_render_device_tags[Gaff::FNV1aHash32Const(u8"all")].emplace_back(rd);
		}

	} else {
		const Gaff::JSON windows = config[u8"windows"];

		if (windows.isObject()) {
			windows.forEachInObject([&](const char8_t* key, const Gaff::JSON& value) -> bool
			{
				const int32_t adapter_id = value[u8"adapter_id"].getInt32();
				const int32_t display_id = value[u8"display_id"].getInt32();

				const auto display_modes = getDisplayModes();

				if (adapter_id >= static_cast<int32_t>(display_modes.size())) {
					LogErrorGraphics("Invalid 'adapter_id' for window '%s'.", key);
					return false;
				}

				const auto& adapter_info = display_modes[adapter_id];

				if (display_id >= static_cast<int32_t>(adapter_info.displays.size())) {
					LogErrorGraphics("Invalid 'display_id' for window '%s'.", key);
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
						LogErrorGraphics("Failed to create render device for window '%s' with adapter id '%i'.", key, adapter_id);
						return false;
					}
				}

				int32_t x = Gaff::Max(0, value[u8"x"].getInt32(0));
				int32_t y = Gaff::Max(0, value[u8"y"].getInt32(0));
				int32_t width = value[u8"width"].getInt32(0);
				int32_t height = value[u8"height"].getInt32(0);
				const int32_t refresh_rate = value[u8"refresh_rate"].getInt32(0);
				const bool vsync = value[u8"vsync"].getBool();
				Gleam::IWindow::WindowMode window_mode = Gleam::IWindow::WindowMode::Windowed;

				SerializeReader<Gaff::JSON> reader(value[u8"window_mode"], ProxyAllocator("Graphics"));

				// Log the error, but leave default to windowed mode.
				if (!Refl::Reflection<Gleam::IWindow::WindowMode>::GetInstance().load(reader, window_mode)) {
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
					LogErrorGraphics("Failed to create window '%s'.", key);
					SHIB_FREET(window, GetAllocator());
					return false;
				}

				if (const Gaff::JSON icon = value[u8"icon"]; icon.isString()) {
					if (!window->setIcon(icon.getString())) {
						// $TODO: Log warning.
					}
				}

				// Add the device to the window tag.
				const Gaff::Hash32 window_hash = Gaff::FNV1aHash32String(key);
				_render_device_tags[window_hash].emplace_back(rd);

				_render_device_tags[Gaff::FNV1aHash32Const(u8"all")].emplace_back(rd);

				// Add render device to tag list if not already present.
				for (const auto& entry : g_display_tags) {
					const auto it = Gaff::Find(entry.second, key, [](const char8_t* lhs, const char8_t* rhs) -> bool
						{
							return eastl::u8string_view(lhs) == eastl::u8string_view(rhs);
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
					LogErrorGraphics("Failed to create render output for window '%s'.", key);
					SHIB_FREET(output, GetAllocator());
					SHIB_FREET(window, GetAllocator());
					return false;
				}

				_window_outputs[window_hash] = eastl::make_pair(std::move(WindowPtr(window)), std::move(OutputPtr(output)));
				return false;
			});

		} else {
			// $TODO: Log error.
			return false;
		}
	}

	fs.closeFile(file);

	const auto& job_pool = app.getJobPool();
	Vector<EA::Thread::ThreadId> thread_ids(job_pool.getNumTotalThreads(), ProxyAllocator("Graphics"));

	job_pool.getThreadIDs(thread_ids.data());
	_deferred_contexts.reserve(_render_devices.size());

	// Loop over all devices.
	for (const auto& rd : _render_devices) {
		auto& context_map = _deferred_contexts[rd.get()];
		context_map.reserve(thread_ids.size());

		// Loop over all thread ids and create deferred contexts for them.
		for (EA::Thread::ThreadId id : thread_ids) {
			Gleam::IRenderDevice* const deferred_device = rd->createDeferredRenderDevice();

			if (!deferred_device) {
				LogErrorGraphics("Failed to create deferred render device for threads.");
				return false;
			}

			context_map[id].reset(deferred_device);
		}
	}

	return true;
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

	for (int32_t i = 0; i < static_cast<int32_t>(RenderOrder::Count); ++i) {
		for (int32_t j = 0; j < 2; ++j) {
			// Create entry for newly managed device.
			_cached_render_commands[i][j][device];
		}
	}
}

const Vector<Gleam::IRenderDevice*>* RenderManagerBase::getDevicesByTag(Gaff::Hash32 tag) const
{
	const auto it = _render_device_tags.find(tag);
	return (it == _render_device_tags.end()) ? nullptr : &it->second;
}

const Vector<Gleam::IRenderDevice*>* RenderManagerBase::getDevicesByTag(const char8_t* tag) const
{
	const Gaff::Hash32 hash = Gaff::FNV1aHash32String(tag);
	return getDevicesByTag(hash);
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

void RenderManagerBase::removeWindow(const Gleam::IWindow& window)
{
	for (auto it = _window_outputs.begin(); it != _window_outputs.end(); ++it) {
		if (it->second.first.get() == &window) {
			_window_outputs.erase(it);
			break;
		}
	}
}

int32_t RenderManagerBase::getNumWindows(void) const
{
	return static_cast<int32_t>(_window_outputs.size());
}

const SamplerStateResourcePtr& RenderManagerBase::getDefaultSamplerState(void) const
{
	return _default_sampler;
}

SamplerStateResourcePtr& RenderManagerBase::getDefaultSamplerState(void)
{
	return _default_sampler;
}

bool RenderManagerBase::createGBuffer(
	EntityID id,
	Gaff::Hash32 device_tag,
	const Gleam::IVec2& size,
	bool create_render_texture)
{
	auto it = _g_buffers.find(id);

	if (it != _g_buffers.end()) {
		return true;
	}

	it = _g_buffers.emplace(id, VectorMap<const Gleam::IRenderDevice*, GBufferData>{ ProxyAllocator("Graphics") }).first;

	const auto* const devices = getDevicesByTag(device_tag);

	if (!devices || devices->empty()) {
		// $TODO: Log error.
		return false;
	}

	for (Gleam::IRenderDevice* device : *devices) {
		GBufferData data;
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

		if (!data.diffuse->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
			LogErrorGraphics("Failed to create diffuse texture for camera [%u / %i].", device_tag, id);
			return false;
		}

		if (!data.specular->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
			LogErrorGraphics("Failed to create specular texture for camera [%u / %i].", device_tag, id);
			return false;
		}

		if (!data.normal->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
			LogErrorGraphics("Failed to create normal texture for camera [%u / %i].", device_tag, id);
			return false;
		}

		if (!data.position->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
			LogErrorGraphics("Failed to create position texture for camera [%u / %i].", device_tag, id);
			return false;
		}

		if (!data.depth->initDepthStencil(*device, size.x, size.y, Gleam::ITexture::Format::DEPTH_32_F)) {
			LogErrorGraphics("Failed to create depth texture for camera [%u / %i].", device_tag, id);
			return false;
		}

		if (!data.diffuse_srv->init(*device, data.diffuse.get())) {
			LogErrorGraphics("Failed to create diffuse srv for camera [%u / %i].", device_tag, id);
			return false;
		}

		if (!data.specular_srv->init(*device, data.specular.get())) {
			LogErrorGraphics("Failed to create specular srv for camera [%u / %i].", device_tag, id);
			return false;
		}

		if (!data.normal_srv->init(*device, data.normal.get())) {
			LogErrorGraphics("Failed to create normal srv for camera [%u / %i].", device_tag, id);
			return false;
		}

		if (!data.position_srv->init(*device, data.position.get())) {
			LogErrorGraphics("Failed to create position srv for camera [%u / %i].", device_tag, id);
			return false;
		}

		if (!data.depth_srv->init(*device, data.depth.get())) {
			LogErrorGraphics("Failed to create depth srv for camera [%u / %i].", device_tag, id);
			return false;
		}

		data.render_target->addTexture(*device, data.diffuse.get());
		data.render_target->addTexture(*device, data.specular.get());
		data.render_target->addTexture(*device, data.normal.get());
		data.render_target->addTexture(*device, data.position.get());
		data.render_target->addDepthStencilBuffer(*device, data.depth.get());

		if (create_render_texture) {
			data.final_image.reset(createTexture());

			if (!data.final_image->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
				LogErrorGraphics("Failed to create final output texture for camera [%u / %i].", device_tag, id);
				return false;
			}

			data.final_srv.reset(createShaderResourceView());

			if (!data.final_srv->init(*device, data.final_image.get())) {
				LogErrorGraphics("Failed to create final output srv for camera [%u / %i].", device_tag, id);
				return false;
			}

			data.final_render_target.reset(createRenderTarget());

			if (!data.final_render_target->addTexture(*device, data.final_image.get())) {
				LogErrorGraphics("Failed to create final output render target for camera [%u / %i].", device_tag, id);
				return false;
			}
		}

		it->second.emplace(device, std::move(data));
	}

	return true;
}

const RenderManagerBase::GBufferData* RenderManagerBase::getGBuffer(EntityID id, const Gleam::IRenderDevice& device) const
{
	const auto rd_it = _g_buffers.find(id);

	if (rd_it != _g_buffers.end()) {
		const auto gb_it = rd_it->second.find(&device);
		return (gb_it != rd_it->second.end()) ? &gb_it->second : nullptr;
	}

	return nullptr;
}

bool RenderManagerBase::removeGBuffer(EntityID id)
{
	return _g_buffers.erase(id) > 0;
}

bool RenderManagerBase::hasGBuffer(EntityID id, const Gleam::IRenderDevice& device) const
{
	const auto it = _g_buffers.find(id);

	if (it != _g_buffers.end()) {
		return it->second.find(&device) != it->second.end();
	}

	return false;
}

bool RenderManagerBase::hasGBuffer(EntityID id) const
{
	return _g_buffers.find(id) != _g_buffers.end();
}

const RenderManagerBase::RenderCommandList& RenderManagerBase::getRenderCommands(const Gleam::IRenderDevice& device, RenderOrder order, int32_t cache_index) const
{
	return const_cast<RenderManagerBase*>(this)->getRenderCommands(device, order, cache_index);
}

RenderManagerBase::RenderCommandList& RenderManagerBase::getRenderCommands(const Gleam::IRenderDevice& device, RenderOrder order, int32_t cache_index)
{
	GAFF_ASSERT(Gaff::Between(cache_index, 0, 1));
	const auto it = _cached_render_commands[static_cast<int32_t>(order)][cache_index].find(&device);
	GAFF_ASSERT(it != _cached_render_commands[static_cast<int32_t>(order)][cache_index].end());

	return it->second;
}

void RenderManagerBase::presentAllOutputs(void)
{
	for (auto& pair : _window_outputs) {
		pair.second.second->present();
	}
}

const Gleam::IRenderDevice* RenderManagerBase::getDeferredDevice(const Gleam::IRenderDevice& device, EA::Thread::ThreadId thread_id) const
{
	return const_cast<RenderManagerBase*>(this)->getDeferredDevice(device, thread_id);
}

Gleam::IRenderDevice* RenderManagerBase::getDeferredDevice(const Gleam::IRenderDevice& device, EA::Thread::ThreadId thread_id)
{
	const auto device_it = _deferred_contexts.find(&device);
	GAFF_ASSERT(device_it != _deferred_contexts.end());

	const auto thread_it = device_it->second.find(thread_id);
	GAFF_ASSERT(thread_it != device_it->second.end());

	return thread_it->second.get();
}

Gleam::IRenderDevice* RenderManagerBase::createRenderDevice(int32_t adapter_id)
{
	Gleam::IRenderDevice* const rd = createRenderDevice();

	if (!rd->init(adapter_id)) {
		LogErrorGraphics("Failed to create render device.");
		SHIB_FREET(rd, GetAllocator());
		return nullptr;
	}

	manageRenderDevice(rd);

	Gleam::ISamplerState* const sampler_state = createSamplerState();
	const Gleam::ISamplerState::Settings sampler_settings = {
		Gleam::ISamplerState::Filter::NearestNearestNearest,
		Gleam::ISamplerState::Wrap::Clamp,
		Gleam::ISamplerState::Wrap::Clamp,
		Gleam::ISamplerState::Wrap::Clamp,
		0.0f, 0.0f,
		0.0f,
		1,
		Gleam::Color::Black,
		Gleam::ComparisonFunc::Never
	};

	if (!sampler_state->init(*rd, sampler_settings)) {
		LogErrorGraphics("Failed to create sampler state for render device.");
		SHIB_FREET(sampler_state, GetAllocator());
		SHIB_FREET(rd, GetAllocator());
		return nullptr;
	}

	_to_screen_samplers[rd].reset(sampler_state);

	return rd;
}

NS_END
