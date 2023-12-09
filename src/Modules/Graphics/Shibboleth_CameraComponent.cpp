/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_RenderManagerBase.h>
#include <Shibboleth_AppUtils.h>
#include <Gaff_Math.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::CameraComponent)
	.template base<Shibboleth::EntitySceneComponent>()

	.template ctor<>()

	.var("device_tag", &Shibboleth::Camera::device_tag, Shibboleth::OptionalAttribute())
	.var("v_fov", &Shibboleth::Camera::v_fov, Shibboleth::OptionalAttribute())
	.var("z_near", &Shibboleth::Camera::z_near, Shibboleth::OptionalAttribute())
	.var("z_far", &Shibboleth::Camera::z_far, Shibboleth::OptionalAttribute())
	//.var("focus_distance", &Shibboleth::Camera::focus_distance, Shibboleth::OptionalAttribute())
	//.var("f_stop", &Shibboleth::Camera::f_stop, Shibboleth::OptionalAttribute())

	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::CameraComponent)

NS_SHIBBOLETH

void CameraComponent::setVerticalFOV(float focal_length, float sensor_size)
{
	GAFF_ASSERT(focal_length > 0.0f);
	GAFF_ASSERT(sensor_size > 0.0f);

	const float fov = Gaff::CalculateFOV(sensor_size, focal_length);
	setVerticalFOV(fov);
}

void CameraComponent::setVerticalFOVDegrees(float fov)
{
	_vertical_fov = fov * Gaff::DegreesToTurns;
}

void CameraComponent::setVerticalFOVRadians(float fov)
{
	_vertical_fov = fov * Gaff::RadiansToTurns;
}

void CameraComponent::setVerticalFOV(float fov)
{
	_vertical_fov = fov;
}

float CameraComponent::getVerticalFOVDegrees(void) const
{
	return _vertical_fov * Gaff::TurnsToDegrees;
}

float CameraComponent::getVerticalFOVRadians(void) const
{
	return _vertical_fov * Gaff::TurnsToRadians;
}

float CameraComponent::getVerticalFOV(void) const
{
	return _vertical_fov;
}

const Gleam::Vec2& CameraComponent::getZPlanes(void) const
{
	return _z_planes;
}

void CameraComponent::setZPlanes(const Gleam::Vec2& z_planes)
{
	GAFF_ASSERT(z_planes.x < z_planes.y);
	_z_planes = z_planes;
}

void CameraComponent::setZNear(float z_near)
{
	_z_planes.x = z_near;
}

float CameraComponent::getZNear(void) const
{
	return _z_planes.x;
}

void CameraComponent::setZFar(float z_far)
{
	_z_planes.y = z_far;
}

float CameraComponent::getZFar(void) const
{
	return _z_planes.y;
}

NS_END
