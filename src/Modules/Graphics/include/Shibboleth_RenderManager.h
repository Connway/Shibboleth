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

#include "Resources/Shibboleth_SamplerStateResource.h"
#include "Pipelines/Shibboleth_RenderPipeline.h"
#include "Model/Shibboleth_ModelInstanceData.h"
#include <Containers/Shibboleth_SparseStack.h>
#include <Containers/Shibboleth_VectorMap.h>
#include <Containers/Shibboleth_Vector.h>
#include <Ptrs/Shibboleth_SmartPtrs.h>
#include <Shibboleth_ResourcePtr.h>
#include <Shibboleth_IManager.h>
#include <Gleam_CommandList.h>
#include <Gleam_Transform.h>
#include <Gleam_Vec2.h>
#include <eathread/eathread_spinlock.h>
#include <eathread/eathread.h>
#include <EASTL/array.h>

NS_GLEAM
	class RenderOutput;
	class RenderDevice;
	class Texture;
	class Window;
NS_END


NS_SHIBBOLETH

struct RenderCommands final
{
	UniquePtr<Gleam::CommandList> commands;
	//Gleam::RenderTarget* target = nullptr;
};

struct RenderCommandList final
{
	Vector<RenderCommands> command_list{ GRAPHICS_ALLOCATOR };

	// $TODO: Evaluate if we need this once converted over to render pipeline.
	//EA::Thread::SpinLock lock;
};

struct RenderCommandData final
{
	using DeviceCommandListMap = VectorMap<const Gleam::RenderDevice*, RenderCommandList>;
	static constexpr int32_t CacheIndexCount = 2;

	DeviceCommandListMap command_lists[CacheIndexCount] =
	{
		DeviceCommandListMap{ GRAPHICS_ALLOCATOR },
		DeviceCommandListMap{ GRAPHICS_ALLOCATOR },
	};

	const RenderCommandList& getCommandList(const Gleam::RenderDevice& device, int32_t cache_index) const
	{
		return const_cast<RenderCommandData*>(this)->getCommandList(device, cache_index);
	}

	RenderCommandList& getCommandList(const Gleam::RenderDevice& device, int32_t cache_index)
	{
		return command_lists[cache_index][&device];
	}
};


class RenderManager final : public IManager
{
public:
	using RenderOutputPtr = UniquePtr<Gleam::RenderOutput>;
	using WindowPtr = UniquePtr<Gleam::Window>;

	//using ShaderResourceViewPtr = UniquePtr<Gleam::ShaderResourceView>;
	//using ProgramBuffersPtr = UniquePtr<Gleam::ProgramBuffers>;
	using SamplerStatePtr = UniquePtr<Gleam::SamplerState>;
	//using TexturePtr = UniquePtr<Gleam::Texture>;

	using RenderDevicePtr = UniquePtr<Gleam::RenderDevice>;


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

	bool init(void) override;

	void manageRenderDevice(Gleam::RenderDevice& device);

	// Are these functions necessary?
	const Gleam::RenderDevice* getDevice(const Gleam::RenderOutput& output) const;
	const Gleam::RenderDevice& getDevice(int32_t index) const;

	Gleam::RenderDevice* getDevice(const Gleam::RenderOutput& output);
	Gleam::RenderDevice& getDevice(int32_t index);

	const Vector<RenderDevicePtr>& getDevices(void) const;
	int32_t getNumDevices(void) const;

	const Vector<Gleam::RenderDevice*>* getDevicesByTag(Gaff::Hash32 tag) const;
	const Vector<Gleam::RenderDevice*>* getDevicesByTag(const char8_t* tag) const;
	const Vector<Gleam::RenderDevice*>* getDevicesByTag(const char* tag) const;

	const Gleam::RenderOutput* getOutput(Gaff::Hash32 tag) const;
	const Gleam::RenderOutput* getOutput(const char8_t* tag) const;
	const Gleam::RenderOutput* getOutput(const char* tag) const;
	const Gleam::RenderOutput* getOutput(int32_t index) const;
	Gleam::RenderOutput* getOutput(Gaff::Hash32 tag);
	Gleam::RenderOutput* getOutput(const char8_t* tag);
	Gleam::RenderOutput* getOutput(const char* tag);
	Gleam::RenderOutput* getOutput(int32_t index);

	const Gleam::Window* getWindow(Gaff::Hash32 tag) const;
	const Gleam::Window* getWindow(const char8_t* tag) const;
	const Gleam::Window* getWindow(const char* tag) const;
	const Gleam::Window* getWindow(int32_t index) const;
	Gleam::Window* getWindow(Gaff::Hash32 tag);
	Gleam::Window* getWindow(const char8_t* tag);
	Gleam::Window* getWindow(const char* tag);
	Gleam::Window* getWindow(int32_t index);

	void removeWindow(const Gleam::Window& window);
	int32_t getNumWindows(void) const;


	const ResourcePtr<SamplerStateResource>& getDefaultSamplerState(void) const;
	ResourcePtr<SamplerStateResource>& getDefaultSamplerState(void);

	//GBuffer* createGBuffer(Gleam::RenderTarget& output);

	//bool createGBuffer(ECSEntityID id, Gaff::Hash32 device_tag, const Gleam::IVec2& size, bool create_render_texture = false);
	//const GBufferData* getGBuffer(ECSEntityID id, const Gleam::IRenderDevice& device) const;
	//bool removeGBuffer(ECSEntityID id);
	//bool hasGBuffer(ECSEntityID id, const Gleam::IRenderDevice& device) const;
	//bool hasGBuffer(ECSEntityID id) const;

	// $TODO: More configurable pipelines instead of this hardcoded pipeline.
	const RenderCommandList& getRenderCommands(const Gleam::RenderDevice& device, RenderOrder order, int32_t cache_index) const;
	RenderCommandList& getRenderCommands(const Gleam::RenderDevice& device, RenderOrder order, int32_t cache_index);

	void presentAllOutputs(void);

	const Gleam::RenderDevice* getDeferredDevice(const Gleam::RenderDevice& device, EA::Thread::ThreadId thread_id) const;
	Gleam::RenderDevice* getDeferredDevice(const Gleam::RenderDevice& device, EA::Thread::ThreadId thread_id);

	void initializeRenderCommandData(RenderCommandData& render_commands) const;

	int32_t getRenderCacheIndex(void) const;

	const RenderPipeline& getRenderPipeline(void) const;
	RenderPipeline& getRenderPipeline(void);


private:
	struct RenderOutput final
	{
		RenderOutputPtr output;
		WindowPtr window;
		Gleam::RenderDevice* render_device = nullptr;
	};

	RenderPipeline _render_pipeline;

	VectorMap< Gaff::Hash32, Vector<Gleam::RenderDevice*> > _render_device_tags{ GRAPHICS_ALLOCATOR };
	Vector<RenderDevicePtr> _render_devices{ GRAPHICS_ALLOCATOR };
	VectorMap<
		const Gleam::RenderDevice*,
		VectorMap< EA::Thread::ThreadId, UniquePtr<Gleam::RenderDevice> >
	> _deferred_devices{ GRAPHICS_ALLOCATOR };

	VectorMap<Gaff::Hash32, RenderOutput> _outputs{ GRAPHICS_ALLOCATOR };
	Vector<RenderOutput> _pending_window_removes{ GRAPHICS_ALLOCATOR };

	// $TODO: Move a lot of these into RenderPipeline.
	VectorMap<const Gleam::RenderDevice*, SamplerStatePtr> _to_screen_samplers{ GRAPHICS_ALLOCATOR };

	RenderCommandData _cached_render_commands[static_cast<size_t>(RenderOrder::Count)];

	ResourcePtr<SamplerStateResource> _default_sampler;

	int32_t _render_cache_index = 0;

	Gleam::RenderDevice* createRenderDevice(const Gleam::Window& window);
	Gleam::RenderDevice* createRenderDevice(int32_t adapter_id);
	Gleam::RenderDevice* finishRenderDevice(Gleam::RenderDevice* rd);

	void handleWindowClosed(Gleam::Window& window);

	SHIB_REFLECTION_CLASS_DECLARE(RenderManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::RenderManager);
