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

#pragma once

#include <Shibboleth_IManager.h>
#include "Shibboleth_SamplerStateResource.h"
#include <Shibboleth_ResourcePtr.h>
#include <Shibboleth_SmartPtrs.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_Vector.h>
#include <eathread/eathread_spinlock.h>
#include <eathread/eathread.h>
#include <EASTL/array.h>

NS_GLEAM
	class ShaderResourceView;
	class ProgramBuffers;
	class RenderOutput;
	class RenderTarget;
	class RenderDevice;
	class SamplerState;
	class CommandList;
	class Texture;
	class Window;
NS_END

NS_SHIBBOLETH

class RenderManager : public IManager
{
public:
	static constexpr int32_t CacheIndexCount = 2;

	using RenderOutputPtr = UniquePtr<Gleam::RenderOutput>;
	using WindowPtr = UniquePtr<Gleam::Window>;

	using ShaderResourceViewPtr = UniquePtr<Gleam::ShaderResourceView>;
	using ProgramBuffersPtr = UniquePtr<Gleam::ProgramBuffers>;
	using SamplerStatePtr = UniquePtr<Gleam::SamplerState>;
	using RenderTargetPtr = UniquePtr<Gleam::RenderTarget>;
	using TexturePtr = UniquePtr<Gleam::Texture>;

	using RenderDevicePtr = UniquePtr<Gleam::RenderDevice>;

	struct GBuffer final
	{
		RenderTargetPtr render_target;

		TexturePtr diffuse;
		TexturePtr specular;
		TexturePtr normal;
		TexturePtr position;
		TexturePtr depth;

		ShaderResourceViewPtr diffuse_srv;
		ShaderResourceViewPtr specular_srv;
		ShaderResourceViewPtr normal_srv;
		ShaderResourceViewPtr position_srv;
		ShaderResourceViewPtr depth_srv;

		// These fields are only filled out if doing off-screen rendering.
		RenderTargetPtr final_render_target;
		TexturePtr final_image;
		ShaderResourceViewPtr final_srv;
	};

	struct RenderCommand final
	{
		UniquePtr<Gleam::CommandList> cmd_list;
		bool owns_command = true;
		//Gleam::IRenderTarget* target = nullptr;
	};

	struct RenderCommandList final
	{
		Vector<RenderCommand> command_list{ ProxyAllocator("Graphics") };
		EA::Thread::SpinLock lock;
	};

	enum class RenderOrder
	{
		ClearRenderTargets,
		InWorldWithDepthTest,
		InWorldNoDepthTest,
		ScreenSpace,
		ToRenderTarget,
		ScreenSpaceDirect, // Renders directly to screen.

		Count
	};


	~RenderManager(void);

	bool initAllModulesLoaded(void) override;
	bool init(void) override;

	void manageRenderDevice(Gleam::RenderDevice& device);

	// Are these functions necessary?
	const Gleam::RenderDevice* getDevice(const Gleam::RenderOutput& output) const;
	const Gleam::RenderDevice& getDevice(int32_t index) const;

	Gleam::RenderDevice* getDevice(const Gleam::RenderOutput& output);
	Gleam::RenderDevice& getDevice(int32_t index);

	const Vector<RenderDevicePtr>& getDevices(void) const;
	int32_t getNumDevices(void) const;


	const Gleam::RenderOutput* getOutput(const char* tag) const
	{
		return getOutput(Gaff::FNV1aHash32String(tag));
	}

	const Gleam::Window* getWindow(const char* tag) const
	{
		return getWindow(Gaff::FNV1aHash32String(tag));
	}

	Gleam::RenderOutput* getOutput(const char* tag)
	{
		return getOutput(Gaff::FNV1aHash32String(tag));
	}

	Gleam::Window* getWindow(const char* tag)
	{
		return getWindow(Gaff::FNV1aHash32String(tag));
	}

	const Gleam::RenderOutput* getOutput(Gaff::Hash32 tag) const;
	const Gleam::RenderOutput* getOutput(int32_t index) const;
	Gleam::RenderOutput* getOutput(Gaff::Hash32 tag);
	Gleam::RenderOutput* getOutput(int32_t index);

	const Gleam::Window* getWindow(Gaff::Hash32 tag) const;
	const Gleam::Window* getWindow(int32_t index) const;
	Gleam::Window* getWindow(Gaff::Hash32 tag);
	Gleam::Window* getWindow(int32_t index);

	void removeWindow(const Gleam::Window& window);
	int32_t getNumWindows(void) const;


	const ResourcePtr<SamplerStateResource>& getDefaultSamplerState(void) const;
	ResourcePtr<SamplerStateResource>& getDefaultSamplerState(void);

	GBuffer* createGBuffer(Gleam::RenderTarget& output);

	//bool createGBuffer(ECSEntityID id, Gaff::Hash32 device_tag, const Gleam::IVec2& size, bool create_render_texture = false);
	//const GBufferData* getGBuffer(ECSEntityID id, const Gleam::IRenderDevice& device) const;
	//bool removeGBuffer(ECSEntityID id);
	//bool hasGBuffer(ECSEntityID id, const Gleam::IRenderDevice& device) const;
	//bool hasGBuffer(ECSEntityID id) const;

	const RenderCommandList& getRenderCommands(const Gleam::RenderDevice& device, RenderOrder order, int32_t cache_index) const;
	RenderCommandList& getRenderCommands(const Gleam::RenderDevice& device, RenderOrder order, int32_t cache_index);

	void presentAllOutputs(void);

	const Gleam::RenderDevice* getDeferredDevice(const Gleam::RenderDevice& device, EA::Thread::ThreadId thread_id) const;
	Gleam::RenderDevice* getDeferredDevice(const Gleam::RenderDevice& device, EA::Thread::ThreadId thread_id);

private:
	struct RenderOutput final
	{
		GBuffer g_buffer;
		RenderOutputPtr output;
		WindowPtr window;
		Gleam::RenderDevice* render_device = nullptr;
	};

	struct RenderCommandData final
	{
		using DeviceCommandListMap = VectorMap<const Gleam::RenderDevice*, RenderCommandList>;

		DeviceCommandListMap command_lists[CacheIndexCount] =
		{
			DeviceCommandListMap{ ProxyAllocator("Graphics") },
			DeviceCommandListMap{ ProxyAllocator("Graphics") },
		};
	};

	VectorMap<const Gleam::RenderDevice*, SamplerStatePtr> _to_screen_samplers{ ProxyAllocator("Graphics") };
	VectorMap<const Gleam::RenderTarget*, GBuffer> _g_buffers{ ProxyAllocator{"Graphics"} };

	VectorMap< Gaff::Hash32, Vector<Gleam::RenderDevice*> > _render_device_tags{ ProxyAllocator("Graphics") };
	Vector<RenderDevicePtr> _render_devices{ ProxyAllocator("Graphics") };
	VectorMap<Gaff::Hash32, RenderOutput> _outputs{ ProxyAllocator("Graphics") };
	Vector<RenderOutput> _pending_window_removes{ ProxyAllocator("Graphics") };

	VectorMap<
		const Gleam::RenderDevice*,
		VectorMap< EA::Thread::ThreadId, UniquePtr<Gleam::RenderDevice> >
	> _deferred_contexts{ ProxyAllocator("Graphics") };

	RenderCommandData _cached_render_commands[static_cast<size_t>(RenderOrder::Count)];

	ResourcePtr<SamplerStateResource> _default_sampler;

	Gleam::RenderDevice* createRenderDevice(const char* adapter_name);
	Gleam::RenderDevice* createRenderDevice(int32_t adapter_id);
	Gleam::RenderDevice* finishRenderDevice(Gleam::RenderDevice* rd);

	void handleWindowClosed(Gleam::Window& window);
};

NS_END
