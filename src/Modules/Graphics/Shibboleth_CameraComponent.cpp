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
#include <Shibboleth_ECSAttributes.h>
#include <Shibboleth_ECSManager.h>
#include <Shibboleth_AppUtils.h>
#include <Gaff_Math.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::Camera)
	.classAttrs(
		Shibboleth::ECSClassAttribute(nullptr, u8"Graphics")
	)

	.base< Shibboleth::ECSComponentBaseNonShared<Shibboleth::Camera> >()

	.staticFunc("Destructor", Shibboleth::Camera::Destructor)

	.var("device_tag", &Shibboleth::Camera::device_tag, Shibboleth::OptionalAttribute())
	.var("v_fov", &Shibboleth::Camera::v_fov, Shibboleth::OptionalAttribute())
	.var("z_near", &Shibboleth::Camera::z_near, Shibboleth::OptionalAttribute())
	.var("z_far", &Shibboleth::Camera::z_far, Shibboleth::OptionalAttribute())
	//.var("focus_distance", &Shibboleth::Camera::focus_distance, Shibboleth::OptionalAttribute())
	//.var("f_stop", &Shibboleth::Camera::f_stop, Shibboleth::OptionalAttribute())

	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::Camera)

NS_SHIBBOLETH

void Camera::CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const Gaff::Hash32* const old_device = reinterpret_cast<const Gaff::Hash32*>(old_begin) + old_index;
	Gaff::Hash32* const new_device = reinterpret_cast<Gaff::Hash32*>(new_begin) + new_index;

	const float* const old_values = GetFloatBegin(old_begin, old_index);
	float* const new_values = GetFloatBegin(new_begin, new_index);

	*new_device = *old_device;
	new_values[0] = old_values[0];
	new_values[4] = old_values[4];
	new_values[8] = old_values[8];
	//new_values[12] = old_values[12];
	//new_values[16] = old_values[16];
}

void Camera::SetInternal(void* component, int32_t page_index, const Camera& value)
{
	Gaff::Hash32* const device = reinterpret_cast<Gaff::Hash32*>(component) + page_index;
	float* const comp = GetFloatBegin(component, page_index);

	*device = value.device_tag;
	comp[0] = value.v_fov;
	comp[4] = value.z_near;
	comp[8] = value.z_far;
}

Camera Camera::GetInternal(const void* component, int32_t page_index)
{
	const Gaff::Hash32* const device = reinterpret_cast<const Gaff::Hash32*>(component) + page_index;

	Camera camera(GetFloatBegin(component, page_index));
	camera.device_tag = *device;

	return camera;
}

void Camera::Destructor(ECSEntityID id, void*, int32_t)
{
	auto& render_mgr = GETMANAGERT(Shibboleth::RenderManagerBase, Shibboleth::RenderManager);
	render_mgr.removeGBuffer(id);
}

Gleam::Vec4SIMD Camera::GetVerticalFOVDegrees(const void* component, int32_t page_index)
{
	return GetVerticalFOV(component, page_index) * Gleam::Vec4SIMD(Gaff::RadToDeg);
}

Gleam::Vec4SIMD Camera::GetVerticalFOV(const void* component, int32_t page_index)
{
	// 2.0f * atan(0.5f * sensor_size / focal_length)
	const Gleam::Vec4SIMD sensor_size = Gleam::Vec4SIMD(DefaultSensorSize * 0.5f);
	const Gleam::Vec4SIMD focal_length = GetFocalLength(component, page_index);

	Gleam::Vec4SIMD fov = Gleam::Vec4SIMD(sensor_size / focal_length);
	fov = glm::atan(fov);
	fov = fov * Gleam::Vec4SIMD(2.0f);

	return fov;
}

Gleam::Vec4SIMD Camera::GetFocalLength(const void* component, int32_t page_index)
{
	const float* const vec = GetFloatBegin(component, page_index);
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

Gleam::Vec4SIMD Camera::GetZNear(const void* component, int32_t page_index)
{
	const float* const vec = GetFloatBegin(component, page_index) + 4;
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

Gleam::Vec4SIMD Camera::GetZFar(const void* component, int32_t page_index)
{
	const float* const vec = GetFloatBegin(component, page_index) + 8;
	return Gleam::Vec4SIMD(vec[0], vec[1], vec[2], vec[3]);
}

//Gleam::Vec4SIMD Camera::GetFocusDistance(const void* component, int32_t page_index)
//{
//	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 12);
//}

//Gleam::Vec4SIMD GetFocalLength(const void* component, int32_t page_index)
//{
//	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 16);
//}

Camera::Camera(const float* component):
	v_fov(component[0]),
	z_near(component[4]),
	z_far(component[8])
{
}

void Camera::SetVerticalFOV(float focal_length, float sensor_size)
{
	v_fov = Gaff::CalculateFOV(sensor_size, focal_length);
}

float Camera::GetVerticalFOV(void) const
{
	return v_fov;
}

const float* Camera::GetFloatBegin(const void* component, int32_t page_index)
{
	return GetFloatBegin(const_cast<void*>(component), page_index);
}

float* Camera::GetFloatBegin(void* component, int32_t page_index)
{
	Gaff::Hash32* const device = reinterpret_cast<Gaff::Hash32*>(component);
	float* const comp = reinterpret_cast<float*>(device + 4) + page_index;
	return comp;
}

NS_END
