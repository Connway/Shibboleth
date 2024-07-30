/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
#include <Attributes/Shibboleth_EngineAttributesCommon.h>
#include <Gaff_Math.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::CameraComponent)
	.template base<Shibboleth::EntitySceneComponent>()
	.template ctor<>()

	.var("view", &Shibboleth::CameraComponent::_view, Shibboleth::OptionalAttribute())
SHIB_REFLECTION_DEFINE_END(Shibboleth::CameraComponent)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(CameraComponent)

void CameraComponent::setFOV(float focal_length, float sensor_size)
{
	GAFF_ASSERT(focal_length > 0.0f);
	GAFF_ASSERT(sensor_size > 0.0f);

	const float fov = Gaff::CalculateFOV(sensor_size, focal_length);
	setFOV(fov);
}

void CameraComponent::setFOVDegrees(float fov)
{
	_view.fov = fov * Gaff::DegreesToTurns;
}

void CameraComponent::setFOVRadians(float fov)
{
	_view.fov = fov * Gaff::RadiansToTurns;
}

void CameraComponent::setFOV(float fov)
{
	_view.fov = fov;
}

float CameraComponent::getFOVDegrees(void) const
{
	return _view.fov * Gaff::TurnsToDegrees;
}

float CameraComponent::getFOVRadians(void) const
{
	return _view.fov * Gaff::TurnsToRadians;
}

float CameraComponent::getFOV(void) const
{
	return _view.fov;
}

const CameraView& CameraComponent::generateView(float /*dt*/)
{
	const Gleam::Transform& tform = getTransformWorld();
	_view.transform.setTranslation(tform.getTranslation());
	_view.transform.setRotationQuatTurns(tform.getRotation());

	return _view;
}

const CameraView& CameraComponent::getCurrentView(void) const
{
	return _view;
}

NS_END
