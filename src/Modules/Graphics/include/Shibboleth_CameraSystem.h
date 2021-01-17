/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Shibboleth_MaterialResource.h"
#include <Shibboleth_Reflection.h>
#include <Shibboleth_ECSQuery.h>
#include <Shibboleth_ISystem.h>
#include <Shibboleth_JobPool.h>
#include <Gleam_ICommandList.h>
#include <Gleam_IRasterState.h>
#include <Gleam_IProgram.h>

namespace Gleam
{
	class IRenderDevice;
	class ICommandList;
}

NS_SHIBBOLETH

class RenderManagerBase;
class ECSManager;

class CameraPreRenderSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(uintptr_t thread_id_int) override;

private:
	Vector<ECSQueryResult> _camera{ ProxyAllocator("Graphics") };
	RenderManagerBase* _render_mgr = nullptr;
	ECSManager* _ecs_mgr = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(CameraPreRenderSystem);
};

class CameraPostRenderSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(uintptr_t thread_id_int) override;

private:
	// $TODO: Camera render order dependencies.

	struct CameraRenderData final
	{
		UniquePtr<Gleam::IProgramBuffers> program_buffers;
		UniquePtr<Gleam::IRasterState> raster_state;
		UniquePtr<Gleam::ISamplerState> sampler;
		const Gleam::IRenderDevice* device = nullptr;
		Gleam::ICommandList* cmd_list = nullptr;
		CameraPostRenderSystem* system = nullptr;
	};

	Vector<CameraRenderData> _camera_job_data_cache{ ProxyAllocator("Graphics") };
	Vector<Gaff::JobData> _job_data_cache{ ProxyAllocator("Graphics") };
	Gaff::Counter _job_counter = 0;
	int32_t _cache_index = 0;

	MaterialResourcePtr _camera_material;
	UniquePtr<Gleam::ICommandList> _cmd_lists[2];

	Vector<ECSQueryResult> _camera{ ProxyAllocator("Graphics") };
	RenderManagerBase* _render_mgr = nullptr;
	ECSManager* _ecs_mgr = nullptr;

	static void RenderCameras(uintptr_t id_int, void* data);

	SHIB_REFLECTION_CLASS_DECLARE(CameraPostRenderSystem);
};

NS_END

SHIB_REFLECTION_DECLARE(CameraPreRenderSystem)
SHIB_REFLECTION_DECLARE(CameraPostRenderSystem)
