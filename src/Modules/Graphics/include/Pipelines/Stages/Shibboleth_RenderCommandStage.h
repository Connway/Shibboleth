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

#include "Pipelines/Shibboleth_IRenderPipelineStage.h"
#include "Resources/Shibboleth_ProgramBuffersResource.h"
#include "Resources/Shibboleth_BufferResource.h"
#include "Model/Shibboleth_ModelPipelineData.h"
#include "Shibboleth_RenderCommands.h"
#include <Shibboleth_JobPool.h>
#include <Gleam_ShaderResourceView.h>
#include <Gleam_Matrix4x4.h>
#include <Gleam_Shader.h>

NS_GLEAM
	class RenderTarget;
NS_END

NS_SHIBBOLETH

class CameraPipelineData;
class ResourceManager;

// $TODO: This should be split up into more discrete stages.
class RenderCommandStage final : public IRenderPipelineStage
{
public:
	bool init(RenderManager& render_mgr) override;
	//void destroy(RenderManager& /*render_mgr*/) override;

	void update(uintptr_t thread_id_int) override;

	const RenderCommandData* getRenderCommands(void) const override;

	SHIB_REFLECTION_CLASS_DECLARE(RenderCommandStage);

private:
	struct RenderJobData final
	{
		Gleam::Mat4x4 view_projection;

		const ModelPipelineData::MeshInstanceDeviceData* mesh_instance_data = nullptr;
		const ModelPipelineData::ModelBucket* model_bucket = nullptr;
		Gleam::RenderDevice* device = nullptr;
		Gleam::CommandList* cmd_list = nullptr;
		Gleam::RenderTarget* target = nullptr;

		RenderCommandStage* rcs = nullptr;
	};

	struct DeviceJobData final
	{
		DeviceJobData(void) = default;
		DeviceJobData(DeviceJobData&& data)
		{
			render_job_data_cache = std::move(data.render_job_data_cache);
			job_data_cache = std::move(data.job_data_cache);
			device = data.device;
			rcs = data.rcs;
			job_counter = static_cast<int32_t>(data.job_counter);
		}

		DeviceJobData(RenderCommandStage& stage, Gleam::RenderDevice& rd):
			rcs(&stage), device(&rd)
		{
		}

		Vector<RenderJobData> render_job_data_cache{ GRAPHICS_ALLOCATOR };
		Vector<Gaff::JobData> job_data_cache{ GRAPHICS_ALLOCATOR };
		Gleam::RenderDevice* device = nullptr;
		RenderCommandStage* rcs = nullptr;
		Gaff::Counter job_counter = 0;
	};

	RenderCommandData _render_commands;

	Vector<DeviceJobData> _device_job_data_cache{ GRAPHICS_ALLOCATOR };
	Vector<Gaff::JobData> _job_data_cache{ GRAPHICS_ALLOCATOR };
	Gaff::Counter _job_counter = 0;

	const CameraPipelineData* _camera_data = nullptr;
	const ModelPipelineData* _model_data = nullptr;

	RenderManager* _render_mgr = nullptr;
	JobPool* _job_pool = nullptr;

	static void GenerateCommandListJob(uintptr_t id_int, void* data);
	static void DeviceJob(uintptr_t id_int, void* data);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::RenderCommandStage)
