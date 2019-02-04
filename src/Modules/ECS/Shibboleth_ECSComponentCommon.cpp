/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_ECSComponentCommon.h"
#include "Shibboleth_ECSAttributes.h"
#include "Shibboleth_ECSManager.h"

SHIB_REFLECTION_DEFINE(Position)
SHIB_REFLECTION_DEFINE(Rotation)
SHIB_REFLECTION_DEFINE(Scale)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Position)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform"),
		ECSVarAttribute<glm::vec3>()
	)
SHIB_REFLECTION_CLASS_DEFINE_END(Position)

void Position::Set(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value)
{
	float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Position>(id)) + id.getPageIndex() % 4;
	component[0] = value.x;
	component[4] = value.y;
	component[8] = value.z;
}

glm::vec3 Position::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Position>(id)) + id.getPageIndex() % 4;

	return glm::vec3(
		component[0],
		component[4],
		component[8]
	);
}

glm_vec4 Position::GetX(void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<float*>(component_begin));
}

glm_vec4 Position::GetY(void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<float*>(component_begin) + 4);
}

glm_vec4 Position::GetZ(void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<float*>(component_begin) + 8);
}



SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Rotation)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform"),
		ECSVarAttribute<glm::quat>()
	)
SHIB_REFLECTION_CLASS_DEFINE_END(Rotation)

void Rotation::Set(ECSManager& ecs_mgr, EntityID id, const glm::quat& value)
{
	float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Rotation>(id)) + id.getPageIndex() % 4;
	component[0] = value.w;
	component[4] = value.x;
	component[8] = value.y;
	component[12] = value.z;
}

glm::quat Rotation::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Rotation>(id)) + id.getPageIndex() % 4;

	return glm::quat(
		component[0],
		component[4],
		component[8],
		component[12]
	);
}

glm_vec4 Rotation::GetX(void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<float*>(component_begin) + 4);
}

glm_vec4 Rotation::GetY(void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<float*>(component_begin) + 8);
}

glm_vec4 Rotation::GetZ(void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<float*>(component_begin) + 12);
}

glm_vec4 Rotation::GetW(void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<float*>(component_begin));
}



SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Scale)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform"),
		ECSVarAttribute<glm::vec3>()
	)
SHIB_REFLECTION_CLASS_DEFINE_END(Scale)

void Scale::Set(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value)
{
	float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Scale>(id)) + id.getPageIndex() % 4;
	component[0] = value.x;
	component[4] = value.y;
	component[8] = value.z;
}

glm::vec3 Scale::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Scale>(id)) + id.getPageIndex() % 4;

	return glm::vec3(
		component[0],
		component[4],
		component[8]
	);
}

glm_vec4 Scale::GetX(void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<float*>(component_begin));
}

glm_vec4 Scale::GetY(void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<float*>(component_begin) + 4);
}

glm_vec4 Scale::GetZ(void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<float*>(component_begin) + 8);
}

NS_END
