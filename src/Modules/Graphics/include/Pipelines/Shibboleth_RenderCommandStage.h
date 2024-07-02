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

#include "Shibboleth_IRenderPipelineStage.h"
#include "Shibboleth_RenderManager.h"
#include "Resources/Shibboleth_ProgramBuffersResource.h"
#include "Resources/Shibboleth_BufferResource.h"
#include <Ptrs/Shibboleth_ManagerRef.h>
#include <Shibboleth_JobPool.h>
#include <Gleam_ShaderResourceView.h>
#include <Gleam_IShader.h>


NS_GLEAM
	class RenderTarget;
NS_END

NS_SHIBBOLETH

struct ModelInstanceData;
class ResourceManager;

// $TODO: This might need to be split up into more discrete stages, such as a culling stage.
class RenderCommandStage final : public IRenderPipelineStage
{
public:
	bool init(RenderManager& render_mgr) override;
	//void destroy(RenderManager& /*render_mgr*/) override;

	void update(uintptr_t thread_id_int) override;

	const RenderCommandData& getRenderCommands(void) const override;

	SHIB_REFLECTION_CLASS_DECLARE(RenderCommandStage);

private:
	struct RenderJobData final
	{
		RenderCommandStage* rcs;
		int32_t index;

		Gleam::RenderDevice* device;
		Gleam::CommandList* cmd_list;
		Gleam::RenderTarget* target;

		Gleam::Mat4x4 view_projection;
	};

	struct DeviceJobData final
	{
		DeviceJobData(void) = default;
		DeviceJobData(DeviceJobData&& data)
		{
			rcs = data.rcs;

			render_job_data_cache = std::move(data.render_job_data_cache);
			job_data_cache = std::move(data.job_data_cache);
			device = data.device;
			job_counter = static_cast<int32_t>(data.job_counter);
		}

		RenderCommandStage* rcs;

		Vector<RenderJobData> render_job_data_cache{ GRAPHICS_ALLOCATOR };
		Vector<Gaff::JobData> job_data_cache{ GRAPHICS_ALLOCATOR };
		Gleam::RenderDevice* device;
		Gaff::Counter job_counter = 0;
	};

	RenderCommandData _render_commands;

	Vector<DeviceJobData> _device_job_data_cache{ GRAPHICS_ALLOCATOR };
	Vector<Gaff::JobData> _job_data_cache{ GRAPHICS_ALLOCATOR };
	Gaff::Counter _job_counter = 0;

	RenderManager* _render_mgr = nullptr;
	JobPool* _job_pool = nullptr;

	static void GenerateCommandListJob(uintptr_t id_int, void* data);
	static void DeviceJob(uintptr_t id_int, void* data);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::RenderCommandStage)
