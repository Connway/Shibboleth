/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Shibboleth_RasterManager.h"
#include "Shibboleth_SpatialManager.h"
#include "Shibboleth_RenderManager.h"
#include <Gleam_IRenderDevice.h>

NS_SHIBBOLETH

REF_IMPL_REQ(RasterManager);
REF_IMPL_SHIB(RasterManager);

RasterManager::RasterManager(App& app):
	_spatial_mgr(nullptr), _render_mgr(nullptr),
	_app(app)
{
}

RasterManager::~RasterManager(void)
{
}

void RasterManager::requestUpdateEntries(Array<UpdateEntry>& entries)
{
}

void RasterManager::allManagersCreated(void)
{
	_spatial_mgr = &_app.getManagerT<SpatialManager>("Spatial Manager");
	_render_mgr = &_app.getManagerT<RenderManager>("Render Manager");
}

unsigned int RasterManager::getNumCameras(void) const
{
	return _camera_data.size();
}

void RasterManager::setCameraEnabled(unsigned int camera, bool enabled)
{
	assert(camera < _camera_data.size());
	_camera_data[camera].enabled = enabled;
}

bool RasterManager::isCameraEnabled(unsigned int camera)
{
	assert(camera < _camera_data.size());
	return _camera_data[camera].enabled;
}

void RasterManager::setCameraTransform(unsigned int camera, const Gleam::Matrix4x4* transform)
{
	assert(camera < _camera_data.size());
	_camera_data[camera].transform = transform;
}

void RasterManager::setCameraDeviceEnabled(unsigned int camera, unsigned int device, bool enabled)
{
	assert(camera < _camera_data.size() && device < _camera_data[camera].device_data.size());
	_camera_data[camera].device_data[device].enabled = enabled;
}

RasterManager::RenderData& RasterManager::getCameraRenderData(unsigned int camera, unsigned int device)
{
	assert(camera < _camera_data.size() && device < _camera_data[camera].device_data.size());
	return _camera_data[camera].device_data[device].render_data;
}

unsigned int RasterManager::createCamera(unsigned int width, unsigned int height, Gleam::ITexture::FORMAT format, const AString& name)
{
	assert(_render_mgr);

	CameraData data;
	data.name = name;
	data.enabled = true;

	// We're about to do stuff to the Render Devices, lock it so that no one can mess with it
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_render_mgr->getSpinLock());
	data.device_data.resize(_render_mgr->getRenderDevice().getNumDevices());

	for (auto it = data.device_data.begin(); it != data.device_data.end(); ++it) {
		_render_mgr->getRenderDevice().setCurrentDevice(it - data.device_data.begin());
		it->render_data.first = _render_mgr->createTexture();
		it->enabled = true;

		if (!it->render_data.first) {
			// log error
			return UINT_FAIL;
		}

		if (!it->render_data.first->init2D(_render_mgr->getRenderDevice(), width, height, format)) {
			// log error
			return UINT_FAIL;
		}

		if (!it->render_data.second->init(_render_mgr->getRenderDevice(), it->render_data.first.get())) {
			// log error
			return UINT_FAIL;
		}
	}

	_camera_data.movePush(Gaff::Move(data));

	return _camera_data.size() - 1;
}

void RasterManager::deleteCameras(void)
{
	_camera_order.clear();
	_camera_data.clear();
}

void RasterManager::setCameraRenderOrder(const unsigned int* order, unsigned int size)
{
	assert(order && size == _camera_data.size());
	_camera_order.resize(size);

	for (unsigned int i = 0; i < size; ++i) {
		assert(order[i] < _camera_data.size());
		_camera_order[i] = order[i];
	}
}

void RasterManager::setCameraRenderOrder(const Array<unsigned int>& order)
{
	assert(order.size() == _camera_data.size());
	_camera_order = order;
}

void RasterManager::InitReflectionDefinition(void)
{
	if (!g_Ref_Def.isDefined()) {
		g_Ref_Def.setAllocator(ProxyAllocator());

		g_Ref_Def.addBaseClassInterfaceOnly<IUpdateQuery>();
		g_Ref_Def.markDefined();
	}
}

NS_END
