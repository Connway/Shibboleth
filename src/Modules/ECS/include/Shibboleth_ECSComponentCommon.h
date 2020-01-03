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

#pragma once

#include "Shibboleth_ECSComponentBase.h"
#include <Shibboleth_Math.h>

NS_SHIBBOLETH

struct ECSQueryResult;

class Position final : public ECSComponentBaseBoth<Position>
{
public:
	static void Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, const Position& value);
	static void Set(ECSManager& ecs_mgr, EntityID id, const Position& value);

	static Position Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index);
	static Position Get(ECSManager& ecs_mgr, EntityID id);

	static glm_vec4 GetX(const void* component_begin);
	static glm_vec4 GetY(const void* component_begin);
	static glm_vec4 GetZ(const void* component_begin);

	static void Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);

	Position(const glm::vec3& val);
	Position(void) = default;

	glm::vec3 value = glm::zero<glm::vec3>();
};

// $TODO: Make euler angles.
class Rotation final : public ECSComponentBaseBoth<Rotation>
{
public:
	static void Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, const Rotation& value);
	static void Set(ECSManager& ecs_mgr, EntityID id, const Rotation& value);

	static Rotation Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index);
	static Rotation Get(ECSManager& ecs_mgr, EntityID id);

	static glm_vec4 GetX(const void* component_begin);
	static glm_vec4 GetY(const void* component_begin);
	static glm_vec4 GetZ(const void* component_begin);
	static glm_vec4 GetW(const void* component_begin);

	static void Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);

	Rotation(const glm::quat& val);
	Rotation(void) = default;

	glm::quat value = glm::identity<glm::quat>();
};

class Scale final : public ECSComponentBaseBoth<Scale>
{
public:
	static void Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, const Scale& value);
	static void Set(ECSManager& ecs_mgr, EntityID id, const Scale& value);

	static Scale Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index);
	static Scale Get(ECSManager& ecs_mgr, EntityID id);

	static glm_vec4 GetX(const void* component_begin);
	static glm_vec4 GetY(const void* component_begin);
	static glm_vec4 GetZ(const void* component_begin);

	static void Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);

	Scale(const glm::vec3& val);
	Scale(void) = default;

	glm::vec3 value = glm::one<glm::vec3>();
};

class Layer final : public ECSComponentBaseShared<Layer>
{
public:
	//static void Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, Layer value);
	//static void Set(ECSManager& ecs_mgr, EntityID id, Layer value);

	//static Layer Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index);
	//static Layer Get(ECSManager& ecs_mgr, EntityID id);

	//static glm_uvec4 Get(const void* component_begin);

	//static void Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);

	Layer(Gaff::Hash32 val);
	Layer(void) = default;

	Gaff::Hash32 value;
};

NS_END

SHIB_REFLECTION_DECLARE(Position)
SHIB_REFLECTION_DECLARE(Rotation)
SHIB_REFLECTION_DECLARE(Scale)
SHIB_REFLECTION_DECLARE(Layer)
