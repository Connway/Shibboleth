/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

	.staticFunc("CopyShared", &Position::CopyShared)
	.staticFunc("Copy", &Position::Copy)
SHIB_REFLECTION_CLASS_DEFINE_END(Position)

void Position::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const glm::vec3& value)
{
	*reinterpret_cast<glm::vec3*>(ecs_mgr.getComponentShared<Position>(archetype)) = value;
}

void Position::SetShared(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value)
{
	*reinterpret_cast<glm::vec3*>(ecs_mgr.getComponentShared<Position>(id)) = value;
}

void Position::Set(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value)
{
	float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Position>(id)) + ecs_mgr.getPageIndex(id) % 4;
	component[0] = value.x;
	component[4] = value.y;
	component[8] = value.z;
}

const glm::vec3& Position::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return *reinterpret_cast<glm::vec3*>(ecs_mgr.getComponentShared<Position>(archetype));
}

const glm::vec3& Position::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return *reinterpret_cast<glm::vec3*>(ecs_mgr.getComponentShared<Position>(id));
}

glm::vec3 Position::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Position>(id)) + ecs_mgr.getPageIndex(id) % 4;

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

void Position::Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const float* const old_values = reinterpret_cast<const float*>(old_begin) + old_index;
	float* const new_values = reinterpret_cast<float*>(new_begin) + new_index;

	new_values[0] = old_values[0];
	new_values[4] = old_values[4];
	new_values[8] = old_values[8];
}

void Position::CopyShared(const void* old_value, void* new_value)
{
	*reinterpret_cast<glm::vec3*>(new_value) = *reinterpret_cast<const glm::vec3*>(old_value);
}



SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Rotation)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform"),
		ECSVarAttribute<glm::quat>()
	)

	.staticFunc("CopyShared", &Rotation::CopyShared)
	.staticFunc("Copy", &Rotation::Copy)
SHIB_REFLECTION_CLASS_DEFINE_END(Rotation)

void Rotation::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const glm::quat& value)
{
	*reinterpret_cast<glm::quat*>(ecs_mgr.getComponentShared<Rotation>(archetype)) = value;
}

void Rotation::SetShared(ECSManager& ecs_mgr, EntityID id, const glm::quat& value)
{
	*reinterpret_cast<glm::quat*>(ecs_mgr.getComponentShared<Rotation>(id)) = value;
}

void Rotation::Set(ECSManager& ecs_mgr, EntityID id, const glm::quat& value)
{
	float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Rotation>(id)) + ecs_mgr.getPageIndex(id) % 4;
	component[0] = value.w;
	component[4] = value.x;
	component[8] = value.y;
	component[12] = value.z;
}

const glm::quat& Rotation::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return *reinterpret_cast<glm::quat*>(ecs_mgr.getComponentShared<Rotation>(archetype));
}

const glm::quat& Rotation::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return *reinterpret_cast<glm::quat*>(ecs_mgr.getComponentShared<Rotation>(id));
}

glm::quat Rotation::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Rotation>(id)) + ecs_mgr.getPageIndex(id) % 4;

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

void Rotation::Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const float* const old_values = reinterpret_cast<const float*>(old_begin) + old_index;
	float* const new_values = reinterpret_cast<float*>(new_begin) + new_index;

	new_values[0] = old_values[0];
	new_values[4] = old_values[4];
	new_values[8] = old_values[8];
	new_values[12] = old_values[12];
}

void Rotation::CopyShared(const void* old_value, void* new_value)
{
	*reinterpret_cast<glm::quat*>(new_value) = *reinterpret_cast<const glm::quat*>(old_value);
}




SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Scale)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform"),
		ECSVarAttribute<glm::vec3>()
	)

	.staticFunc("CopyShared", &Scale::CopyShared)
	.staticFunc("Copy", &Scale::Copy)
SHIB_REFLECTION_CLASS_DEFINE_END(Scale)

void Scale::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const glm::vec3& value)
{
	*reinterpret_cast<glm::vec3*>(ecs_mgr.getComponentShared<Scale>(archetype)) = value;
}

void Scale::SetShared(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value)
{
	*reinterpret_cast<glm::vec3*>(ecs_mgr.getComponentShared<Scale>(id)) = value;
}

void Scale::Set(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value)
{
	float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Scale>(id)) + ecs_mgr.getPageIndex(id) % 4;
	component[0] = value.x;
	component[4] = value.y;
	component[8] = value.z;
}

const glm::vec3& Scale::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return *reinterpret_cast<glm::vec3*>(ecs_mgr.getComponentShared<Scale>(archetype));
}

const glm::vec3& Scale::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return *reinterpret_cast<glm::vec3*>(ecs_mgr.getComponentShared<Scale>(id));
}

glm::vec3 Scale::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* component = reinterpret_cast<float*>(ecs_mgr.getComponent<Scale>(id)) + ecs_mgr.getPageIndex(id) % 4;

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

void Scale::Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const float* const old_values = reinterpret_cast<const float*>(old_begin) + old_index;
	float* const new_values = reinterpret_cast<float*>(new_begin) + new_index;

	new_values[0] = old_values[0];
	new_values[4] = old_values[4];
	new_values[8] = old_values[8];
}

void Scale::CopyShared(const void* old_value, void* new_value)
{
	*reinterpret_cast<glm::vec3*>(new_value) = *reinterpret_cast<const glm::vec3*>(old_value);
}

NS_END
