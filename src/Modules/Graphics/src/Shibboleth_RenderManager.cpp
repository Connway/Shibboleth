/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
#include "Shibboleth_GraphicsReflection.h"
#include "Shibboleth_GraphicsLogging.h"
#include "Shibboleth_GraphicsConfig.h"
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_SerializeReader.h>
#include <Log/Shibboleth_LogManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_Image.h>
#include <Shibboleth_IApp.h>
#include <Gleam_ShaderResourceView.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_RenderOutput.h>
#include <Gleam_SamplerState.h>
#include <Gleam_CommandList.h>
#include <Gleam_Texture.h>
#include <Gleam_Window.h>
#include <Gaff_Function.h>
#include <Gaff_Assert.h>
#include <Gaff_JSON.h>
#include <Gaff_Math.h>
#include <gtx/quaternion.hpp>
#include <GLFW/glfw3.h>
#include <Gleam_IncludeGLFWNative.h>

#ifdef PLATFORM_MAC
	#include <IOKit/graphics/IOGraphicsLib.h>
#endif

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::RenderManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::RenderManager)

namespace
{
	static Shibboleth::ProxyAllocator g_allocator("Graphics");
}



NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(RenderManager)

RenderManager::~RenderManager(void)
{
	for (int32_t i = 0; static_cast<size_t>(i) < std::size(_cached_render_commands); ++i) {
		for (int32_t j = 0; static_cast<size_t>(j) < std::size(_cached_render_commands[i].command_lists); ++j) {
			for (auto& pair : _cached_render_commands[i].command_lists[j]) {
				for (auto& cmd : pair.second.command_list) {
					GAFF_REF(cmd);
//					if (!cmd.owns_command) {
//						cmd.cmd_list.release();
//					}
				}
			}
		}
	}

	Gleam::Window::GlobalShutdown();
}

// $TODO: Break this function down.
bool RenderManager::init(void)
{
	if (!Gleam::Window::GlobalInit()) {
		// $TODO: Log error.
		return false;
	}

	IApp& app = GetApp();
	app.getLogManager().addChannel(HashStringView32<>(k_log_channel_name_graphics));

	const GraphicsConfig config = GetConfigRef<GraphicsConfig>();

	const_cast<GraphicsConfig&>(config).texture_filtering_sampler.requestLoad();

	if (!config.texture_filtering_sampler->waitUntilLoaded()) {
		// $TODO: Log error.
		return false;
	}

	_default_sampler = config.texture_filtering_sampler;

	const_cast<GraphicsConfig&>(config).icon.requestLoad();
	GLFWimage icon;

	if (config.icon->waitUntilLoaded()) {
		config.icon->fillGLFWImage(icon);
	} else {
		// $TODO: Log warning.
	}

	if (config.windows.empty()) {
		// $TODO: Add support to this section to use adapter names or monitor IDs.
		/*const Gaff::JSON adapters = config.getObject(u8"adapters");

		if (adapters.isObject() && adapters.size() > 0) {
			adapters.forEachInObject([&](const char8_t* key, const Gaff::JSON& value) -> bool
			{
				value.forEachInArray([&](int32_t index, const Gaff::JSON& adapter_id) -> bool
				{
					const int32_t aid = adapter_id.getInt32();
					Gleam::IRenderDevice* const rd = createRenderDeviceFromAdapter(aid);

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
			Gleam::RenderDevice* const rd = createRenderDeviceFromAdapter(0);

			if (!rd) {
				LogErrorGraphics("Failed to create render device with adapter id '0'.");
				return false;
			}

			// Nothing left to do, the render device is already part of the _render_device list.
			const Gaff::Hash32 tag_hash = Gaff::FNV1aHash32String(u8"main");
			_render_device_tags[tag_hash].emplace_back(rd);

			_render_device_tags[Gaff::FNV1aHash32Const(u8"all")].emplace_back(rd);
		}*/

		return false;

	} else {
		int monitor_count = 0;
		GLFWmonitor*const * const monitors = glfwGetMonitors(&monitor_count);

		for (const auto& entry : config.windows) {
			if (!Gaff::ValidIndex(entry.second.monitor_id, monitor_count)) {
				// $TODO: Log error.
				continue;
			}

			GLFWmonitor* const monitor = monitors[entry.second.monitor_id];
			Gleam::Window* window = nullptr;

			int32_t refresh_rate = -1;

			if (entry.second.windowed) {
				window = SHIB_ALLOCT(Gleam::Window, g_allocator);

				if (!window->initWindowed(entry.first.getBuffer(), Gleam::IVec2(entry.second.width, entry.second.height))) {
					LogErrorGraphics("Failed to create window '%s'.", reinterpret_cast<const char*>(entry.first.getBuffer()));
					SHIB_FREET(window, GetAllocator());

					continue;
				}

			} else {
				// Find video mode referenced in config settings.
				int video_mode_count = 0;
				const GLFWvidmode* const video_modes = glfwGetVideoModes(monitor, &video_mode_count);

				const GLFWvidmode* video_mode = eastl::find(video_modes, video_modes + video_mode_count, entry.second, [](const GLFWvidmode& lhs, const GraphicsConfigWindow& rhs) -> bool
				{
					return lhs.width == rhs.width &&
						lhs.height == rhs.height &&
						lhs.refreshRate == rhs.refresh_rate;
				});

				// Didn't find a video mode that matches current settings. Use current desktop video mode.
				if (!video_mode) {
					video_mode = glfwGetVideoMode(monitor);

					// Still don't have a video mode. Something really bad is happening.
					if (!video_mode) {
						LogErrorGraphics("Failed to find video mode for window '%s'.", reinterpret_cast<const char*>(entry.first.getBuffer()));
						continue;
					}

					refresh_rate = video_mode->refreshRate;
				}

				window = SHIB_ALLOCT(Gleam::Window, g_allocator);

				if (!window->initFullscreen(entry.first.getBuffer(), *monitor, *video_mode)) {
					LogErrorGraphics("Failed to create window '%s'.", reinterpret_cast<const char*>(entry.first.getBuffer()));
					SHIB_FREET(window, GetAllocator());

					continue;
				}
			}

			window->addCloseCallback(Gaff::MemberFunc(this, &RenderManager::handleWindowClosed));

			// $TODO: create render devices and outputs.


			Gleam::RenderDevice* rd = nullptr;

			// Check if we've already created this device.
			for (const auto& render_device : _render_devices) {
				if (render_device->isUsedBy(*window)) {
					rd = render_device.get();
					break;
				}
			}

			// Create it if we don't already have it.
			if (!rd) {
				rd = createRenderDevice(*window);

				if (!rd) {
					LogErrorGraphics("Failed to create render device for window '%s'.", reinterpret_cast<const char*>(entry.first.getBuffer()));
					continue;
				}
			}

			if (config.icon->isLoaded()) {
				window->setIcon(icon);
			}

			// Add the device to the window tag.
			const Gaff::Hash32 window_hash = Gaff::FNV1aHash32String(entry.first.getBuffer());
			Gaff::InsertSorted(_render_device_tags[window_hash], rd);

			Gaff::InsertSorted(_render_device_tags[Gaff::FNV1aHash32Const(u8"all")], rd);

			// Add render device to tag list if not already present.
			for (const HashString32<>& tag : entry.second.tags) {
				auto& render_devices = _render_device_tags[tag.getHash()];

				if (!Gaff::Contains(render_devices, rd)) {
					Gaff::InsertSorted(render_devices, rd);
				}
			}

			Gleam::RenderOutput* const output = SHIB_ALLOCT(Gleam::RenderOutput, g_allocator);

			if (!output->init(*rd, *window, refresh_rate, entry.second.vsync)) {
				LogErrorGraphics("Failed to create render output for window '%s'.", reinterpret_cast<const char*>(entry.first.getBuffer()));
				SHIB_FREET(output, GetAllocator());
				SHIB_FREET(window, GetAllocator());

				continue;
			}

			auto& output_entry = _outputs[window_hash];
			output_entry.render_device = rd;
			output_entry.window.reset(window);
			output_entry.output.reset(output);
		}
	}

	const auto& job_pool = app.getJobPool();
	Vector<EA::Thread::ThreadId> thread_ids(job_pool.getNumTotalThreads(), GRAPHICS_ALLOCATOR);

	job_pool.getThreadIDs(thread_ids.data());
	_deferred_devices.reserve(_render_devices.size());

	// Loop over all devices.
	for (const auto& rd : _render_devices) {
		auto& context_map = _deferred_devices[rd.get()];
		context_map.reserve(thread_ids.size());

		// Loop over all thread ids and create deferred contexts for them.
		for (EA::Thread::ThreadId id : thread_ids) {
			Gleam::IRenderDevice* const deferred_device = rd->createDeferredRenderDevice();

			if (!deferred_device) {
				LogErrorGraphics("Failed to create deferred render device for threads.");
				return false;
			}

			context_map[id].reset(static_cast<Gleam::RenderDevice*>(deferred_device));
		}
	}

	if (!_render_pipeline.init(*this)) {
		return false;
	}

	return true;
}

//void RenderManager::updateWindows(void)
//{
//	Gleam::Window::PollEvents();
//}

void RenderManager::manageRenderDevice(Gleam::RenderDevice& device)
{
	const auto it = Gaff::Find(_render_devices, device, [](const auto& lhs, const auto& rhs) -> bool { return lhs.get() == &rhs; });
	GAFF_ASSERT(it == _render_devices.end());

	_render_devices.emplace_back(&device);

	for (int32_t i = 0; i < static_cast<int32_t>(RenderOrder::Count); ++i) {
		for (int32_t j = 0; j < RenderCommandData::CacheIndexCount; ++j) {
			// Create entry for newly managed device.
			_cached_render_commands[i].command_lists[j].insert(&device);
		}
	}
}

const Gleam::RenderDevice* RenderManager::getDevice(const Gleam::RenderOutput& output) const
{
	return const_cast<RenderManager*>(this)->getDevice(output);
}

const Gleam::RenderDevice& RenderManager::getDevice(int32_t index) const
{
	return const_cast<RenderManager*>(this)->getDevice(index);
}

Gleam::RenderDevice* RenderManager::getDevice(const Gleam::RenderOutput& output)
{
	for (const auto& element : _outputs) {
		if (element.second.output.get() == &output) {
			return element.second.render_device;
		}
	}

	return nullptr;
}

Gleam::RenderDevice& RenderManager::getDevice(int32_t index)
{
	GAFF_ASSERT(Gaff::ValidIndex(index, static_cast<int32_t>(_render_devices.size())));
	return *_render_devices[index];
}

const Vector<RenderManager::RenderDevicePtr>& RenderManager::getDevices(void) const
{
	return _render_devices;
}

int32_t RenderManager::getNumDevices(void) const
{
	return static_cast<int32_t>(_render_devices.size());
}

const Vector<Gleam::RenderDevice*>* RenderManager::getDevicesByTag(Gaff::Hash32 tag) const
{
	const auto it = _render_device_tags.find(tag);
	return (it != _render_device_tags.end()) ? &it->second : nullptr;
}

const Vector<Gleam::RenderDevice*>* RenderManager::getDevicesByTag(const char8_t* tag) const
{
	return getDevicesByTag(Gaff::FNV1aHash32String(tag));
}

const Vector<Gleam::RenderDevice*>* RenderManager::getDevicesByTag(const char* tag) const
{
	return getDevicesByTag(Gaff::FNV1aHash32String(tag));
}

const Gleam::RenderOutput* RenderManager::getOutput(Gaff::Hash32 tag) const
{
	return const_cast<RenderManager*>(this)->getOutput(tag);
}

const Gleam::RenderOutput* RenderManager::getOutput(const char8_t* tag) const
{
	return const_cast<RenderManager*>(this)->getOutput(tag);
}

const Gleam::RenderOutput* RenderManager::getOutput(const char* tag) const
{
	return const_cast<RenderManager*>(this)->getOutput(tag);
}

const Gleam::RenderOutput* RenderManager::getOutput(int32_t index) const
{
	return const_cast<RenderManager*>(this)->getOutput(index);
}

Gleam::RenderOutput* RenderManager::getOutput(Gaff::Hash32 tag)
{
	const auto it = _outputs.find(tag);
	return (it == _outputs.end()) ? nullptr : it->second.output.get();
}

Gleam::RenderOutput* RenderManager::getOutput(const char8_t* tag)
{
	return getOutput(Gaff::FNV1aHash32String(tag));
}

Gleam::RenderOutput* RenderManager::getOutput(const char* tag)
{
	return getOutput(Gaff::FNV1aHash32String(tag));
}

Gleam::RenderOutput* RenderManager::getOutput(int32_t index)
{
	GAFF_ASSERT(Gaff::ValidIndex(index, static_cast<int32_t>(_outputs.size())));
	return _outputs.data()[index].second.output.get();
}

const Gleam::Window* RenderManager::getWindow(Gaff::Hash32 tag) const
{
	return const_cast<RenderManager*>(this)->getWindow(tag);
}

const Gleam::Window* RenderManager::getWindow(const char8_t* tag) const
{
	return const_cast<RenderManager*>(this)->getWindow(tag);
}

const Gleam::Window* RenderManager::getWindow(const char* tag) const
{
	return const_cast<RenderManager*>(this)->getWindow(tag);
}

const Gleam::Window* RenderManager::getWindow(int32_t index) const
{
	return const_cast<RenderManager*>(this)->getWindow(index);
}

Gleam::Window* RenderManager::getWindow(Gaff::Hash32 tag)
{
	const auto it = _outputs.find(tag);
	return (it == _outputs.end()) ? nullptr : it->second.window.get();
}

Gleam::Window* RenderManager::getWindow(const char8_t* tag)
{
	return getWindow(Gaff::FNV1aHash32String(tag));
}

Gleam::Window* RenderManager::getWindow(const char* tag)
{
	return getWindow(Gaff::FNV1aHash32String(tag));
}

Gleam::Window* RenderManager::getWindow(int32_t index)
{
	GAFF_ASSERT(Gaff::ValidIndex(index, static_cast<int32_t>(_outputs.size())));
	return _outputs.data()[index].second.window.get();
}

void RenderManager::removeWindow(const Gleam::Window& window)
{
	for (auto it = _outputs.begin(); it != _outputs.end(); ++it) {
		if (it->second.window.get() == &window) {
			// Move over to pending for now. If we delete in the middle of a close window callback,
			// this would delete the window before the other callbacks have had a chance to be called.
			auto& pending = _pending_window_removes.emplace_back();
			pending = std::move(it->second);

			_outputs.erase(it);
			break;
		}
	}
}

int32_t RenderManager::getNumWindows(void) const
{
	int32_t count = 0;

	for (const auto& entry : _outputs) {
		if (entry.second.window) {
			++count;
		}
	}

	return count;
}

const ResourcePtr<SamplerStateResource>& RenderManager::getDefaultSamplerState(void) const
{
	return _default_sampler;
}

ResourcePtr<SamplerStateResource>& RenderManager::getDefaultSamplerState(void)
{
	return _default_sampler;
}

//bool RenderManager::createGBuffer(
//	ECSEntityID id,
//	Gaff::Hash32 device_tag,
//	const Gleam::IVec2& size,
//	bool create_render_texture)
//{
//	auto it = _g_buffers.find(id);
//
//	if (it != _g_buffers.end()) {
//		return true;
//	}
//
//	it = _g_buffers.emplace(id, VectorMap<const Gleam::IRenderDevice*, GBufferData>{ GRAPHICS_ALLOCATOR }).first;
//
//	const auto* const devices = getDevicesByTag(device_tag);
//
//	if (!devices || devices->empty()) {
//		// $TODO: Log error.
//		return false;
//	}
//
//	for (Gleam::IRenderDevice* device : *devices) {
//		GBufferData data;
//		data.render_target.reset(createRenderTarget());
//
//		data.diffuse.reset(createTexture());
//		data.specular.reset(createTexture());
//		data.normal.reset(createTexture());
//		data.position.reset(createTexture());
//		data.depth.reset(createTexture());
//
//		data.diffuse_srv.reset(createShaderResourceView());
//		data.specular_srv.reset(createShaderResourceView());
//		data.normal_srv.reset(createShaderResourceView());
//		data.position_srv.reset(createShaderResourceView());
//		data.depth_srv.reset(createShaderResourceView());
//
//		if (!data.diffuse->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
//			LogErrorGraphics("Failed to create diffuse texture for camera [%u / %i].", device_tag, id);
//			return false;
//		}
//
//		if (!data.specular->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
//			LogErrorGraphics("Failed to create specular texture for camera [%u / %i].", device_tag, id);
//			return false;
//		}
//
//		if (!data.normal->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
//			LogErrorGraphics("Failed to create normal texture for camera [%u / %i].", device_tag, id);
//			return false;
//		}
//
//		if (!data.position->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
//			LogErrorGraphics("Failed to create position texture for camera [%u / %i].", device_tag, id);
//			return false;
//		}
//
//		if (!data.depth->initDepthStencil(*device, size.x, size.y, Gleam::ITexture::Format::DEPTH_32_F)) {
//			LogErrorGraphics("Failed to create depth texture for camera [%u / %i].", device_tag, id);
//			return false;
//		}
//
//		if (!data.diffuse_srv->init(*device, data.diffuse.get())) {
//			LogErrorGraphics("Failed to create diffuse srv for camera [%u / %i].", device_tag, id);
//			return false;
//		}
//
//		if (!data.specular_srv->init(*device, data.specular.get())) {
//			LogErrorGraphics("Failed to create specular srv for camera [%u / %i].", device_tag, id);
//			return false;
//		}
//
//		if (!data.normal_srv->init(*device, data.normal.get())) {
//			LogErrorGraphics("Failed to create normal srv for camera [%u / %i].", device_tag, id);
//			return false;
//		}
//
//		if (!data.position_srv->init(*device, data.position.get())) {
//			LogErrorGraphics("Failed to create position srv for camera [%u / %i].", device_tag, id);
//			return false;
//		}
//
//		if (!data.depth_srv->init(*device, data.depth.get())) {
//			LogErrorGraphics("Failed to create depth srv for camera [%u / %i].", device_tag, id);
//			return false;
//		}
//
//		data.render_target->addTexture(*device, data.diffuse.get());
//		data.render_target->addTexture(*device, data.specular.get());
//		data.render_target->addTexture(*device, data.normal.get());
//		data.render_target->addTexture(*device, data.position.get());
//		data.render_target->addDepthStencilBuffer(*device, data.depth.get());
//
//		if (create_render_texture) {
//			data.final_image.reset(createTexture());
//
//			if (!data.final_image->init2D(*device, size.x, size.y, Gleam::ITexture::Format::RGBA_8_UNORM)) {
//				LogErrorGraphics("Failed to create final output texture for camera [%u / %i].", device_tag, id);
//				return false;
//			}
//
//			data.final_srv.reset(createShaderResourceView());
//
//			if (!data.final_srv->init(*device, data.final_image.get())) {
//				LogErrorGraphics("Failed to create final output srv for camera [%u / %i].", device_tag, id);
//				return false;
//			}
//
//			data.final_render_target.reset(createRenderTarget());
//
//			if (!data.final_render_target->addTexture(*device, data.final_image.get())) {
//				LogErrorGraphics("Failed to create final output render target for camera [%u / %i].", device_tag, id);
//				return false;
//			}
//		}
//
//		it->second.emplace(device, std::move(data));
//	}
//
//	return true;
//}

//const RenderManager::GBufferData* RenderManager::getGBuffer(ECSEntityID id, const Gleam::IRenderDevice& device) const
//{
//	const auto rd_it = _g_buffers.find(id);
//
//	if (rd_it != _g_buffers.end()) {
//		const auto gb_it = rd_it->second.find(&device);
//		return (gb_it != rd_it->second.end()) ? &gb_it->second : nullptr;
//	}
//
//	return nullptr;
//}
//
//bool RenderManager::removeGBuffer(ECSEntityID id)
//{
//	return _g_buffers.erase(id) > 0;
//}
//
//bool RenderManager::hasGBuffer(ECSEntityID id, const Gleam::IRenderDevice& device) const
//{
//	const auto it = _g_buffers.find(id);
//
//	if (it != _g_buffers.end()) {
//		return it->second.find(&device) != it->second.end();
//	}
//
//	return false;
//}
//
//bool RenderManager::hasGBuffer(ECSEntityID id) const
//{
//	return _g_buffers.find(id) != _g_buffers.end();
//}

const RenderCommandList& RenderManager::getRenderCommands(const Gleam::RenderDevice& device, RenderOrder order, int32_t cache_index) const
{
	return const_cast<RenderManager*>(this)->getRenderCommands(device, order, cache_index);
}

RenderCommandList& RenderManager::getRenderCommands(const Gleam::RenderDevice& device, RenderOrder order, int32_t cache_index)
{
	GAFF_ASSERT(order != RenderOrder::Count);
	GAFF_ASSERT(Gaff::ValidIndex(cache_index, 1));

	const auto it = _cached_render_commands[static_cast<int32_t>(order)].command_lists[cache_index].find(&device);
	GAFF_ASSERT(it != _cached_render_commands[static_cast<int32_t>(order)].command_lists[cache_index].end());

	return it->second;
}

void RenderManager::presentAllOutputs(void)
{
	for (auto& entry: _outputs) {
		if (entry.second.output) {
			entry.second.output->present();
		}
	}

	_pending_window_removes.clear();
}

const Gleam::RenderDevice* RenderManager::getDeferredDevice(const Gleam::RenderDevice& device, EA::Thread::ThreadId thread_id) const
{
	return const_cast<RenderManager*>(this)->getDeferredDevice(device, thread_id);
}

Gleam::RenderDevice* RenderManager::getDeferredDevice(const Gleam::RenderDevice& device, EA::Thread::ThreadId thread_id)
{
	const auto device_it = _deferred_devices.find(&device);
	GAFF_ASSERT(device_it != _deferred_devices.end());

	const auto thread_it = device_it->second.find(thread_id);
	GAFF_ASSERT(thread_it != device_it->second.end());

	return thread_it->second.get();
}

void RenderManager::initializeRenderCommandData(RenderCommandData& render_commands) const
{
	GAFF_ASSERT(render_commands.command_lists[0].empty());

	for (auto& command_map : render_commands.command_lists) {
		command_map.reserve(_deferred_devices.size());

		for (const auto& entry : _deferred_devices) {
			command_map.insert(entry.first);
		}
	}
}

int32_t RenderManager::getRenderCacheIndex(void) const
{
	return _render_cache_index;
}

const RenderPipeline& RenderManager::getRenderPipeline(void) const
{
	return _render_pipeline;
}

RenderPipeline& RenderManager::getRenderPipeline(void)
{
	return _render_pipeline;
}

Gleam::RenderDevice* RenderManager::createRenderDevice(const Gleam::Window& window)
{
	Gleam::RenderDevice* const rd = SHIB_ALLOCT(Gleam::RenderDevice, g_allocator);

	if (!rd->init(window)) {
		LogErrorGraphics("Failed to create render device.");
		SHIB_FREET(rd, GetAllocator());

		return nullptr;
	}

	manageRenderDevice(*rd);
	return finishRenderDevice(rd);
}

Gleam::RenderDevice* RenderManager::createRenderDevice(int32_t adapter_id)
{
	Gleam::RenderDevice* const rd = SHIB_ALLOCT(Gleam::RenderDevice, g_allocator);

	if (!rd->init(adapter_id)) {
		LogErrorGraphics("Failed to create render device.");
		SHIB_FREET(rd, GetAllocator());

		return nullptr;
	}

	manageRenderDevice(*rd);
	return finishRenderDevice(rd);
}

Gleam::RenderDevice* RenderManager::finishRenderDevice(Gleam::RenderDevice* rd)
{
	Gleam::SamplerState* const sampler_state = SHIB_ALLOCT(Gleam::SamplerState, g_allocator);

	const Gleam::ISamplerState::Settings sampler_settings =
	{
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

void RenderManager::handleWindowClosed(Gleam::Window& window)
{
	removeWindow(window);

	if (!getNumWindows()) {
		GetApp().quit();
	}
}

NS_END
