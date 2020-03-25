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

#include "Shibboleth_ECSAttributes.h"
#include "Shibboleth_ECSManager.h"
#include "Shibboleth_ECSEntity.h"

NS_GAFF
	class ISerializeReader;
NS_END

NS_SHIBBOLETH

struct ECSQueryResult;
class ECSManager;

enum class ECSComponentType
{
	NonShared,
	Shared,
	Both
};

template <class T, ECSComponentType type = ECSComponentType::Both>
class ECSComponentBase
{
public:
	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const T& value);
	static void SetShared(ECSManager& ecs_mgr, EntityID id, const T& value);
	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, T&& value);
	static void SetShared(ECSManager& ecs_mgr, EntityID id, T&& value);

	static T& GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype);
	static T& GetShared(ECSManager& ecs_mgr, EntityID id);

	static void Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, const T& value);
	static void Set(ECSManager& ecs_mgr, EntityID id, const T& value);

	static T Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index);
	static T Get(ECSManager& ecs_mgr, EntityID id);

	static void CopyDefaultToNonShared(ECSManager& ecs_mgr, EntityID id, const void* shared);
	static void CopyShared(const void* old_value, void* new_value);
	static void Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);

	static bool Load(ECSManager& ecs_mgr, EntityID id, const Gaff::ISerializeReader& reader);

	static constexpr bool IsNonShared(void);
	static constexpr bool IsShared(void);

protected:
	static void CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);
	static void SetInternal(void* component, int32_t page_index, const T& value);
	static T GetInternal(const void* component, int32_t page_index);
};

template <class T>
class ECSComponentBaseNonShared : public ECSComponentBase<T, ECSComponentType::NonShared>
{
};

template <class T>
class ECSComponentBaseShared : public ECSComponentBase<T, ECSComponentType::Shared>
{
};

template <class T>
class ECSComponentBaseBoth : public ECSComponentBase<T, ECSComponentType::Both>
{
};

template <class T>
class ECSComponentWithSingleArg
{
public:
	ECSComponentWithSingleArg(const T& val): value(val) {}
	ECSComponentWithSingleArg(void) = default;

	T value;
};

NS_END

SHIB_TEMPLATE_REFLECTION_DECLARE(ECSComponentBaseNonShared, T)
SHIB_TEMPLATE_REFLECTION_DECLARE(ECSComponentBaseShared, T)
SHIB_TEMPLATE_REFLECTION_DECLARE(ECSComponentBaseBoth, T)

#include "Shibboleth_ECSComponentBase.inl"

#define SHIB_ECS_COMPONENT_REFLECTION(type, name, category) \
	SHIB_REFLECTION_DEFINE_BEGIN(type) \
		.classAttrs( \
			ECSClassAttribute(name, category) \
		) \
		.ctor<>(); \
		if constexpr (type::IsNonShared() && type::IsShared()) { \
			builder \
				.base< ECSComponentBaseBoth<type> >(); \
		} else if constexpr (type::IsNonShared()) { \
			builder \
				.base< ECSComponentBaseNonShared<type> >(); \
		} else if constexpr (type::IsShared()) { \
			builder \
				.base< ECSComponentBaseShared<type> >(); \
		} \
		builder

#define SHIB_ECS_TAG_COMPONENT_DEFINE(type, name, category) \
	SHIB_ECS_COMPONENT_REFLECTION(type, name, category) \
	SHIB_REFLECTION_DEFINE_END(type)

#define SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_BEGIN_WITH_DEFAULT(name, type, base, default_val) \
	class name final : public ECSComponentWithSingleArg<type>, public base<name> \
	{ \
	public: \
		name(const type& val): ECSComponentWithSingleArg(val) {} \
		name(void): ECSComponentWithSingleArg(default_val) {}

#define SHIB_ECS_SINGLE_ARG_COMPONENT_DECLARE_BEGIN(name, type, base) \
	class name final : public ECSComponentWithSingleArg<type>, public base<name> \
	{ \
	public: \
		name(const type& val): ECSComponentWithSingleArg(val) {} \
		name(void) = default;

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
	SHIB_REFLECTION_DEFINE_END(type) \
