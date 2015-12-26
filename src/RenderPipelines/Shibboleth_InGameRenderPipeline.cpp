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
	//ResourceManager& res_mgr = GetApp().getManagerT<ResourceManager>();

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

void InGameRenderPipeline::run(double dt, void* frame_data)
{
	FrameData* fd = reinterpret_cast<FrameData*>(frame_data);

	for (auto it = fd->camera_object_data.begin(); it != fd->camera_object_data.end(); ++it) {
		if (it->second.command_lists.empty()) {
			it->second.command_lists.resize(_render_mgr.getRenderDevice().getNumDevices());

			for (size_t i = 0; i < it->second.command_lists.size(); ++i) {
				it->second.command_lists[i] = _render_mgr.createCommandList();
				assert(it->second.command_lists[i]);
			}
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
	FrameData* fd = jd->second;

	// For each camera
	for (auto it = fd->camera_object_data.begin(); it != fd->camera_object_data.end(); ++it) {
		if (!it->second.active) {
			continue;
		}

		// Grab an available camera
		unsigned int device_index = AtomicIncrement(&it->second.curr_device) - 1;

		// Create command list for each device
		while (device_index < it->first->getDevices().size()) {
			unsigned int device = it->first->getDevices()[device_index];
			auto& rd = rds[device];

			// Change to pre-allocate command lists and re-use them instead of destroying and re-creating them.
			Gleam::ICommandList* cmd_list = jd->first->_render_mgr.createCommandList();

			if (!cmd_list) {
				// log error
				continue;
			}


			// For each object type (minus lights)
			for (size_t i = 0; i < OcclusionManager::OT_SIZE - 1; ++i) {
				// Group together into instance buffer

				//// Set render state

				// For each object
				for (size_t j = 0; j < it->second.objects.results[i].size(); ++j) {
					const OcclusionManager::QueryResult& result = it->second.objects.results[i][j];
					//Gleam::TransformCPU inverse_camera = it->second.eye_transform.inverse();
					//Gleam::TransformCPU final_transform = inverse_camera + it->second.transforms[i][j];
					//Gleam::Matrix4x4CPU final_matrix = it->second.projection_matrix * final_transform.matrix();

					assert(result.second.first);
					ModelComponent* model_comp = reinterpret_cast<ModelComponent*>(result.second.first);


					//size_t lod = model_comp->determineLOD(it->second.eye_transform.getTranslation());
					//auto& program_buffers = model_comp->getProgramBuffers();
					//auto& materials = model_comp->getMaterials();
					//auto& buffers = model_comp->getBuffers();
					//ModelData& model = model_comp->getModel();

					//// Update program buffers with transform information
					//// Buffer zero is always assumed to be the transform buffer
					//BufferPtr& buffer = buffers[0]->data[device];
					//float* transforms = reinterpret_cast<float*>(buffer->map(*rd));

					//if (!transforms) {
					//	// log error
					//	continue;
					//}

					//memcpy(transforms, final_matrix.getBuffer(), sizeof(float) * 16);

					//buffer->unmap(*rd);

					//ModelPtr& m = model.models[device][lod];

					////for (size_t k = 0; k < RP_COUNT; ++k) {
					////	if (render_modes[k].empty()) {
					////		continue;
					////	}

					////	for (size_t l = 0; l < render_modes[k].size(); ++l) {
					////		size_t rm = render_modes[k][l];

					////		materials[rm]->programs[device]->bind(*rd, program_buffers[rm]->data[device].get());
					////		m->render(*rd, rm);
					////	}
					////}
				}
			}

			// generate command list
			if (!rd->finishCommandList(cmd_list)) {
				// log error
			}

			it->second.command_lists[device] = cmd_list;
			device = AtomicIncrement(&it->second.curr_device) - 1;
		}
	}
}

void InGameRenderPipeline::GenerateLightCommandLists(Array<RenderManager::RenderDevicePtr>& rds, GenerateJobData* jd)
{
	
}

NS_END
