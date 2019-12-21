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
		ECSClassAttribute(nullptr, "Transform")
	)

	.base< ECSComponentBase<Position> >()

	.staticFunc("Copy", &Position::Copy)
	.var("value", &Position::value)
	.ctor<>()
SHIB_REFLECTION_BUILDER_END(Position)

void Position::Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, const Position& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));
	component[0] = value.value.x;
	component[4] = value.value.y;
	component[8] = value.value.z;
}

void Position::Set(ECSManager& ecs_mgr, EntityID id, const Position& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Position>(id)) + ecs_mgr.getPageIndex(id) % 4;
	component[0] = value.value.x;
	component[4] = value.value.y;
	component[8] = value.value.z;
}

Position Position::Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));

	return Position(glm::vec3(
		component[0],
		component[4],
		component[8]
	));
}

Position Position::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Position>(id)) + ecs_mgr.getPageIndex(id) % 4;

	return Position(glm::vec3(
		component[0],
		component[4],
		component[8]
	));
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

Position::Position(const glm::vec3& val):
	value(val)
{
}



SHIB_REFLECTION_BUILDER_BEGIN(Rotation)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform")
	)

	.base< ECSComponentBase<Rotation> >()

	.staticFunc("Copy", &Rotation::Copy)
	.var("value", &Rotation::value)
	.ctor<>()
SHIB_REFLECTION_BUILDER_END(Rotation)

void Rotation::Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, const Rotation& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));
	component[0] = value.value.x;
	component[4] = value.value.y;
	component[8] = value.value.z;
	component[12] = value.value.w;
}

void Rotation::Set(ECSManager& ecs_mgr, EntityID id, const Rotation& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Rotation>(id)) + ecs_mgr.getPageIndex(id) % 4;
	component[0] = value.value.x;
	component[4] = value.value.y;
	component[8] = value.value.z;
	component[12] = value.value.w;
}

Rotation Rotation::Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));

	return Rotation(glm::quat(
		component[12],
		component[0],
		component[4],
		component[8]
	));
}

Rotation Rotation::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Rotation>(id)) + ecs_mgr.getPageIndex(id) % 4;

	return Rotation(glm::quat(
		component[12],
		component[0],
		component[4],
		component[8]
	));
}

glm_vec4 Rotation::GetX(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin));
}

glm_vec4 Rotation::GetY(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 4);
}

glm_vec4 Rotation::GetZ(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 8);
}

glm_vec4 Rotation::GetW(const void* component_begin)
{
	return _mm_load_ps(reinterpret_cast<const float*>(component_begin) + 12);
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

Rotation::Rotation(const glm::quat& val):
	value(val)
{
}



SHIB_REFLECTION_BUILDER_BEGIN(Scale)
	.classAttrs(
		ECSClassAttribute(nullptr, "Transform")
	)

	.base< ECSComponentBase<Scale> >()

	.staticFunc("Copy", &Scale::Copy)
	.var("value", &Scale::value)
	.ctor<>()
SHIB_REFLECTION_BUILDER_END(Scale)

void Scale::Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, const Scale& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));
	component[0] = value.value.x;
	component[4] = value.value.y;
	component[8] = value.value.z;
}

void Scale::Set(ECSManager& ecs_mgr, EntityID id, const Scale& value)
{
	float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Scale>(id)) + ecs_mgr.getPageIndex(id) % 4;
	component[0] = value.value.x;
	component[4] = value.value.y;
	component[8] = value.value.z;
}

Scale Scale::Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent(query_result, entity_index));

	return Scale(glm::vec3(
		component[0],
		component[4],
		component[8]
	));
}

Scale Scale::Get(ECSManager& ecs_mgr, EntityID id)
{
	const float* const component = reinterpret_cast<float*>(ecs_mgr.getComponent<Scale>(id)) + ecs_mgr.getPageIndex(id) % 4;

	return Scale(glm::vec3(
		component[0],
		component[4],
		component[8]
	));
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

Scale::Scale(const glm::vec3& val):
	value(val)
{
}



SHIB_REFLECTION_BUILDER_BEGIN(Layer)
	.classAttrs(
		ECSClassAttribute(nullptr, "Scene")
	)

	.base< ECSComponentBase<Layer> >()

	//.staticFunc("Copy", &Layer::Copy)
	.var("Name", &Layer::value, HashStringAttribute())
	.ctor<>()
SHIB_REFLECTION_BUILDER_END(Layer)

void Layer::Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, Layer value)
{
	Gaff::Hash32* const component = reinterpret_cast<Gaff::Hash32*>(ecs_mgr.getComponent(query_result, entity_index));
	*component = value.value;
}

void Layer::Set(ECSManager& ecs_mgr, EntityID id, Layer value)
{
	Gaff::Hash32* const component = reinterpret_cast<Gaff::Hash32*>(ecs_mgr.getComponent<Layer>(id)) + ecs_mgr.getPageIndex(id) % 4;
	*component = value.value;
}

Layer Layer::Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index)
{
	const auto* const layer = reinterpret_cast<const Gaff::Hash32*>(ecs_mgr.getComponent(query_result, entity_index));
	return Layer(*layer);
}

Layer Layer::Get(ECSManager& ecs_mgr, EntityID id)
{
	const auto* const layer = reinterpret_cast<const Gaff::Hash32*>(ecs_mgr.getComponent<Layer>(id)) + ecs_mgr.getPageIndex(id) % 4;
	return Layer(*layer);
}

glm_uvec4 Layer::Get(const void* component_begin)
{
	return _mm_load_si128(reinterpret_cast<const glm_uvec4*>(component_begin));
}

void Layer::Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	const Gaff::Hash32* const old_value = reinterpret_cast<const Gaff::Hash32*>(old_begin) + old_index;
	Gaff::Hash32* const new_value = reinterpret_cast<Gaff::Hash32*>(new_begin) + new_index;

	*new_value = *old_value;
}

Layer::Layer(Gaff::Hash32 val):
	value(val)
{
}

NS_END
