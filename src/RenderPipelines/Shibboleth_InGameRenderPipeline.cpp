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
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_FrameManager.h>

#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_ModelComponent.h>

#include <Shibboleth_ModelAnimResources.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>

#include <Gleam_IRenderDevice.h>
#include <Gleam_IProgram.h>
#include <Gleam_IModel.h>

NS_SHIBBOLETH

using JobData = Gaff::Pair<InGameRenderPipeline*, FrameData*>;

InGameRenderPipeline::InGameRenderPipeline(void):
	_render_mgr(GetApp().getManagerT<RenderManager>("Render Manager"))
{
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

	// If we are on the first run, allocate the command lists for each thread
	if (fd->command_lists.empty()) {
		Array<unsigned int> thread_ids(GetApp().getJobPool().getNumTotalThreads(), 0);
		GetApp().getJobPool().getThreadIDs(thread_ids.getArray());
		fd->command_lists.reserve(thread_ids.size());

		for (auto it = thread_ids.begin(); it != thread_ids.end(); ++it) {
			fd->command_lists[*it] = FrameData::CommandListMap();
		}
	}

	// Clear the old command lists
	for (auto it_thread = fd->command_lists.begin(); it_thread != fd->command_lists.end(); ++it_thread) {
		for (auto it = it_thread->second.begin(); it != it_thread->second.end(); ++it) {
			for (size_t i = 0; i < it->second.size(); ++i) {
				for (size_t j = 0; j < it->second[i].size(); ++j) {
					it->second[i][j].clearNoFree();
				}
			}
		}
	}

	// Clear the object indexes
	for (size_t i = 0; i < fd->object_data.size(); ++i) {
		for (size_t j = 0; j < OcclusionManager::OT_SIZE; ++j) {
			fd->object_data[i].next_index[j] = 0;
		}
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
	auto& cl = fd->command_lists[Gaff::Thread::GetCurrentThreadID()];

	for (auto it = fd->object_data.begin(); it != fd->object_data.end(); ++it) {
		for (size_t i = 0; i < OcclusionManager::OT_SIZE; ++i) {
			// Get the next object to be processed in the list.
			unsigned int index = AtomicIncrement(&it->next_index[i]) - 1;

			// Process objects until we run out, then move on to the next list.
			// If the next index is larger than the number of objects in the list, move on to the next list.
			while (index < it->objects.results[i].size()) {
				const OcclusionManager::QueryResult& result = it->objects.results[i][index];
				Gleam::TransformCPU final_transform;
				Gleam::TransformCPU inverse_camera = it->camera_transform.inverse();

				final_transform = inverse_camera + it->transforms[i][index];

				// Pre-computing the world-to-camera-to-projection matrix.
				Gleam::Matrix4x4CPU final_matrix = it->camera->getProjectionMatrix() * final_transform.matrix();

				assert(result.second.first);
				ModelComponent* model_comp = reinterpret_cast<ModelComponent*>(result.second.first);

				size_t lod = model_comp->determineLOD(it->camera_transform.getTranslation());
				auto& program_buffers = model_comp->getProgramBuffers();
				auto& materials = model_comp->getMaterials();
				auto& buffers = model_comp->getBuffers();
				ModelData& model = model_comp->getModel();

				const Array<unsigned int>& camera_devices = it->camera->getDevices();
				const Array<size_t>* render_modes = model_comp->getRenderModes();
				auto& command_lists = cl[it->camera];

				// First time using this frame data. Size the render commands appropriately.
				if (command_lists.empty()) {
					command_lists.resize(jd->first->_render_mgr.getRenderDevice().getNumDevices());

					for (size_t j = 0; j < cl.size(); ++j) {
						command_lists[j].resize(GetEnumRefDef<RenderModes>().getNumEntries() - 1);
					}
				}

				for (auto it_dev = camera_devices.begin(); it_dev != camera_devices.end(); ++it_dev) {
					auto& rd = rds[*it_dev];

					// Update program buffers with transform information
					// Buffer zero is always assumed to be the transform buffer
					BufferPtr& buffer = buffers[0]->data[*it_dev];
					float* transforms = reinterpret_cast<float*>(buffer->map(*rd));

					if (!transforms) {
						// log error
						continue;
					}

					memcpy(transforms, final_matrix.getBuffer(), sizeof(float) * 16);

					buffer->unmap(*rd);

					ModelPtr& m = model.models[*it_dev][lod];

					for (size_t j = 0; j < GetEnumRefDef<RenderModes>().getNumEntries() - 1; ++j) {
						if (render_modes[j].empty()) {
							continue;
						}

						Gleam::ICommandList* cmd_list = jd->first->_render_mgr.createCommandList();

						if (!cmd_list) {
							// log error
							continue;
						}

						for (size_t k = 0; k < render_modes[j].size(); ++k) {
							size_t rm = render_modes[j][k];

							materials[rm]->programs[*it_dev]->bind(*rd, program_buffers[rm]->data[*it_dev].get());
							m->render(*rd, rm);
						}

						// generate command list
						if (!rd->finishCommandList(cmd_list)) {
							// log error
							GetAllocator()->freeT(cmd_list);
							continue;
						}

						// add it to command list list
						command_lists[*it_dev][j].emplacePush(cmd_list);
					}
				}

				// Get the next object to be processed in the list.
				index = AtomicIncrement(&it->next_index[i]) - 1;
			}
		}
	}
}

NS_END
