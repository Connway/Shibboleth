/************************************************************************************
Copyright (C) by Nicholas LaCroix

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
#include <Gleam_ShaderResourceView.h>
#include <Gleam_Transform.h>
#include <Gleam_Texture.h>
#include <Gleam_Vec2.h>
#include <Gaff_IncludeEASTLArray.h>
#include <eathread/eathread_spinlock.h>
#include <eathread/eathread.h>

NS_GLEAM
	class RenderOutput;
	class RenderDevice;
	class Window;
NS_END


NS_SHIBBOLETH

class RenderManager final : public IManager
{
public:
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

	void presentAllOutputs(void);

	const Gleam::RenderDevice* getDeferredDevice(const Gleam::RenderDevice& device, EA::Thread::ThreadId thread_id) const;
	Gleam::RenderDevice* getDeferredDevice(const Gleam::RenderDevice& device, EA::Thread::ThreadId thread_id);

	const RenderPipeline& getRenderPipeline(void) const;
	RenderPipeline& getRenderPipeline(void);


private:
	// $TODO: Do we need all these allocated pointers to Gleam data structures? We're not using the interface versions of these,
	// so the implementation is known at compile time. Most of these data structures are just holding onto RefPtrs and other small
	// amounts of data. Can probably just in-place construct these instead of allocating UniquePtrs.

	struct RenderOutput final
	{
		UniquePtr<Gleam::RenderOutput> output;
		UniquePtr<Gleam::Window> window;
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
	VectorMap< const Gleam::RenderDevice*, UniquePtr<Gleam::SamplerState> > _to_screen_samplers{ GRAPHICS_ALLOCATOR };

	ResourcePtr<SamplerStateResource> _default_sampler;

	Gleam::RenderDevice* createRenderDevice(const Gleam::Window& window);
	Gleam::RenderDevice* createRenderDevice(int32_t adapter_id);
	Gleam::RenderDevice* finishRenderDevice(Gleam::RenderDevice* rd);

	void handleWindowClosed(Gleam::Window& window);

	SHIB_REFLECTION_CLASS_DECLARE(RenderManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::RenderManager);

#ifdef SHIB_REFL_IMPL
	SHIB_REFLECTION_BUILD_BEGIN(Shibboleth::RenderManager)
		.template base<Shibboleth::IManager>()
		.template ctor<>()
	SHIB_REFLECTION_BUILD_END(Shibboleth::RenderManager)
#endif
