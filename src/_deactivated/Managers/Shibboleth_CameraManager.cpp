/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Shibboleth_CameraManager.h"
#include "Shibboleth_IOcclusionManager.h"
#include "Shibboleth_IFrameManager.h"
#include <Shibboleth_OcclusionUserDataTags.h>
#include <Shibboleth_ModelAnimResources.h>
#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_ModelComponent.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>
#include <Gleam_ICommandList.h>
#include <Gleam_IProgram.h>
#include <Gleam_IModel.h>

NS_SHIBBOLETH

REF_IMPL_REQ(CameraManager);
SHIB_REF_IMPL(CameraManager)
.addBaseClassInterfaceOnly<CameraManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(ICameraManager)
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

CameraManager::CameraManager(void):
	_occlusion_mgr(nullptr)
{
}

CameraManager::~CameraManager(void)
{
}

const char* CameraManager::getName(void) const
{
	return GetFriendlyName();
}

void CameraManager::allManagersCreated(void)
{
	_occlusion_mgr = &GetApp().getManagerT<IOcclusionManager>();
}

void CameraManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.emplacePush(U8String("Camera Manager: Update"), Gaff::Bind(this, &CameraManager::update));
}

void CameraManager::registerCamera(CameraComponent* camera)
{
	auto it = _cameras.binarySearch(camera, [](const CameraComponent* lhs, const CameraComponent* rhs) -> bool
	{
		return lhs->getRenderOrder() < rhs->getRenderOrder();
	});

	_cameras.emplace(it, camera);
}

void CameraManager::removeCamera(CameraComponent* camera)
{
	auto it = _cameras.binarySearch(camera, [](const CameraComponent* lhs, const CameraComponent* rhs) -> bool
	{
		return lhs->getRenderOrder() < rhs->getRenderOrder();
	});

	GAFF_ASSERT(it != _cameras.end() && *it == camera);
	_cameras.erase(it);
}

void CameraManager::addModelComponent(ObjectData& od, ModelComponent* mc, const Gleam::Vector4CPU& eye_pos)
{
	ModelData& model_data = mc->getModel();

	size_t lod = mc->determineLOD(eye_pos);

	Array<ModelPtr> lod_models(model_data.models.height(), ModelPtr());

	// For each device, gather all the LOD models for each device
	for (size_t i = 0; i < model_data.models.height(); ++i) {
		//Array<ModelPtr>& models = model_data.models[i];

		if (model_data.models[i][0] && model_data.models[i][lod]) {
			lod_models[i] = model_data.models[i][lod];
		}
	}

	od.models.emplacePush(std::move(lod_models));

	auto& mesh_data = mc->getMeshData();

	for (auto it = mesh_data.begin(); it != mesh_data.end(); ++it) {
		od.raster_states.emplacePush(it->material->raster_states);
		od.program_buffers.emplacePush(it->program_buffers->data);
		od.programs.emplacePush(it->material->programs);
		od.render_pass.emplacePush(it->material->render_pass);
	}

	//// Assumes every LOD has the same number of meshes per model
	//auto& program_buffers = mc->getProgramBuffers();
	//auto& programs = mc->getMaterials();

	//GAFF_ASSERT(program_buffers.size() == programs.size());

	//// For each material per mesh, add the shader data
	//for (size_t i = 0; i < program_buffers.size(); ++i) {
	//	// TODO: Copy and clone all program buffer data in case a buffer gets modified mid render or between frames.
	//	// Clone the program buffers in case any data gets changed.
	//	//Array<ProgramBuffersPtr> pbs(program_buffers[i]->data.size());

	//	//for (size_t j = 0; j < pbs.size(); ++j) {
	//	//	pbs[j] = program_buffers[i]->data[j]->clone();
	//	//}

	//	//od.program_buffers.emplacePush(std::move(pbs));

	//	od.raster_states.emplacePush(programs[i]->raster_states);
	//	od.program_buffers.emplacePush(program_buffers[i]->data);
	//	od.programs.emplacePush(programs[i]->programs);
	//	od.render_pass.emplacePush(programs[i]->render_pass);
	//}
}

void CameraManager::update(double, void* frame_data)
{
	FrameData* fd = reinterpret_cast<FrameData*>(frame_data);
	fd->camera_object_data.clearNoFree();

	IOcclusionManager::QueryData query_result;

	for (auto it = _cameras.begin(); it != _cameras.end(); ++it) {
		ObjectData& od = fd->camera_object_data[*it];

		od.active = (*it)->isActive();

		if (od.active) {
			od.projection_matrix = (*it)->getProjectionMatrix();
			od.eye_transform = (*it)->getOwner()->getWorldTransform();
			od.inv_eye_transform = od.eye_transform.inverse();
			od.clear_mode = (*it)->getClearMode();

			if (od.clear_mode == CameraComponent::CM_COLOR) {
				memcpy(od.clear_color, (*it)->getClearColor(), sizeof(float) * 4);
			}

			//_occlusion_mgr->findObjectsInFrustum((*it)->getFrustum(), od.objects);
			_occlusion_mgr->findObjectsInFrustum((*it)->getFrustum(), query_result);

			for (int i = IOcclusionManager::OT_STATIC; i < IOcclusionManager::OT_SIZE; ++i) {
				// Copy all the transforms.
				for (auto it_obj = query_result.results[i].begin(); it_obj != query_result.results[i].end(); ++it_obj) {
					//od.transforms[i].emplacePush(it_obj->first->getWorldTransform());
					od.transforms.emplacePush(it_obj->first->getWorldTransform());

					switch (it_obj->second.second) {
					case OMT_MODEL_COMP:
						addModelComponent(od, reinterpret_cast<ModelComponent*>(it_obj->second.first), it_obj->first->getWorldTransform().getTranslation());
						break;

					case OMT_INST_COMP:
						//addInstancedModelComponent(od, reinterpret_cast<InstancedModelComponent*>(it_obj->second.first));
						break;
					}
				}
			}
		}
	}
}

NS_END