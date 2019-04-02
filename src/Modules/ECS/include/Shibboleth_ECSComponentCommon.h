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

#include "Shibboleth_ECSEntity.h"
#include <Shibboleth_Reflection.h>
#include <Shibboleth_Math.h>
#include <simd/geometric.h>

NS_SHIBBOLETH

struct ECSQueryResult;
class ECSManager;

class Position final
{
public:
	// Slow versions for posterity.
	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const glm::vec3& value);
	static void SetShared(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value);
	static void Set(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index, const glm::vec3& value);
	static void Set(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value);

	static const glm::vec3& GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype);
	static const glm::vec3& GetShared(ECSManager& ecs_mgr, EntityID id);
	static glm::vec3 Get(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index);
	static glm::vec3 Get(ECSManager& ecs_mgr, EntityID id);

	static glm_vec4 GetX(void* component_begin);
	static glm_vec4 GetY(void* component_begin);
	static glm_vec4 GetZ(void* component_begin);

	static void Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);
	static void CopyShared(const void* old_value, void* new_value);

	glm::vec3 value;
};

class Rotation final
{
public:
	// Slow versions for posterity.
	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const glm::quat& value);
	static void SetShared(ECSManager& ecs_mgr, EntityID id, const glm::quat& value);
	static void Set(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index, const glm::quat& value);
	static void Set(ECSManager& ecs_mgr, EntityID id, const glm::quat& value);

	static const glm::quat& GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype);
	static const glm::quat& GetShared(ECSManager& ecs_mgr, EntityID id);
	static glm::quat Get(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index);
	static glm::quat Get(ECSManager& ecs_mgr, EntityID id);

	static glm_vec4 GetX(void* component_begin);
	static glm_vec4 GetY(void* component_begin);
	static glm_vec4 GetZ(void* component_begin);
	static glm_vec4 GetW(void* component_begin);

	static void Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);
	static void CopyShared(const void* old_value, void* new_value);

	glm::quat value;
};

class Scale final
{
public:
	// Slow versions for posterity.
	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const glm::vec3& value);
	static void SetShared(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value);
	static void Set(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index, const glm::vec3& value);
	static void Set(ECSManager& ecs_mgr, EntityID id, const glm::vec3& value);

	static const glm::vec3& GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype);
	static const glm::vec3& GetShared(ECSManager& ecs_mgr, EntityID id);
	static glm::vec3 Get(ECSManager& ecs_mgr, ECSQueryResult& query_result, int32_t entity_index);
	static glm::vec3 Get(ECSManager& ecs_mgr, EntityID id);

	static glm_vec4 GetX(void* component_begin);
	static glm_vec4 GetY(void* component_begin);
	static glm_vec4 GetZ(void* component_begin);

	static void Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);
	static void CopyShared(const void* old_value, void* new_value);

	glm::vec3 value;
};

NS_END

SHIB_REFLECTION_DECLARE(Position)
SHIB_REFLECTION_DECLARE(Rotation)
SHIB_REFLECTION_DECLARE(Scale)
