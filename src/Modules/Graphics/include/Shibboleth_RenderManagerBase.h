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

#pragma once

#include "Shibboleth_IRenderManager.h"
#include "Shibboleth_SamplerStateResource.h"
#include <Shibboleth_ECSEntity.h>
#include <Shibboleth_SmartPtrs.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_Vector.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IRenderOutput.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_IRenderTarget.h>
#include <Gleam_ISamplerState.h>
#include <Gleam_ICommandList.h>
#include <Gleam_Transform.h>
#include <Gleam_IProgram.h>
#include <Gleam_ITexture.h>
#include <Gleam_IWindow.h>
#include <Gaff_Hash.h>
#include <EAThread/eathread_spinlock.h>
#include <EAThread/eathread.h>

NS_SHIBBOLETH

class RenderManagerBase : public IRenderManager
{
public:
	using OutputPtr = UniquePtr<Gleam::IRenderOutput>;
	using WindowPtr = UniquePtr<Gleam::IWindow>;
	using WindowOutputPair = eastl::pair<WindowPtr, OutputPtr>;

	using ProgramBuffersPtr = UniquePtr<Gleam::IProgramBuffers>;
	using SamplerPtr = UniquePtr<Gleam::ISamplerState>;
	using SRVPtr = UniquePtr<Gleam::IShaderResourceView>;
	using RTVPtr = UniquePtr<Gleam::IRenderTarget>;
	using TexturePtr = UniquePtr<Gleam::ITexture>;

	using RenderDevicePtr = UniquePtr<Gleam::IRenderDevice>;

	struct GBufferData final
	{
		RTVPtr render_target;

		TexturePtr diffuse;
		TexturePtr specular;
		TexturePtr normal;
		TexturePtr position;
		TexturePtr depth;

		SRVPtr diffuse_srv;
		SRVPtr specular_srv;
		SRVPtr normal_srv;
		SRVPtr position_srv;
		SRVPtr depth_srv;

		// These fields are only filled out if doing off-screen rendering.
		RTVPtr final_render_target;
		TexturePtr final_image;
		SRVPtr final_srv;
	};

	struct RenderCommand final
	{
		UniquePtr<Gleam::ICommandList> cmd_list;
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

	RenderManagerBase(void);
	~RenderManagerBase(void);

	bool initAllModulesLoaded(void) override;
	bool init(void) override;

	virtual Gleam::IRenderDevice* createRenderDevice(void) const = 0;

	Gleam::IKeyboard* createKeyboard(void) const override;
	Gleam::IMouse* createMouse(void) const override;

	void addRenderDeviceTag(Gleam::IRenderDevice* device, const char* tag);
	void manageRenderDevice(Gleam::IRenderDevice* device);

	const Vector<Gleam::IRenderDevice*>* getDevicesByTag(Gaff::Hash32 tag) const;
	const Vector<Gleam::IRenderDevice*>* getDevicesByTag(const char8_t* tag) const;
	const Vector<Gleam::IRenderDevice*>* getDevicesByTag(const char* tag) const;
	Gleam::IRenderDevice& getDevice(int32_t index) const;
	int32_t getNumDevices(void) const;

	Gleam::IRenderOutput* getOutput(const char* tag) const
	{
		return getOutput(Gaff::FNV1aHash32String(tag));
	}

	Gleam::IWindow* getWindow(const char* tag) const
	{
		return getWindow(Gaff::FNV1aHash32String(tag));
	}

	Gleam::IRenderOutput* getOutput(Gaff::Hash32 tag) const;
	Gleam::IWindow* getWindow(Gaff::Hash32 tag) const;
	void removeWindow(const Gleam::IWindow& window);
	int32_t getNumWindows(void) const;

	const SamplerStateResourcePtr& getDefaultSamplerState(void) const;
	SamplerStateResourcePtr& getDefaultSamplerState(void);

	bool createGBuffer(EntityID id, Gaff::Hash32 device_tag, const Gleam::IVec2& size, bool create_render_texture = false);
	const GBufferData* getGBuffer(EntityID id, const Gleam::IRenderDevice& device) const;
	bool removeGBuffer(EntityID id);
	bool hasGBuffer(EntityID id, const Gleam::IRenderDevice& device) const;
	bool hasGBuffer(EntityID id) const;

	const RenderCommandList& getRenderCommands(const Gleam::IRenderDevice& device, RenderOrder order, int32_t cache_index) const;
	RenderCommandList& getRenderCommands(const Gleam::IRenderDevice& device, RenderOrder order, int32_t cache_index);

	void presentAllOutputs(void);

	const Gleam::IRenderDevice* getDeferredDevice(const Gleam::IRenderDevice& device, EA::Thread::ThreadId thread_id) const;
	Gleam::IRenderDevice* getDeferredDevice(const Gleam::IRenderDevice& device, EA::Thread::ThreadId thread_id);

private:
	VectorMap<const Gleam::IRenderDevice*, SamplerPtr> _to_screen_samplers{ ProxyAllocator("Graphics") };
	VectorMap<EntityID, VectorMap<const Gleam::IRenderDevice*, GBufferData> > _g_buffers{ ProxyAllocator("Graphics") };

	VectorMap< Gaff::Hash32, Vector<Gleam::IRenderDevice*> > _render_device_tags{ ProxyAllocator("Graphics") };
	Vector<RenderDevicePtr> _render_devices{ ProxyAllocator("Graphics") };
	VectorMap<Gaff::Hash32, WindowOutputPair> _window_outputs{ ProxyAllocator("Graphics") };

	VectorMap<
		const Gleam::IRenderDevice*,
		VectorMap< EA::Thread::ThreadId, UniquePtr<Gleam::IRenderDevice> >
	> _deferred_contexts{ ProxyAllocator("Graphics") };

	VectorMap<const Gleam::IRenderDevice*, RenderCommandList> _cached_render_commands[static_cast<size_t>(RenderOrder::Count)][2] = {
		{
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
		},
		{
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
		},
		{
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
		},
		{
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
		},
		{
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
		},
		{
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") },
			VectorMap<const Gleam::IRenderDevice*, RenderCommandList>{ ProxyAllocator("Graphics") }
		}
	};

	SamplerStateResourcePtr _default_sampler;

	Gleam::IRenderDevice* createRenderDevice(int32_t adapter_id);
};

NS_END
