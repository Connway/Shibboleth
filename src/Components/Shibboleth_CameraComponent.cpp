/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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
#include <Shibboleth_IResourceManager.h>
#include <Shibboleth_ISchemaManager.h>
#include <Shibboleth_IRenderManager.h>
#include <Shibboleth_ICameraManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

SHIB_ENUM_REF_IMPL_EMBEDDED(ClearMode, CameraComponent::ClearMode)
.addValue("Skybox", CameraComponent::CM_SKYBOX)
.addValue("Color", CameraComponent::CM_COLOR)
.addValue("Depth-Stencil", CameraComponent::CM_DEPTH_STENCIL)
.addValue("Stencil", CameraComponent::CM_STENCIL)
.addValue("Nothing", CameraComponent::CM_NOTHING)
;

REF_IMPL_REQ(CameraComponent);
SHIB_REF_IMPL(CameraComponent)
.addBaseClassInterfaceOnly<CameraComponent>()
.ADD_BASE_CLASS_INTERFACE_ONLY(ICameraComponent)
.addEnum("Clear Mode", &CameraComponent::_clear_mode, GetEnumRefDef<CameraComponent::ClearMode>())
.addArray("Viewport", &CameraComponent::_viewport)
.addArray("Clear Color", &CameraComponent::_clear_color)
.addFloat("Field of View", &CameraComponent::getFOV, &CameraComponent::setFOV)
.addFloat("Z Near", &CameraComponent::_z_near)
.addFloat("Z Far", &CameraComponent::_z_far)
.addUChar("Render Order", &CameraComponent::_render_order)
.addBool("Active", &CameraComponent::isActive, &CameraComponent::setActive)
;

CameraComponent::CameraComponent(void):
	_clear_mode(CM_COLOR), _aspect_ratio(1.0f),
	_fov(Gaff::DegToRad * 90.0f), _z_near(0.1f),
	_z_far(5000.0f), _render_order(0)
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

const Gaff::JSON& CameraComponent::getSchema(void) const
{
	static const Gaff::JSON& schema = GetApp().getManagerT<ISchemaManager>().getSchema("CameraComponent.schema");
	return schema;
}

bool CameraComponent::validate(const Gaff::JSON& json)
{
	Component::validate(json);
	return true;
}

bool CameraComponent::load(const Gaff::JSON& json)
{
	g_ref_def.read(json, this);

	auto resCallback = Gaff::Bind(this, &CameraComponent::renderTargetCallback);
	_render_target = GetApp().getManagerT<IResourceManager>().requestResource(json["Render Target File"].getString());
	_render_target.getResourcePtr()->addCallback(resCallback);

	if (isActive()) {
		getOwner()->registerForWorldDirtyCallback(Gaff::Bind(this, &CameraComponent::updateFrustum));
	}

	GetApp().getManagerT<ICameraManager>().registerCamera(this);

	return true;
}

bool CameraComponent::save(Gaff::JSON& json)
{
	g_ref_def.write(json, this);
	return true;
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

const float* CameraComponent::getClearColor(void) const
{
	return _clear_color;
}

void CameraComponent::setClearColor(float r, float g, float b, float a)
{
	_clear_color[0] = r;
	_clear_color[1] = g;
	_clear_color[2] = b;
	_clear_color[3] = a;
}

void CameraComponent::setClearColor(const float* color)
{
	memcpy(_clear_color, color, sizeof(float) * 4);
}

CameraComponent::ClearMode CameraComponent::getClearMode(void) const
{
	return _clear_mode;
}

void CameraComponent::setClearMode(ClearMode cm)
{
	_clear_mode = cm;
}

ResourceWrapper<RenderTargetData>& CameraComponent::getRenderTarget(void)
{
	return _render_target;
}

const Array<unsigned int>& CameraComponent::getDevices(void) const
{
	return _devices;
}

unsigned char CameraComponent::getRenderOrder(void) const
{
	return _render_order;
}

void CameraComponent::setActive(bool active)
{
	Component::setActive(active);

	if (active && !isActive()) {
		getOwner()->registerForWorldDirtyCallback(Gaff::Bind(this, &CameraComponent::updateFrustum));
	} else if (!active && isActive()) {
		getOwner()->unregisterForWorldDirtyCallback(Gaff::Bind(this, &CameraComponent::updateFrustum));
	}
}

void CameraComponent::renderTargetCallback(ResourceContainer*)
{
	if (_render_target.getResourcePtr()->hasFailed()) {
		// log error
		GetApp().quit();
		return;
	}

	_aspect_ratio = static_cast<float>(_render_target->width) / static_cast<float>(_render_target->height);
	constructProjectionMatrixAndFrustum();

	IRenderManager& render_mgr = GetApp().getManagerT<IRenderManager>();

	_devices = (_render_target->any_display_with_tags) ?
		render_mgr.getDevicesWithTagsAny(_render_target->tags) :
		render_mgr.getDevicesWithTags(_render_target->tags);
}

void CameraComponent::constructProjectionMatrixAndFrustum(void)
{
	_projection_matrix.setPerspectiveLH(_fov * Gaff::DegToRad, _aspect_ratio, _z_near, _z_far);
	_unstransformed_frustum.construct(_fov, _aspect_ratio, _z_near, _z_far);
	updateFrustum(getOwner(), 0);
}

void CameraComponent::updateFrustum(Object* object, uint64_t)
{
	_frustum = _unstransformed_frustum;
	_frustum.transform(object->getWorldTransform());
}

NS_END
