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
#include <Gleam_Transform.h>
#include <Gleam_IProgram.h>
#include <Gleam_IBuffer.h>
#include <Gleam_IModel.h>

NS_SHIBBOLETH

using JobData = Gaff::Pair<InGameRenderPipeline*, FrameData*>;

InGameRenderPipeline::InGameRenderPipeline(void):
	_render_mgr(GetApp().getManagerT<RenderManager>())
{
	ResourceManager& res_mgr = GetApp().getManagerT<ResourceManager>();

	for (size_t i = 0; i < GetEnumRefDef<RenderModes>().getNumEntries() - 1; ++i) {
	}
}

InGameRenderPipeline::~InGameRenderPipeline(void)
{
}

const char* InGameRenderPipeline::getName(void) const
{
	return "In-Game Render Pipeline";
}

void InGameRenderPipeline::run(double dt, void* frame_data)
{
	FrameData* fd = reinterpret_cast<FrameData*>(frame_data);

	for (auto it = fd->object_data.begin(); it != fd->object_data.end(); ++it) {
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

	size_t num_threads = GetApp().getJobPool().getNumTotalThreads();

	Array<Gaff::JobData> jobs(num_threads);
	Gaff::Counter* counter = nullptr;

	JobData job_data(this, fd);

	for (size_t i = 0; i < num_threads; ++i) {
		jobs.emplacePush(&InGameRenderPipeline::GenerateCommandLists, &job_data);
	}

	GetApp().getJobPool().addJobs(jobs.getArray(), jobs.size(), &counter);
	GetApp().getJobPool().helpWhileWaiting(counter);
}

void InGameRenderPipeline::GenerateCommandLists(void* job_data)
{
	JobData* jd = reinterpret_cast<JobData*>(job_data);
	Array<RenderManager::RenderDevicePtr>& rds = jd->first->_render_mgr.getDeferredRenderDevices(Gaff::Thread::GetCurrentThreadID());
	FrameData* fd = jd->second;

	for (auto it = fd->object_data.begin(); it != fd->object_data.end(); ++it) {
		if (!it->second.active) {
			continue;
		}

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

			// For each object type
			for (size_t i = 0; i < OcclusionManager::OT_SIZE; ++i) {
				// Set render state

				// For each object
				for (size_t j = 0; j < it->second.objects.results[i].size(); ++j) {
					const OcclusionManager::QueryResult& result = it->second.objects.results[i][j];
					Gleam::TransformCPU inverse_camera = it->second.camera_transform.inverse();
					Gleam::TransformCPU final_transform = inverse_camera + it->second.transforms[i][j];
					Gleam::Matrix4x4CPU final_matrix = it->second.camera_projection_matrix * final_transform.matrix();

					assert(result.second.first);
					ModelComponent* model_comp = reinterpret_cast<ModelComponent*>(result.second.first);

					size_t lod = model_comp->determineLOD(it->second.camera_transform.getTranslation());
					auto& program_buffers = model_comp->getProgramBuffers();
					auto& materials = model_comp->getMaterials();	
					auto& buffers = model_comp->getBuffers();
					ModelData& model = model_comp->getModel();

					const Array<size_t>* render_modes = model_comp->getRenderModes();

					// Update program buffers with transform information
					// Buffer zero is always assumed to be the transform buffer
					BufferPtr& buffer = buffers[0]->data[device];
					float* transforms = reinterpret_cast<float*>(buffer->map(*rd));

					if (!transforms) {
						// log error
						continue;
					}

					memcpy(transforms, final_matrix.getBuffer(), sizeof(float) * 16);

					buffer->unmap(*rd);

					ModelPtr& m = model.models[device][lod];

					for (size_t k = 0; k < GetEnumRefDef<RenderModes>().getNumEntries() - 1; ++k) {
						if (render_modes[k].empty()) {
							continue;
						}

						for (size_t l = 0; l < render_modes[k].size(); ++l) {
							size_t rm = render_modes[k][l];

							materials[rm]->programs[device]->bind(*rd, program_buffers[rm]->data[device].get());
							m->render(*rd, rm);
						}
					}

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

NS_END
