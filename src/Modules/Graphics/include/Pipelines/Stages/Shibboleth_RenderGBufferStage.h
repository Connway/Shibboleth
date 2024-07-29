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
#include "Resources/Shibboleth_MaterialResource.h"
#include "Shibboleth_RenderCommands.h"
#include <Shibboleth_JobPool.h>
#include <Gleam_ProgramBuffers.h>
#include <Gleam_SamplerState.h>
#include <Gleam_RasterState.h>

NS_GLEAM
	class Program;
NS_END

NS_SHIBBOLETH

class CameraPipelineData;

class RenderGBufferStage final : public IRenderPipelineStage
{
public:
	bool init(RenderManager& render_mgr) override;

	void update(uintptr_t thread_id_int) override;

	const RenderCommandData* getRenderCommands(void) const override;

private:
	struct DeviceJobData final
	{
		Gleam::RenderDevice* device = nullptr;
		RenderGBufferStage* rgbs = nullptr;

		UniquePtr<Gleam::ProgramBuffers> program_buffers;
		UniquePtr<Gleam::RasterState> raster_state;
		UniquePtr<Gleam::SamplerState> sampler;
		Gleam::Program* program = nullptr;
	};

	RenderCommandData _render_commands;

	const CameraPipelineData* _camera_data = nullptr;
	RenderManager* _render_mgr = nullptr;

	ResourcePtr<MaterialResource> _render_g_buffer_material;

	Vector<DeviceJobData> _device_job_data_cache{ GRAPHICS_ALLOCATOR };
	Vector<Gaff::JobData> _job_data_cache{ GRAPHICS_ALLOCATOR };
	Gaff::Counter _job_counter = 0;

	static void DeviceJob(uintptr_t id_int, void* data);

	SHIB_REFLECTION_CLASS_DECLARE(RenderGBufferStage);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::RenderGBufferStage)
