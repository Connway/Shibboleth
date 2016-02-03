/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Shibboleth_InGameRenderPipeline.h"
#include <Shibboleth_OcclusionManager.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_FrameManager.h>

#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_ModelComponent.h>

#include <Shibboleth_ModelAnimResources.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

#include <Gleam_IRenderDevice.h>
#include <Gleam_IProgram.h>
#include <Gleam_IBuffer.h>
#include <Gleam_IModel.h>

NS_SHIBBOLETH

struct RenderInfo
{
	RenderInfo(
		const Gleam::TransformCPU& ot, RasterStatePtr rs, ProgramBuffersPtr pbs,
		ProgramPtr p, ModelPtr mo, size_t me
	):
		object_transform(ot), raster_state(rs),
		program_buffers(pbs), program(p), model(mo),
		mesh(me)
	{
	}

	const Gleam::TransformCPU& object_transform;

	RasterStatePtr raster_state;
	ProgramBuffersPtr program_buffers;
	ProgramPtr program;
	ModelPtr model;
	size_t mesh;
};

// Will want to further sort this into objects with the same material/renderstate pairs
// Will also want to add support for instancing
static THREAD_LOCAL Array<RenderInfo> gRender_pass_info[RP_COUNT];
static THREAD_LOCAL bool gInit = false;

InGameRenderPipeline::InGameRenderPipeline(void):
	_job_cache(GetApp().getJobPool().getNumTotalThreads()),
	_counter(nullptr),
	_render_mgr(GetApp().getManagerT<RenderManager>())
{
}

InGameRenderPipeline::~InGameRenderPipeline(void)
{
	if (_counter) {
		GetApp().getJobPool().freeCounter(_counter);
	}
}

bool InGameRenderPipeline::init(void)
{
	// Create RenderStates for each render pass
	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();
	Gleam::IDepthStencilState::DepthStencilStateSettings dss;
	Gleam::IBlendState::BlendStateSettings bss;

	// RenderState for opaque pass
	bss.blend_src_color = Gleam::IBlendState::BLEND_FACTOR_SRC_COLOR;
	bss.blend_dst_color = Gleam::IBlendState::BLEND_FACTOR_DEST_COLOR;
	bss.blend_op_color = Gleam::IBlendState::BLEND_OP_ADD;
	bss.blend_src_alpha = Gleam::IBlendState::BLEND_FACTOR_SRC_ALPHA;
	bss.blend_dst_alpha = Gleam::IBlendState::BLEND_FACTOR_DEST_ALPHA;
	bss.blend_op_alpha = Gleam::IBlendState::BLEND_OP_SUBTRACT;
	bss.color_write_mask = Gleam::IBlendState::COLOR_ALL;
	bss.enable_alpha_blending = false;

	dss.front_face.stencil_depth_fail = Gleam::IDepthStencilState::STENCIL_KEEP;
	dss.front_face.stencil_depth_pass = Gleam::IDepthStencilState::STENCIL_KEEP;
	dss.front_face.stencil_pass_depth_fail = Gleam::IDepthStencilState::STENCIL_KEEP;
	dss.front_face.comp_func = Gleam::IDepthStencilState::COMPARE_ALWAYS;

	dss.back_face = dss.front_face;

	dss.stencil_read_mask = 0xFF;
	dss.stencil_write_mask = 0xFF;
	dss.stencil_test = false;
	dss.stencil_ref = 0;

	dss.depth_func = Gleam::IDepthStencilState::COMPARE_LESS;
	dss.depth_test = true;

	_ds_states[RP_OPAQUE].resize(rd.getNumDevices());
	_ds_states[RP_TRANSPARENT].resize(rd.getNumDevices());
	_blend_states[RP_OPAQUE].resize(rd.getNumDevices());
	_blend_states[RP_TRANSPARENT].resize(rd.getNumDevices());

	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		Gleam::IDepthStencilState* ds = _render_mgr.createDepthStencilState();
		rd.setCurrentDevice(i);

		if (!ds) {
			// log error
			return false;
		}

		if (!ds->init(rd, dss)) {
			// log error
			return false;
		}

		_ds_states[RP_OPAQUE][i] = ds;
		_ds_states[RP_TRANSPARENT][i] = ds;

		Gleam::IBlendState* bs = _render_mgr.createBlendState();

		if (!bs) {
			// log error
			return false;
		}

		if (!bs->init(rd, bss)) {
			// log error
			return false;
		}

		_blend_states[RP_OPAQUE][i] = bs;

		bss.enable_alpha_blending = true;

		bs = _render_mgr.createBlendState();

		if (!bs) {
			// log error
			return false;
		}

		if (!bs->init(rd, bss)) {
			// log error
			return false;
		}

		_blend_states[RP_TRANSPARENT][i] = bs;
	}

	return true;
}

const char* InGameRenderPipeline::getName(void) const
{
	return "In-Game Render Pipeline";
}

void InGameRenderPipeline::run(double, void* frame_data)
{
	FrameData* fd = reinterpret_cast<FrameData*>(frame_data);

	for (auto it = fd->camera_object_data.begin(); it != fd->camera_object_data.end(); ++it) {
		if (it->second.command_lists.empty()) {
			unsigned int size = _render_mgr.getRenderDevice().getNumDevices();
			it->second.command_lists.resize(size);
			it->second.cmd_lists_locks.resize(size);
		}

		it->second.curr_device = 0;
	}

	// Maybe pre-allocate the destination arrays so that we don't have to lock in helper threads?

	GenerateJobData job_data(this, fd);

	for (size_t i = 0; i < GetApp().getJobPool().getNumTotalThreads(); ++i) {
		_job_cache.emplacePush(&InGameRenderPipeline::GenerateCommandLists, &job_data);
	}

	GetApp().getJobPool().addJobs(_job_cache.getArray(), _job_cache.size(), &_counter);
	GetApp().getJobPool().helpWhileWaiting(_counter);

	_job_cache.clearNoFree();
}

void InGameRenderPipeline::GenerateCommandLists(void* job_data)
{
	GenerateJobData* jd = reinterpret_cast<GenerateJobData*>(job_data);
	Array<RenderManager::RenderDevicePtr>& rds = jd->first->_render_mgr.getDeferredRenderDevices(Gaff::Thread::GetCurrentThreadID());

	GenerateCameraCommandLists(rds, jd);
	GenerateLightCommandLists(rds, jd);
}

void InGameRenderPipeline::GenerateCameraCommandLists(Array<RenderManager::RenderDevicePtr>& rds, GenerateJobData* jd)
{
	if (!gInit) {
		Gaff::construct(gRender_pass_info + RP_OPAQUE);
		Gaff::construct(gRender_pass_info + RP_TRANSPARENT);
		gInit = true;
	}

	FrameData* fd = jd->second;

	// For each camera
	for (auto it = fd->camera_object_data.begin(); it != fd->camera_object_data.end(); ++it) {
		if (!it->second.active) {
			continue;
		}

		// Grab an available camera
		unsigned int device_index = AtomicIncrement(&it->second.curr_device) - 1;
		auto& devices = it->first->getDevices();

		// Create command list for each device
		while (device_index < devices.size()) {
			unsigned int device = devices[device_index];
			auto& rd = rds[device];
			auto& od = it->second;

			// Change to pre-allocate command lists and re-use them instead of destroying and re-creating them.
			Gleam::ICommandList* cmd_list = jd->first->_render_mgr.createCommandList();

			if (!cmd_list) {
				// log error
				continue;
			}

			SortIntoRenderPasses(od, device);

			Gleam::IRenderTargetPtr& rt = it->first->getRenderTarget()->render_targets[device];
			rt->bind(*rd);

			ClearCamera(rd.get(), it->second, rt);
			RunCommands(rd.get(), jd, device, od);

			// generate command list
			if (!rd->finishCommandList(cmd_list)) {
				// log error
			}

			it->second.cmd_lists_locks[device].lock();
			it->second.command_lists[device].emplacePush(cmd_list);
			it->second.cmd_lists_locks[device].unlock();

			device_index = AtomicIncrement(&it->second.curr_device) - 1;

			gRender_pass_info[RP_OPAQUE].clearNoFree();
			gRender_pass_info[RP_TRANSPARENT].clearNoFree();
		}
	}
}

void InGameRenderPipeline::GenerateLightCommandLists(Array<RenderManager::RenderDevicePtr>& rds, GenerateJobData* jd)
{
	
}

void InGameRenderPipeline::SortIntoRenderPasses(ObjectData& od, unsigned int device)
{
	// TODO: Batch by matching raster state and shaders.
	for (size_t obj = 0, mesh = 0; obj < od.transforms.size(); ++obj) {
		auto& model = od.models[obj];

		if (model[device]) {
			for (size_t i = 0; i < model[device]->getMeshCount(); ++i, ++mesh) {
				auto& rss = od.raster_states[mesh];
				auto& pbs = od.program_buffers[mesh];
				auto& programs = od.programs[mesh];

				assert(pbs[device] && programs[device]);

				gRender_pass_info[od.render_pass[mesh]].emplacePush(
					od.transforms[obj], rss[device], pbs[device],
					programs[device], model[device], i
				);
			}
		}
	}
}

void InGameRenderPipeline::RunCommands(Gleam::IRenderDevice* rd, GenerateJobData* jd, unsigned int device, const ObjectData& od)
{
	for (int i = 0; i < RP_COUNT; ++i) {
		jd->first->_ds_states[i][device]->set(*rd);
		jd->first->_blend_states[i == RP_TRANSPARENT][device]->set(*rd);

		for (auto it = gRender_pass_info[i].begin(); it != gRender_pass_info[i].end(); ++it) {
			// Copy transform matrices over. Always assume the first constant buffer in the
			// vertex shader is the transform buffer and that it is sized appropriately.
			if (it->program_buffers->getConstantBufferCount()) {
				Gleam::IBuffer* buffer = it->program_buffers->getConstantBuffer(Gleam::IShader::SHADER_VERTEX, 0);

				float* transforms = reinterpret_cast<float*>(buffer->map(*rd));

				if (!transforms) {
					// log error
					continue;
				}

				Gleam::TransformCPU final_transform = od.inv_eye_transform + it->object_transform;
				Gleam::Matrix4x4CPU final_matrix = od.projection_matrix * final_transform.matrix();

				memcpy(transforms, final_matrix.getBuffer(), sizeof(float) * 16);
				buffer->unmap(*rd);
			}

			it->raster_state->set(*rd);
			it->program->bind(*rd, it->program_buffers.get());
			it->model->render(*rd, it->mesh);
		}
	}
}

void InGameRenderPipeline::ClearCamera(Gleam::IRenderDevice* rd, ObjectData& od, Gleam::IRenderTargetPtr& rt)
{
	switch (od.clear_mode) {
		case CameraComponent::CM_SKYBOX:
			// Currently unsupported
			break;

		case CameraComponent::CM_COLOR:
			rt->clear(*rd, Gleam::IRenderTarget::CLEAR_ALL, 1.0f, 0, od.clear_color);
			break;

		case CameraComponent::CM_DEPTH_STENCIL:
			rt->clear(*rd, Gleam::IRenderTarget::CLEAR_DEPTH | Gleam::IRenderTarget::CLEAR_STENCIL);
			break;

		case CameraComponent::CM_STENCIL:
			rt->clear(*rd, Gleam::IRenderTarget::CLEAR_STENCIL);
			break;

		case CameraComponent::CM_NOTHING:
			break;
	}
}

NS_END
