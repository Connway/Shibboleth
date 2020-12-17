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

#include "Shibboleth_ClearRenderTargetSystem.h"
#include "Shibboleth_RenderManagerBase.h"
#include "Shibboleth_CameraComponent.h"
#include <Shibboleth_ECSManager.h>

SHIB_REFLECTION_DEFINE_BEGIN(ClearRenderTargetSystem)
	.BASE(ISystem)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(ClearRenderTargetSystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ClearRenderTargetSystem)

bool ClearRenderTargetSystem::init(void)
{
	ECSQuery camera_query;
	camera_query.add<Camera>(_camera);

	_render_mgr = &GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	_ecs_mgr = &GetApp().getManagerTFast<ECSManager>();

	_ecs_mgr->registerQuery(std::move(camera_query));

	_cmd_lists[0].reset(_render_mgr->createCommandList());
	_cmd_lists[1].reset(_render_mgr->createCommandList());

	if (!_cmd_lists[0] || !_cmd_lists[1]) {
		// $TODO: Log error.
		return false;
	}

	return true;
}

void ClearRenderTargetSystem::update(uintptr_t thread_id_int)
{
	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	const int32_t num_cameras = static_cast<int32_t>(_camera.size());

	for (int32_t camera_index = 0; camera_index < num_cameras; ++camera_index) {
		_ecs_mgr->iterate<Camera>(
			_camera[camera_index],
			[&](EntityID id, const Camera& camera) -> void
			{
				const auto* const devices = _render_mgr->getDevicesByTag(camera.device_tag);

				if (!devices) {
					return;
				}

				// $TODO: In the future, when more expensive clear options become available,
				// may want to split this out into a per-device job.
				for (const Gleam::IRenderDevice* device : *devices) {
					const auto* const g_buffer = _render_mgr->getGBuffer(id, *device);

					if (!g_buffer) {
						// $TODO: Log error.
						return;
					}

					Gleam::IRenderDevice* const deferred_device = _render_mgr->getDeferredDevice(*device, thread_id);
					Gleam::IRenderTarget* const render_target = g_buffer->render_target.get();
					Gleam::ICommandList* const cmd_list = _cmd_lists[_cache_index].get();

					// $TODO: Make the clearing type an option.
					render_target->bind(*deferred_device);
					render_target->clear(*deferred_device, Gleam::IRenderTarget::CLEAR_ALL, 1.0f, 0, Gleam::COLOR_BLACK);

					if (!deferred_device->finishCommandList(*cmd_list)) {
						// $TODO: Log error periodic.
						SHIB_FREET(cmd_list, GetAllocator());
						return;
					}

					auto& render_cmds = _render_mgr->getRenderCommands(
						*device,
						RenderManagerBase::RenderOrder::ClearRenderTargets,
						_cache_index
					);

					render_cmds.lock.Lock();
					auto& cmd = render_cmds.command_list.emplace_back();
					cmd.cmd_list.reset(cmd_list);
					cmd.owns_command = false;
					render_cmds.lock.Unlock();
				}
			}
		);
	}
}

NS_END
