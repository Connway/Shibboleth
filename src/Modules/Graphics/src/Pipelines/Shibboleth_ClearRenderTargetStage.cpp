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

#include "Pipelines/Shibboleth_ClearRenderTargetStage.h"
#include "Resources/Shibboleth_RenderTargetResource.h"
#include <Ptrs/Shibboleth_ManagerRef.h>
#include <Shibboleth_ResourceManager.h>
#include <Gleam_RenderTarget.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_CommandList.h>

SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE(Shibboleth::ClearRenderTargetStage, Shibboleth::IRenderStage)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ClearRenderTargetStage)

bool ClearRenderTargetStage::init(RenderManager& render_mgr)
{
	ManagerRef<ResourceManager> res_mgr;
	_render_targets = &res_mgr->getResources(Refl::Reflection<RenderTargetResource>::GetReflectionDefinition());
	_resource_lock = &res_mgr->getResourceBucketLock<RenderTargetResource>();

	_render_mgr = &render_mgr;
	_render_mgr->initializeRenderCommandData(_render_commands);

	for (auto& command_map : _render_commands.command_lists) {
		for (auto& map_entry : command_map) {
			map_entry.second.command_list.resize(1);
			map_entry.second.command_list[0].commands.reset(SHIB_ALLOCT(Gleam::CommandList, GRAPHICS_ALLOCATOR));
		}
	}

	return _render_targets != nullptr;
}

//void ClearRenderTargetStage::destroy(RenderManager& /*render_mgr*/)
//{
//}

void ClearRenderTargetStage::update(uintptr_t thread_id_int)
{
	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

	// Ensure the render target bucket doesn't get changed from underneath us.
	const EA::Thread::AutoMutex lock(*_resource_lock);

	// All these IResources are RenderTargetResources. Safe to static_cast.
	for (IResource* const resource : *_render_targets) {
		RenderTargetResource* const rt_res = static_cast<RenderTargetResource*>(resource);

		if (!rt_res->canClear()) {
			continue;
		}

		Gleam::RenderTarget* const target = rt_res->getRenderTarget();
		Gleam::RenderDevice* const device = rt_res->getDevice();

		// $TODO: Should we log?
		if (!device || !target) {
			continue;
		}

		Gleam::RenderDevice* const deferred_device = _render_mgr->getDeferredDevice(*device, thread_id);
		const int32_t cache_index = _render_mgr->getRenderCacheIndex();

		Gleam::CommandList* const cmd_list = _render_commands.getCommandList(*deferred_device, cache_index).command_list[0].commands.get();

		target->bind(*deferred_device);
		target->clear(*deferred_device, rt_res->getClearSettings());

		if (!deferred_device->finishCommandList(*cmd_list)) {
			// $TODO: Log periodic error.
		}
	}
}

const RenderCommandData& ClearRenderTargetStage::getRenderCommands(void) const
{
	return _render_commands;
}

NS_END
