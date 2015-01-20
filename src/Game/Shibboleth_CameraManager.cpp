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
#include "Shibboleth_RenderManager.h"
#include <Shibboleth_CameraComponent.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

REF_IMPL_REQ(CameraManager);
REF_IMPL_SHIB(CameraManager)
.addBaseClassInterfaceOnly<CameraManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

CameraManager::CameraManager(IApp& app):
	_occlusion_manager(nullptr), _render_manager(nullptr),
	_app(app)
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
	_occlusion_manager = &_app.getManagerT<OcclusionManager>("Occlusion Manager");
	_render_manager = &_app.getManagerT<RenderManager>("Render Manager");
}

void CameraManager::requestUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.movePush(UpdateEntry(AString("Camera Manager: Render"), Gaff::Bind(this, &CameraManager::update)));
}

void CameraManager::update(double)
{
	for (auto it = _cameras.begin(); it != _cameras.end(); ++it) {
		if ((*it)->isActive()) {
			// get visible objects in camera frustrum
			// render objects to camera's render target
		}
	}
}

void CameraManager::registerCamera(CameraComponent* camera, unsigned int position)
{
	if (position == UINT_FAIL) {
		_cameras.push(camera);

	} else {
		_cameras.reserve(position);

		// If the position causes us to resize more than _size + 1,
		// then we are promising that we will fill those slots later.
		if (_cameras.size() < position) {
			_cameras.push(nullptr);
		}

		_cameras.insert(camera, position);
	}
}

void CameraManager::unregisterCamera(CameraComponent* camera)
{
	auto it = _cameras.linearSearch(camera);

	if (it != _cameras.end()) {
		// Use normal erase over fastErase to preserve ordering
		_cameras.erase(it);
	}
}

NS_END
