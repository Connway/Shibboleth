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
#include <Gleam_RenderDevice.h>
#include <Gleam_CommandList.h>

SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE(Shibboleth::ClearRenderTargetStage, Shibboleth::IRenderStage)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ClearRenderTargetStage)

bool ClearRenderTargetStage::init(RenderManager& render_mgr)
{
	_render_mgr = &render_mgr;

	_render_mgr->initializeRenderCommandData(_render_commands);

	for (auto& command_map : _render_commands.command_lists) {
		for (auto& map_entry : command_map) {
			map_entry.second.command_list.resize(1);
			map_entry.second.command_list[0].commands.reset(SHIB_ALLOCT(Gleam::CommandList, GRAPHICS_ALLOCATOR));
		}
	}

	return true;
}

//void ClearRenderTargetStage::destroy(RenderManager& /*render_mgr*/)
//{
//}

void ClearRenderTargetStage::update(uintptr_t thread_id_int)
{
	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);

	// iterate over all render targets.
	// get device for render target.

	Gleam::RenderDevice* const deferred_device = _render_mgr->getDeferredDevice(*device, thread_id);
	const int32_t cache_index = _render_mgr->getRenderCacheIndex();

	Gleam::CommandList* const cmd_list = _render_commands.getCommandList(*deferred_device, cache_index).command_list[0].commands.get();

	//render_target->bind(*deferred_device);
	//render_target->clear(*deferred_device, Gleam::IRenderTarget::ClearFlags::All, 1.0f, 0, Gleam::Color::Black);

	if (!deferred_device->finishCommandList(*cmd_list)) {
		// $TODO: Log periodic error.
	}

	//const int32_t num_cameras = static_cast<int32_t>(_camera.size());

	//for (int32_t camera_index = 0; camera_index < num_cameras; ++camera_index) {
	//	_ecs_mgr->iterate<Camera>(
	//		_camera[camera_index],
	//		[&](ECSEntityID id, const Camera& camera) -> void
	//		{
	//			const auto* const devices = _render_mgr->getDevicesByTag(camera.device_tag);

	//			if (!devices) {
	//				return;
	//			}

	//			// $TODO: In the future, when more expensive clear options become available,
	//			// may want to split this out into a per-device job.
	//			for (const Gleam::IRenderDevice* device : *devices) {
	//				const auto* const g_buffer = _render_mgr->getGBuffer(id, *device);

	//				if (!g_buffer) {
	//					// $TODO: Log error.
	//					return;
	//				}

	//				Gleam::IRenderDevice* const deferred_device = _render_mgr->getDeferredDevice(*device, thread_id);
	//				Gleam::IRenderTarget* const render_target = g_buffer->render_target.get();
	//				Gleam::ICommandList* const cmd_list = _cmd_lists[_cache_index].get();

	//				// $TODO: Make the clearing type an option.
	//				render_target->bind(*deferred_device);
	//				render_target->clear(*deferred_device, Gleam::IRenderTarget::ClearFlags::All, 1.0f, 0, Gleam::Color::Black);

	//				if (!deferred_device->finishCommandList(*cmd_list)) {
	//					// $TODO: Log error periodic.
	//					SHIB_FREET(cmd_list, GetAllocator());
	//					return;
	//				}

	//				auto& render_cmds = _render_mgr->getRenderCommands(
	//					*device,
	//					RenderManagerBase::RenderOrder::ClearRenderTargets,
	//					_cache_index
	//				);

	//				render_cmds.lock.Lock();
	//				auto& cmd = render_cmds.command_list.emplace_back();
	//				cmd.cmd_list.reset(cmd_list);
	//				cmd.owns_command = false;
	//				render_cmds.lock.Unlock();
	//			}
	//		}
	//	);
	//}
}

const RenderCommandData& ClearRenderTargetStage::getRenderCommands(void) const
{
	return _render_commands;
}

NS_END
