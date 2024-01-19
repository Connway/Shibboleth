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

#pragma once

#include <Shibboleth_EntitySceneComponent.h>
#include <Shibboleth_Math.h>

NS_SHIBBOLETH

class CameraComponent final : public EntitySceneComponent
{
public:
	CameraComponent(void) = default;

	void setVerticalFOV(float focal_length, float sensor_size);
	void setVerticalFOVDegrees(float fov);
	void setVerticalFOVRadians(float fov);
	void setVerticalFOV(float fov);
	float getVerticalFOVDegrees(void) const;
	float getVerticalFOVRadians(void) const;
	float getVerticalFOV(void) const;

	void setZPlanes(const Gleam::Vec2& z_planes);
	const Gleam::Vec2& getZPlanes(void) const;

	void setZNear(float z_near);
	float getZNear(void) const;

	void setZFar(float z_far);
	float getZFar(void) const;

private:
	Gleam::Vec2 _z_planes{ 0.001f, 2000.0f }; // m
	float _vertical_fov = 0.25f; // turns

	SHIB_REFLECTION_CLASS_DECLARE(CameraComponent);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::CameraComponent)
