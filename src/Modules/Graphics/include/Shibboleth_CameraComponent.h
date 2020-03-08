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

#pragma once

#include <Shibboleth_ECSComponentBase.h>
#include <Shibboleth_Math.h>

NS_GAFF
	class ISerializeReader;
NS_END

NS_SHIBBOLETH

struct ECSQueryResult;
class ECSManager;

class Camera final : public ECSComponentBaseNonShared<Camera>
{
public:
	static void Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, const Camera& value);
	static void Set(ECSManager& ecs_mgr, EntityID id, const Camera& value);

	static Camera Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index);
	static Camera Get(ECSManager& ecs_mgr, EntityID id);

	static glm_vec4 GetVerticalFOVDegrees(const void* component_begin);
	static glm_vec4 GetVerticalFOV(const void* component_begin);
	static glm_vec4 GetFocalLength(const void* component_begin);
	static glm_vec4 GetZNear(const void* component_begin);
	static glm_vec4 GetZFar(const void* component_begin);
	//static glm_vec4 GetFocusDistance(const void* component_begin);
	//static glm_vec4 GetFocalLength(const void* component_begin);

	static void Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);

	// 35mm film (24mm x 36mm) [width x height]
	static constexpr float DefaultSensorSize = 36.0f;

	Camera(const float* component);
	Camera(void) = default;

	void SetVerticalFOV(float focal_length, float sensor_size = DefaultSensorSize);
	float GetVerticalFOV(void) const;

	Gaff::Hash32 device_tag = Gaff::FNV1aHash32Const("main");

	float v_fov = 0.25f; // turns
	float z_near = 0.001f; // m
	float z_far = 2000.0f; // m

	//float focus_distance;
	//float f_stop;
};

NS_END

SHIB_REFLECTION_DECLARE(Camera)
