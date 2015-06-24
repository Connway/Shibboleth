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

	// First time using this frame data. Size the render commands appropriately.
	if (fd->render_commands.empty()) {
		fd->render_commands.resize(GetEnumRefDef<RenderModes>().getNumEntries());
	}

	// Clear the old command lists
	for (size_t i = 0; i < fd->render_commands.size(); ++i) {
		fd->render_commands[i].command_lists.clearNoFree();
	}

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

	for (auto it = fd->object_data.begin(); it != fd->object_data.end(); ++it) {
		for (size_t i = 0; i < OcclusionManager::OT_SIZE; ++i) {
			// Get the next object to be processed in the list.
			unsigned int index = AtomicIncrement(&it->next_index[i]) - 1;

			// If the next index is larger than the number of objects in the list, move on to the next list.
			if (index >= it->objects.results[i].size()) {
				continue;
			}

			const OcclusionManager::QueryResult& result = it->objects.results[i][index];
			Gleam::TransformCPU final_transform;
			Gleam::TransformCPU inverse_camera = it->camera_transform.inverse();

			// If we're a static object, just grab the transform from the object.
			// Maybe just copy the transforms for static objects anyways for cache coherency?
			if (i == OcclusionManager::OT_STATIC) {
				final_transform = inverse_camera + result.first->getWorldTransform();

			// Otherwise, grab it from the copies we made.
			} else {
				final_transform = inverse_camera + it->transforms[i - 1][index];
			}

			// Pre-computing the world-to-camera-to-projection matrix.
			Gleam::Matrix4x4CPU final_matrix = it->camera->getProjectionMatrix() * final_transform.matrix();

			assert(result.second.first);
			ModelComponent* model_comp = reinterpret_cast<ModelComponent*>(result.second.first);

			size_t lod = model_comp->determineLOD(it->camera_transform.getTranslation());
			auto& program_buffers = model_comp->getProgramBuffers();
			auto& materials = model_comp->getMaterials();
			ModelData& model = model_comp->getModel();

			const Array<unsigned int>& camera_devices = it->camera->getDevices();

			for (auto it_dev = camera_devices.begin(); it_dev != camera_devices.end(); ++it_dev) {
				// update program buffers with transform information

				ModelPtr& m = model.models[*it_dev][lod];

				for (size_t j = 0; j < m->getMeshCount(); ++j) {
					materials[j]->programs[*it_dev]->bind(*rds[*it_dev]);
					m->render(*rds[*it_dev], j);
				}

				// generate command list
				// add it to command list list
			}
		}
	}
}

NS_END
