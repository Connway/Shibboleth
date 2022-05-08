/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Shibboleth_ECSAttributes.h"
#include "Shibboleth_ECSManager.h"
#include "Shibboleth_ECSEntity.h"
#include <Shibboleth_EngineAttributesCommon.h>

NS_SHIBBOLETH

struct ECSQueryResult;
class ECSManager;

enum class ECSComponentType
{
	NonShared,
	Shared,
	Both
};

template <class T, class GetT, ECSComponentType type = ECSComponentType::Both>
class ECSComponentBase
{
public:
	using GetType = GetT;

	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const T& value);
	static void SetShared(ECSManager& ecs_mgr, ECSEntityID id, const T& value);
	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, T&& value);
	static void SetShared(ECSManager& ecs_mgr, ECSEntityID id, T&& value);

	static T& GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype);
	static T& GetShared(ECSManager& ecs_mgr, ECSEntityID id);

	template <class Value>
	static void Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, const Value& value);

	template <class Value>
	static void Set(ECSManager& ecs_mgr, ECSEntityID id, const Value& value);

	static GetT Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index);
	static GetT Get(ECSManager& ecs_mgr, ECSEntityID id);

	static void CopyDefaultToNonShared(ECSManager& ecs_mgr, ECSEntityID id, const void* shared);
	static void CopyShared(const void* old_value, void* new_value);
	static void Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);

	static bool Load(ECSManager& ecs_mgr, ECSEntityID id, const ISerializeReader& reader);

	static constexpr bool IsNonShared(void);
	static constexpr bool IsShared(void);

protected:
	static void CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);
	static void SetInternal(void* component, int32_t page_index, const T& value);
	static GetT GetInternal(const void* component, int32_t page_index);
};

template <class T, class GetT = T>
class ECSComponentBaseNonShared : public ECSComponentBase<T, GetT, ECSComponentType::NonShared>
{
};

template <class T, class GetT = T>
class ECSComponentBaseShared : public ECSComponentBase<T, GetT, ECSComponentType::Shared>
{
};

template <class T, class GetT = T>
class ECSComponentBaseBoth : public ECSComponentBase<T, GetT, ECSComponentType::Both>
{
};

// Only supports single argument components.
template <class T>
class ECSComponentDestructable
{
public:
	static void Constructor(void* component, int32_t entity_index);
	static void Destructor(void* component, int32_t entity_index);
};

NS_END

SHIB_TEMPLATE_REFLECTION_DECLARE(Shibboleth::ECSComponentBaseNonShared, T, GetT)
SHIB_TEMPLATE_REFLECTION_DECLARE(Shibboleth::ECSComponentBaseShared, T, GetT)
SHIB_TEMPLATE_REFLECTION_DECLARE(Shibboleth::ECSComponentBaseBoth, T, GetT)

SHIB_TEMPLATE_REFLECTION_DECLARE(Shibboleth::ECSComponentDestructable, T)

#include "Shibboleth_ECSComponentBase.inl"

#define SHIB_ECS_COMPONENT_REFLECTION(type, name, category) \
	SHIB_REFLECTION_DEFINE_BEGIN(type) \
		.classAttrs( \
			Shibboleth::ECSClassAttribute(name, category) \
		) \
		.template ctor<>() \
		.staticFunc("IsNonShared", &type::IsNonShared) \
		.staticFunc("IsShared", &type::IsShared); \
		if constexpr (type::IsNonShared() && type::IsShared()) { \
			builder \
				.template base< Shibboleth::ECSComponentBaseBoth<type> >(); \
		} else if constexpr (type::IsNonShared()) { \
			builder \
				.template base< Shibboleth::ECSComponentBaseNonShared<type> >(); \
		} else if constexpr (type::IsShared()) { \
			builder \
				.template base< Shibboleth::ECSComponentBaseShared<type> >(); \
		} \
		builder

#define SHIB_ECS_TAG_COMPONENT_DEFINE(type, name, category) \
	SHIB_ECS_COMPONENT_REFLECTION(type, name, category) \
	SHIB_REFLECTION_DEFINE_END(type)

#define SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_BEGIN_WITH_DEFAULT(name, type, base, default_value) \
	class name final : public base<name> \
	{ \
	public: \
		name(const type& val): value(val) {} \
		name(void) = default; \
		type value = default_value;

#define SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_BEGIN(name, type, base) \
	class name final : public base<name> \
	{ \
	public: \
		name(const type& val): value(val) {} \
		name(void) = default; \
		type value;

#define SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_END(name) \
	}; \

#define SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_WITH_DEFAULT(name, type, base, default_val) \
	SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_BEGIN_WITH_DEFAULT(name, type, base, default_val) \
	SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_END(name)

#define SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE(name, type, base) \
	SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_BEGIN(name, type, base) \
	SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_END(name)

#define SHIB_ECS_SINGLE_ARG_COMPONENT_DEFINE(type, name, category, ...) \
	SHIB_ECS_COMPONENT_REFLECTION(type, name, category) \
		.var("value", &type::value, ##__VA_ARGS__); \
		if constexpr (type::IsNonShared()) { \
			builder \
				.staticFunc("Get", static_cast<typename type::GetType (*)(Shibboleth::ECSManager&, Shibboleth::ECSEntityID)>(&type::Get)) \
				.staticFunc("Set", static_cast<void (*)(Shibboleth::ECSManager&, Shibboleth::ECSEntityID, const type&)>(&type::Set)); \
		} \
		if constexpr (type::IsShared()) { \
			builder \
				.staticFunc("GetShared", static_cast<type& (*)(Shibboleth::ECSManager&, Shibboleth::ECSEntityID)>(&type::GetShared)) \
				.staticFunc("SetShared", static_cast<void (*)(Shibboleth::ECSManager&, Shibboleth::ECSEntityID, const type&)>(&type::SetShared)); \
		} \
	SHIB_REFLECTION_DEFINE_END(type)
