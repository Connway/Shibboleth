/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
#include <Shibboleth_ECSAttributes.h>
#include <Shibboleth_ECSManager.h>

SHIB_REFLECTION_DEFINE_BEGIN(Camera)
	.classAttrs(
		ECSClassAttribute(nullptr, "Graphics")
	)

	.base< ECSComponentBaseNonShared<Camera> >()

	.staticFunc("Copy", &Camera::Copy)

	.var("v_fov", &Camera::v_fov, OptionalAttribute())
	.var("z_near", &Camera::z_near, OptionalAttribute())
	.var("z_far", &Camera::z_far, OptionalAttribute())
	//.var("focus_distance", &Camera::focus_distance, OptionalAttribute())
	//.var("f_stop", &Camera::f_stop, OptionalAttribute())
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Camera)

NS_SHIBBOLETH

void Camera::Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, const Camera& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index)) + ecs_mgr.getPageIndex(query_result, entity_index) % 4;
	component[0] = value.v_fov;
	component[4] = value.z_near;
	component[8] = value.z_far;
	//component[12] = value.focus_distance;
	//component[16] = value.f_stop;
}

void Camera::Set(ECSManager& ecs_mgr, EntityID id, const Camera& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Camera>(id)) + ecs_mgr.getPageIndex(id) % 4;

	component[0] = value.v_fov;
	component[4] = value.z_near;
	component[8] = value.z_far;
	//component[12] = value.focus_distance;
	//component[16] = value.f_stop;
}

Camera Camera::Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index)) + ecs_mgr.getPageIndex(query_result, entity_index) % 4;
	return Camera(component);
}

Camera Camera::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Camera>(id)) + ecs_mgr.getPageIndex(id) % 4;
	return Camera(component);
}

glm_vec4 Camera::GetVerticalFOVDegrees(const void* component_begin)
{
	return glm_vec4_mul(GetVerticalFOV(component_begin), _mm_set_ps1(Gaff::RadToDeg));
}

glm_vec4 Camera::GetVerticalFOV(const void* component_begin)
{
	// 2.0f * atan(0.5f * sensor_size / focal_length)
	const glm_vec4 sensor_size = _mm_set_ps1(DefaultSensorSize * 0.5f);
	const glm_vec4 focal_length = GetFocalLength(component_begin);

	glm_vec4 fov = glm_vec4_div(sensor_size, focal_length);
	fov = _mm_atan_ps(fov);
	fov = glm_vec4_mul(fov, _mm_set_ps1(2.0f));

	return fov;
}

glm_vec4 Camera::GetFocalLength(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin));
}

glm_vec4 Camera::GetZNear(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 4);
}

glm_vec4 Camera::GetZFar(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 8);
}

//glm_vec4 Camera::GetFocusDistance(const void* component_begin)
//{
//	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 12);
//}

//glm_vec4 GetFocalLength(const void* component_begin)
//{
//	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 16);
//}

void Camera::Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const float* const old_values = reinterpret_cast<const float*>(old_begin) + old_index;
	float* const new_values = reinterpret_cast<float*>(new_begin) + new_index;

	new_values[0] = old_values[0];
	new_values[4] = old_values[4];
	new_values[8] = old_values[8];
	//new_values[12] = old_values[12];
	//new_values[16] = old_values[16];
}

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

NS_END
