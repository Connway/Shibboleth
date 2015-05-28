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

#include "Shibboleth_CameraComponent.h"
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_CameraManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

REF_IMPL_REQ(CameraComponent);
SHIB_REF_IMPL(CameraComponent)
.addBaseClassInterfaceOnly<CameraComponent>()
.addArray("Viewport", &CameraComponent::_viewport)
.addArray("Clear Color", &CameraComponent::_clear_color)
.addFloat("Field of View", &CameraComponent::getFOV, &CameraComponent::setFOV)
.addFloat("Z Near", &CameraComponent::_z_near)
.addFloat("Z Far", &CameraComponent::_z_far)
.addUChar("Render Order", &CameraComponent::_render_order)
.addBool("Active", &CameraComponent::_active)
;

CameraComponent::CameraComponent(void):
	_aspect_ratio(1.0f), _fov(Gaff::DegToRad * 90.0f),
	_z_near(0.0001f), _z_far(5000.0f), _window_tags(0),
	_render_order(0), _active(true)
{
	_clear_color[0] = 0.0f;
	_clear_color[1] = 0.0f;
	_clear_color[2] = 0.0f;
	_clear_color[3] = 1.0f;

	_viewport[0] = 0.0f;
	_viewport[1] = 0.0f;
	_viewport[2] = 1.0f;
	_viewport[3] = 1.0f;
}

CameraComponent::~CameraComponent(void)
{
	setActive(false);
}

bool CameraComponent::validate(Gaff::JSON& json)
{
	// IMPLEMENT ME

	return true;
}

bool CameraComponent::load(const Gaff::JSON& json)
{
	gRefDef.read(json, this);

	_render_target = GetApp().getManagerT<ResourceManager>("Resource Manager").requestResource(json["Render Target File"].getString());

	if (_active) {
		getOwner()->registerForWorldDirtyCallback(Gaff::Bind(this, &CameraComponent::updateFrustum));
	}

	_devices = GetApp().getManagerT<RenderManager>("Render Manager").getDevicesWithTagsAny(_window_tags);
	GetApp().getManagerT<CameraManager>("Camera Manager").registerCamera(this);

	return true;
}

bool CameraComponent::save(Gaff::JSON& json)
{
	gRefDef.write(json, this);

	Gaff::JSON viewport = Gaff::JSON::createArray();
	json.setObject("Viewport", viewport);

	Gaff::JSON clear_color = Gaff::JSON::createArray();
	json.setObject("Clear Color", clear_color);

	return true;
}

void CameraComponent::updateFrustum(Object* object, unsigned long long)
{
	_frustum = _unstransformed_frustum;
	_frustum.transform(object->getWorldTransform());
}

const Gleam::Matrix4x4CPU& CameraComponent::getProjectionMatrix(void) const
{
	return _projection_matrix;
}

const Gleam::FrustumCPU& CameraComponent::getFrustum(void) const
{
	return _frustum;
}

const float* CameraComponent::getViewport(void) const
{
	return _viewport;
}

float CameraComponent::getAspectRatio(void) const
{
	return _aspect_ratio;
}

float CameraComponent::getFOV(void) const
{
	return _fov;
}

void CameraComponent::setFOV(float fov)
{
	_fov = fov;
	constructProjectionMatrixAndFrustum();
}

float CameraComponent::getZNear(void) const
{
	return _z_near;
}

void CameraComponent::setZNear(float z_near)
{
	_z_near = z_near;
	constructProjectionMatrixAndFrustum();
}

float CameraComponent::getZFar(void) const
{
	return _z_far;
}

void CameraComponent::setZFar(float z_far)
{
	_z_far = z_far;
	constructProjectionMatrixAndFrustum();
}

const Array<unsigned int>& CameraComponent::getDevices(void) const
{
	return _devices;
}

unsigned short CameraComponent::getWindowTags(void) const
{
	return _window_tags;
}

unsigned char CameraComponent::getRenderOrder(void) const
{
	return _render_order;
}

void CameraComponent::setActive(bool active)
{
	if (active && !_active) {
		getOwner()->registerForWorldDirtyCallback(Gaff::Bind(this, &CameraComponent::updateFrustum));
	} else if (!active && _active) {
		getOwner()->unregisterForWorldDirtyCallback(Gaff::Bind(this, &CameraComponent::updateFrustum));
	}

	_active = active;
}

bool CameraComponent::isActive(void) const
{
	return _active;
}

void CameraComponent::RenderTargetCallback(const AHashString& /*resource*/, bool success)
{
	if (!success) {
		// log error
		GetApp().quit();
		return;
	}

	_aspect_ratio = static_cast<float>(_render_target->width) / static_cast<float>(_render_target->height);
	constructProjectionMatrixAndFrustum();
}

void CameraComponent::constructProjectionMatrixAndFrustum(void)
{
	_projection_matrix.setPerspectiveLH(_fov, _aspect_ratio, _z_near, _z_far);
	_unstransformed_frustum.construct(_fov, _aspect_ratio, _z_near, _z_far);
	updateFrustum(getOwner(), 0);
}

NS_END
