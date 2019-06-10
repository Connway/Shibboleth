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
#include <Shibboleth_EngineAttributesCommon.h>

SHIB_REFLECTION_EXTERNAL_DEFINE(Position)
SHIB_REFLECTION_EXTERNAL_DEFINE(Rotation)
SHIB_REFLECTION_EXTERNAL_DEFINE(Scale)
SHIB_REFLECTION_EXTERNAL_DEFINE(Layer)

NS_SHIBBOLETH

SHIB_REFLECTION_BUILDER_BEGIN(Position)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform"),
		ECSVarAttribute<glm::vec3>()
	)

	.staticFunc("CopyShared", &Position::CopyShared)
	.staticFunc("Copy", &Position::Copy)

	.var("Position", &Position::value)
	.ctor<>()
SHIB_REFLECTION_BUILDER_END(Position)

void Position::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const glm::vec3& value)
{
	ecs_mgr.getComponentShared<Position>(archetype)->value = value;
}

void Position::SetShared(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value)
{
	ecs_mgr.getComponentShared<Position>(id)->value = value;
}

void Position::Set(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index, const glm::vec3& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));
	component[0] = value.x;
	component[4] = value.y;
	component[8] = value.z;
}

void Position::Set(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Position>(id)) + ecs_mgr.getPageIndex(id) % 4;
	component[0] = value.x;
	component[4] = value.y;
	component[8] = value.z;
}

const glm::vec3& Position::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return ecs_mgr.getComponentShared<Position>(archetype)->value;
}

const glm::vec3& Position::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return ecs_mgr.getComponentShared<Position>(id)->value;
}

glm::vec3 Position::Get(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));

	return glm::vec3(
		component[0],
		component[4],
		component[8]
	);
}

glm::vec3 Position::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Position>(id)) + ecs_mgr.getPageIndex(id) % 4;

	return glm::vec3(
		component[0],
		component[4],
		component[8]
	);
}

glm_vec4 Position::GetX(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin));
}

glm_vec4 Position::GetY(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 4);
}

glm_vec4 Position::GetZ(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 8);
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
	reinterpret_cast<Position*>(new_value)->value = reinterpret_cast<const Position*>(old_value)->value;
}



SHIB_REFLECTION_BUILDER_BEGIN(Rotation)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform"),
		ECSVarAttribute<glm::quat>()
	)

	.staticFunc("CopyShared", &Rotation::CopyShared)
	.staticFunc("Copy", &Rotation::Copy)

	.var("Rotation", &Rotation::value)
	.ctor<>()
SHIB_REFLECTION_BUILDER_END(Rotation)

void Rotation::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const glm::quat& value)
{
	ecs_mgr.getComponentShared<Rotation>(archetype)->value = value;
}

void Rotation::SetShared(ECSManager& ecs_mgr, EntityID id, const glm::quat& value)
{
	ecs_mgr.getComponentShared<Rotation>(id)->value = value;
}

void Rotation::Set(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index, const glm::quat& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));
	component[0] = value.w;
	component[4] = value.x;
	component[8] = value.y;
	component[12] = value.z;
}

void Rotation::Set(ECSManager& ecs_mgr, EntityID id, const glm::quat& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Rotation>(id)) + ecs_mgr.getPageIndex(id) % 4;
	component[0] = value.w;
	component[4] = value.x;
	component[8] = value.y;
	component[12] = value.z;
}

const glm::quat& Rotation::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return ecs_mgr.getComponentShared<Rotation>(archetype)->value;
}

const glm::quat& Rotation::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return ecs_mgr.getComponentShared<Rotation>(id)->value;
}

glm::quat Rotation::Get(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));

	return glm::quat(
		component[0],
		component[4],
		component[8],
		component[12]
	);
}

glm::quat Rotation::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Rotation>(id)) + ecs_mgr.getPageIndex(id) % 4;

	return glm::quat(
		component[0],
		component[4],
		component[8],
		component[12]
	);
}

glm_vec4 Rotation::GetX(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 4);
}

glm_vec4 Rotation::GetY(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 8);
}

glm_vec4 Rotation::GetZ(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 12);
}

glm_vec4 Rotation::GetW(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin));
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
	reinterpret_cast<Rotation*>(new_value)->value = reinterpret_cast<const Rotation*>(old_value)->value;
}



SHIB_REFLECTION_BUILDER_BEGIN(Scale)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform"),
		ECSVarAttribute<glm::vec3>()
	)

	.staticFunc("CopyShared", &Scale::CopyShared)
	.staticFunc("Copy", &Scale::Copy)

	.var("Scale", &Scale::value)
	.ctor<>()
SHIB_REFLECTION_BUILDER_END(Scale)

void Scale::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const glm::vec3& value)
{
	ecs_mgr.getComponentShared<Scale>(archetype)->value = value;
}

void Scale::SetShared(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value)
{
	ecs_mgr.getComponentShared<Scale>(id)->value = value;
}

void Scale::Set(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index, const glm::vec3& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));
	component[0] = value.x;
	component[4] = value.y;
	component[8] = value.z;
}

void Scale::Set(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Scale>(id)) + ecs_mgr.getPageIndex(id) % 4;
	component[0] = value.x;
	component[4] = value.y;
	component[8] = value.z;
}

const glm::vec3& Scale::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return ecs_mgr.getComponentShared<Scale>(archetype)->value;
}

const glm::vec3& Scale::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return ecs_mgr.getComponentShared<Scale>(id)->value;
}

glm::vec3 Scale::Get(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));

	return glm::vec3(
		component[0],
		component[4],
		component[8]
	);
}

glm::vec3 Scale::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Scale>(id)) + ecs_mgr.getPageIndex(id) % 4;

	return glm::vec3(
		component[0],
		component[4],
		component[8]
	);
}

glm_vec4 Scale::GetX(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin));
}

glm_vec4 Scale::GetY(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 4);
}

glm_vec4 Scale::GetZ(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 8);
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
	reinterpret_cast<Scale*>(new_value)->value = reinterpret_cast<const Scale*>(old_value)->value;
}



SHIB_REFLECTION_BUILDER_BEGIN(Layer)
	.classAttrs(
		ECSClassAttribute(nullptr, "Scene"),
		ECSVarAttribute<Gaff::Hash32>()
	)

	.staticFunc("CopyShared", &Layer::CopyShared)
	//.staticFunc("Copy", &Layer::Copy)

	.var("Name", &Layer::value, HashStringAttribute())
	.ctor<>()
SHIB_REFLECTION_BUILDER_END(Layer)

void Layer::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, Gaff::Hash32 value)
{
	ecs_mgr.getComponentShared<Layer>(archetype)->value = value;
}

void Layer::SetShared(ECSManager& ecs_mgr, EntityID id, Gaff::Hash32 value)
{
	ecs_mgr.getComponentShared<Layer>(id)->value = value;
}

//void Layer::Set(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index, Gaff::Hash32 value)
//{
//	Gaff::Hash32* const component = reinterpret_cast<Gaff::Hash32*>(ecs_mgr.getComponent(query_result, entity_index));
//	*component = value;
//}
//
//void Layer::Set(ECSManager& ecs_mgr, EntityID id, Gaff::Hash32 value)
//{
//	Gaff::Hash32* const component = reinterpret_cast<Gaff::Hash32*>(ecs_mgr.getComponent<Layer>(id)) + ecs_mgr.getPageIndex(id) % 4;
//	*component = value;
//}

Gaff::Hash32 Layer::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return ecs_mgr.getComponentShared<Layer>(archetype)->value;
}

Gaff::Hash32 Layer::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return ecs_mgr.getComponentShared<Layer>(id)->value;
}

//Gaff::Hash32 Layer::Get(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index)
//{
//	return *reinterpret_cast<const Gaff::Hash32*>(ecs_mgr.getComponent(query_result, entity_index));
//}
//
//Gaff::Hash32 Layer::Get(ECSManager& ecs_mgr, EntityID id)
//{
//	return *reinterpret_cast<const Gaff::Hash32*>(ecs_mgr.getComponent<Layer>(id)) + ecs_mgr.getPageIndex(id) % 4;
//}

//glm_uvec4 Layer::Get(const void* component_begin)
//{
//	return _mm_load_si128(reinterpret_cast<const glm_uvec4*>(component_begin));
//}

//void Layer::Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
//{
//	const Gaff::Hash32* const old_value = reinterpret_cast<const Gaff::Hash32*>(old_begin) + old_index;
//	Gaff::Hash32* const new_value = reinterpret_cast<Gaff::Hash32*>(new_begin) + new_index;
//
//	*new_value = *old_value;
//}

void Layer::CopyShared(const void* old_value, void* new_value)
{
	reinterpret_cast<Layer*>(new_value)->value = reinterpret_cast<const Layer*>(old_value)->value;
}

NS_END
