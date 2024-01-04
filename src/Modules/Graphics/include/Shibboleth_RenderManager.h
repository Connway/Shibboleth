/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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
	class Texture;
	class Window;
NS_END

NS_SHIBBOLETH

class RenderManager : public IManager
{
public:
	static constexpr int32_t CacheIndexCount = 2;

	using OutputPtr = UniquePtr<Gleam::RenderOutput>;
	using WindowPtr = UniquePtr<Gleam::Window>;
	using WindowOutputPair = eastl::pair<WindowPtr, OutputPtr>;

	using ProgramBuffersPtr = UniquePtr<Gleam::ProgramBuffers>;
	using SamplerStatePtr = UniquePtr<Gleam::SamplerState>;
	using SRVPtr = UniquePtr<Gleam::ShaderResourceView>;
	using RTVPtr = UniquePtr<Gleam::RenderTarget>;
	using TexturePtr = UniquePtr<Gleam::Texture>;

	using RenderDevicePtr = UniquePtr<Gleam::RenderDevice>;

	//struct GBufferData final
	//{
	//	RTVPtr render_target;

	//	TexturePtr diffuse;
	//	TexturePtr specular;
	//	TexturePtr normal;
	//	TexturePtr position;
	//	TexturePtr depth;

	//	SRVPtr diffuse_srv;
	//	SRVPtr specular_srv;
	//	SRVPtr normal_srv;
	//	SRVPtr position_srv;
	//	SRVPtr depth_srv;

	//	// These fields are only filled out if doing off-screen rendering.
	//	RTVPtr final_render_target;
	//	TexturePtr final_image;
	//	SRVPtr final_srv;
	//};

	//struct RenderCommand final
	//{
	//	UniquePtr<Gleam::ICommandList> cmd_list;
	//	bool owns_command = true;
	//	//Gleam::IRenderTarget* target = nullptr;
	//};

	//struct RenderCommandList final
	//{
	//	Vector<RenderCommand> command_list{ ProxyAllocator("Graphics") };
	//	EA::Thread::SpinLock lock;
	//};

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

	static Gleam::RendererType GetRendererType(void);


	RenderManager(void);
	~RenderManager(void);

	bool initAllModulesLoaded(void) override;
	bool init(void) override;

	void manageRenderDevice(Gleam::IRenderDevice* device);

	const Gleam::IRenderDevice* getDevice(const Gleam::RenderOutput& output) const;
	const Gleam::IRenderDevice& getDevice(int32_t index) const;

	Gleam::IRenderDevice* getDevice(const Gleam::RenderOutput& output);
	Gleam::IRenderDevice& getDevice(int32_t index);

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

	//bool createGBuffer(ECSEntityID id, Gaff::Hash32 device_tag, const Gleam::IVec2& size, bool create_render_texture = false);
	//const GBufferData* getGBuffer(ECSEntityID id, const Gleam::IRenderDevice& device) const;
	//bool removeGBuffer(ECSEntityID id);
	//bool hasGBuffer(ECSEntityID id, const Gleam::IRenderDevice& device) const;
	//bool hasGBuffer(ECSEntityID id) const;

	//const RenderCommandList& getRenderCommands(const Gleam::IRenderDevice& device, RenderOrder order, int32_t cache_index) const;
	//RenderCommandList& getRenderCommands(const Gleam::IRenderDevice& device, RenderOrder order, int32_t cache_index);

	void presentAllOutputs(void);

	const Gleam::RenderDevice* getDeferredDevice(const Gleam::RenderDevice& device, EA::Thread::ThreadId thread_id) const;
	Gleam::RenderDevice* getDeferredDevice(const Gleam::RenderDevice& device, EA::Thread::ThreadId thread_id);

private:
	//struct RenderCommandData final
	//{
	//	using CommandList
	//	RenderCommandList command_lists[CacheIndexCount];
	//};

	VectorMap<const Gleam::RenderDevice*, SamplerStatePtr> _to_screen_samplers{ ProxyAllocator("Graphics") };
	//VectorMap<ECSEntityID, VectorMap<const Gleam::RenderDevice*, GBufferData> > _g_buffers{ ProxyAllocator("Graphics") };

	//VectorMap< Gaff::Hash32, Vector<Gleam::RenderDevice*> > _render_device_tags{ ProxyAllocator("Graphics") };
	Vector<RenderDevicePtr> _render_devices{ ProxyAllocator("Graphics") };
	//VectorMap<Gaff::Hash32, WindowOutputPair> _window_outputs{ ProxyAllocator("Graphics") };
	//Vector<WindowOutputPair> _pending_window_removes{ ProxyAllocator("Graphics") };

	VectorMap<
		const Gleam::IRenderDevice*,
		VectorMap< EA::Thread::ThreadId, UniquePtr<Gleam::IRenderDevice> >
	> _deferred_contexts{ ProxyAllocator("Graphics") };

	//VectorMap<const Gleam::IRenderDevice*, RenderCommandList> _cached_render_commands[static_cast<size_t>(RenderOrder::Count)][CacheIndexCount] = {
	//	{
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
	//	},
	//	{
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
	//	},
	//	{
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
	//	},
	//	{
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
	//	},
	//	{
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
	//	},
	//	{
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
	//		VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
	//	}
	//};

	ResourcePtr<SamplerStateResource> _default_sampler;

	Gleam::RenderDevice* createRenderDeviceFromAdapter(int32_t adapter_id);
	void handleWindowClosed(Gleam::Window& window);
};

NS_END
