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

#include "Shibboleth_CameraManager.h"
#include "Shibboleth_OcclusionManager.h"
#include "Shibboleth_FrameManager.h"
#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

REF_IMPL_REQ(CameraManager);
SHIB_REF_IMPL(CameraManager)
.addBaseClassInterfaceOnly<CameraManager>()
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
	return "Camera Manager";
}

void CameraManager::allManagersCreated(void)
{
	_occlusion_mgr = &GetApp().getManagerT<OcclusionManager>("Occlusion Manager");
}

void CameraManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.emplacePush(AString("Camera Manager: Update"), Gaff::Bind(this, &CameraManager::update));
}

void CameraManager::update(double, void* frame_data)
{
	FrameData* fd = reinterpret_cast<FrameData*>(frame_data);
	fd->object_data.clearNoFree();

	for (size_t i = 0; i < _cameras.size(); ++i) {
		if (_cameras[i]->isActive()) {
			fd->object_data.emplacePush();

			FrameData::ObjectData& od = fd->object_data.last();
			od.camera_transform = _cameras[i]->getOwner()->getWorldTransform();
			od.camera = _cameras[i];

			_occlusion_mgr->findObjectsInFrustum(_cameras[i]->getFrustum(), od.objects);

			// Skip over static objects. Assuming static objects will never actually move.
			for (int j = OcclusionManager::OT_DYNAMIC; j < OcclusionManager::OT_SIZE; ++j) {
				// Copy all the transforms.
				for (auto it = od.objects.results[j].begin(); it != od.objects.results[j].end(); ++it) {
					od.transforms[j - 1].emplacePush(it->first->getWorldTransform());
				}
			}
		}
	}
}

void CameraManager::registerCamera(CameraComponent* camera)
{
	auto it = _cameras.binarySearch(camera, [](const CameraComponent* lhs, const CameraComponent* rhs) -> bool
	{
		return lhs->getRenderOrder() < rhs->getRenderOrder();
	});

	_cameras.emplaceInsert(it, camera);
}

void CameraManager::removeCamera(CameraComponent* camera)
{
	auto it = _cameras.binarySearch(camera, [](const CameraComponent* lhs, const CameraComponent* rhs) -> bool
	{
		return lhs->getRenderOrder() < rhs->getRenderOrder();
	});

	assert(it != _cameras.end() && *it == camera);
	_cameras.erase(it);
}

NS_END