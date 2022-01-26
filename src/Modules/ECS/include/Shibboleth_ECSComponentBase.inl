/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(Shibboleth::ECSComponentBaseNonShared, T, GetT)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(
			Shibboleth::ScriptFlagsAttribute::Flag::NoRegister,
			Shibboleth::ScriptFlagsAttribute::Flag::NoInherit
		)
	)

	.staticFunc("CopyDefaultToNonShared", &Shibboleth::ECSComponentBaseNonShared<T, GetT>::CopyDefaultToNonShared)
	.staticFunc("CopyShared", &Shibboleth::ECSComponentBaseNonShared<T, GetT>::CopyShared)
	.staticFunc("Copy", &Shibboleth::ECSComponentBaseNonShared<T, GetT>::Copy)

	.staticFunc("Load", &Shibboleth::ECSComponentBaseNonShared<T, GetT>::Load)

	.staticFunc("IsNonShared", &Shibboleth::ECSComponentBaseNonShared<T, GetT>::IsNonShared)
	.staticFunc("IsShared", &Shibboleth::ECSComponentBaseNonShared<T, GetT>::IsShared)
SHIB_TEMPLATE_REFLECTION_DEFINE_END(Shibboleth::ECSComponentBaseNonShared, T, GetT)

SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(Shibboleth::ECSComponentBaseShared, T, GetT)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(
			Shibboleth::ScriptFlagsAttribute::Flag::NoRegister,
			Shibboleth::ScriptFlagsAttribute::Flag::NoInherit
		)
	)

	.staticFunc("CopyDefaultToNonShared", &Shibboleth::ECSComponentBaseShared<T, GetT>::CopyDefaultToNonShared)
	.staticFunc("CopyShared", &Shibboleth::ECSComponentBaseShared<T, GetT>::CopyShared)

	.staticFunc("Load", &Shibboleth::ECSComponentBaseShared<T, GetT>::Load)

	.staticFunc("IsNonShared", &Shibboleth::ECSComponentBaseShared<T, GetT>::IsNonShared)
	.staticFunc("IsShared", &Shibboleth::ECSComponentBaseShared<T, GetT>::IsShared)
SHIB_TEMPLATE_REFLECTION_DEFINE_END(Shibboleth::ECSComponentBaseShared, T, GetT)

SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(Shibboleth::ECSComponentBaseBoth, T, GetT)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(
			Shibboleth::ScriptFlagsAttribute::Flag::NoRegister,
			Shibboleth::ScriptFlagsAttribute::Flag::NoInherit
		)
	)

	.staticFunc("CopyDefaultToNonShared", &Shibboleth::ECSComponentBaseBoth<T, GetT>::CopyDefaultToNonShared)
	.staticFunc("CopyShared", &Shibboleth::ECSComponentBaseBoth<T, GetT>::CopyShared)
	.staticFunc("Copy", &Shibboleth::ECSComponentBaseBoth<T, GetT>::Copy)

	.staticFunc("Load", &Shibboleth::ECSComponentBaseBoth<T, GetT>::Load)

	.staticFunc("IsNonShared", &Shibboleth::ECSComponentBaseBoth<T, GetT>::IsNonShared)
	.staticFunc("IsShared", &Shibboleth::ECSComponentBaseBoth<T, GetT>::IsShared)
SHIB_TEMPLATE_REFLECTION_DEFINE_END(Shibboleth::ECSComponentBase, T, GetT)


SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(Shibboleth::ECSComponentDestructable, T)
	.classAttrs(
		Shibboleth::ScriptFlagsAttribute(
			Shibboleth::ScriptFlagsAttribute::Flag::NoRegister,
			Shibboleth::ScriptFlagsAttribute::Flag::NoInherit
		)
	)

	.staticFunc("Constructor", &Shibboleth::ECSComponentDestructable<T>::Constructor)
	.staticFunc("Destructor", &Shibboleth::ECSComponentDestructable<T>::Destructor)
SHIB_TEMPLATE_REFLECTION_DEFINE_END(Shibboleth::ECSComponentDestructable, T)

NS_SHIBBOLETH

template <class T, class GetT, ECSComponentType type>
void ECSComponentBase<T, GetT, type>::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const T& value)
{
	*ecs_mgr.getComponentShared<T>(archetype) = value;
}

template <class T, class GetT, ECSComponentType type>
void ECSComponentBase<T, GetT, type>::SetShared(ECSManager& ecs_mgr, EntityID id, const T& value)
{
	*ecs_mgr.getComponentShared<T>(id) = value;
}

template <class T, class GetT, ECSComponentType type>
void ECSComponentBase<T, GetT, type>::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, T&& value)
{
	*ecs_mgr.getComponentShared<T>(archetype) = std::move(value);
}

template <class T, class GetT, ECSComponentType type>
void ECSComponentBase<T, GetT, type>::SetShared(ECSManager& ecs_mgr, EntityID id, T&& value)
{
	*ecs_mgr.getComponentShared<T>(id) = std::move(value);
}

template <class T, class GetT, ECSComponentType type>
T& ECSComponentBase<T, GetT, type>::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return *ecs_mgr.getComponentShared<T>(archetype);
}

template <class T, class GetT, ECSComponentType type>
T& ECSComponentBase<T, GetT, type>::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return *ecs_mgr.getComponentShared<T>(id);
}

template <class T, class GetT, ECSComponentType type>
template <class Value>
void ECSComponentBase<T, GetT, type>::Set(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index, const Value& value)
{
	void* const component = ecs_mgr.getComponent(query_result, entity_index);
	const int32_t page_index = ecs_mgr.getPageIndex(query_result, entity_index) % 4;

	T::SetInternal(component, page_index, value);
}

template <class T, class GetT, ECSComponentType type>
template <class Value>
void ECSComponentBase<T, GetT, type>::Set(ECSManager& ecs_mgr, EntityID id, const Value& value)
{
	void* const component = ecs_mgr.getComponent<T>(id);
	const int32_t page_index = ecs_mgr.getPageIndex(id) % 4;

	T::SetInternal(component, page_index, value);
}

template <class T, class GetT, ECSComponentType type>
GetT ECSComponentBase<T, GetT, type>::Get(ECSManager& ecs_mgr, const ECSQueryResult& query_result, int32_t entity_index)
{
	const void* const component = ecs_mgr.getComponent(query_result, entity_index);
	const int32_t page_index = ecs_mgr.getPageIndex(query_result, entity_index) % 4;

	return T::GetInternal(component, page_index);
}

template <class T, class GetT, ECSComponentType type>
GetT ECSComponentBase<T, GetT, type>::Get(ECSManager& ecs_mgr, EntityID id)
{
	const void* const component = ecs_mgr.getComponent<T>(id);
	const int32_t page_index = ecs_mgr.getPageIndex(id) % 4;

	return T::GetInternal(component, page_index);
}

template <class T, class GetT, ECSComponentType type>
void ECSComponentBase<T, GetT, type>::CopyDefaultToNonShared(ECSManager& ecs_mgr, EntityID id, const void* shared)
{
	if constexpr (IsNonShared()) {
		T::Set(ecs_mgr, id, *reinterpret_cast<const T*>(shared));
	} else {
		GAFF_ASSERT_MSG(false, "Calling CopyDefaultToNonShared() on an ECS component that does not support non-shared data.");
		GAFF_REF(ecs_mgr, id, shared);
	}
}

template <class T, class GetT, ECSComponentType type>
void ECSComponentBase<T, GetT, type>::CopyShared(const void* old_value, void* new_value)
{
	*reinterpret_cast<T*>(new_value) = *reinterpret_cast<const T*>(old_value);
}

template <class T, class GetT, ECSComponentType type>
void ECSComponentBase<T, GetT, type>::Copy(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	T::CopyInternal(old_begin, old_index, new_begin, new_index);
}

template <class T, class GetT, ECSComponentType type>
bool ECSComponentBase<T, GetT, type>::Load(ECSManager& ecs_mgr, EntityID id, const ISerializeReader& reader)
{
	if constexpr (IsNonShared()) {
		T value;

		if (!Refl::Reflection<T>::GetInstance().load(reader, value)) {
			// $TODO: Log error.
			return false;
		}

		T::Set(ecs_mgr, id, value);
		return true;

	} else {
		GAFF_ASSERT_MSG(false, "Calling Load() on an ECS component that does not support non-shared data.");
		GAFF_REF(ecs_mgr, id, reader);
		return false;
	}
}

template <class T, class GetT, ECSComponentType type>
constexpr bool ECSComponentBase<T, GetT, type>::IsNonShared(void)
{
	return type != ECSComponentType::Shared;
}

template <class T, class GetT, ECSComponentType type>
constexpr bool ECSComponentBase<T, GetT, type>::IsShared(void)
{
	return type != ECSComponentType::NonShared;
}

template <class T, class GetT, ECSComponentType type>
void ECSComponentBase<T, GetT, type>::CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index)
{
	GAFF_REF(old_begin, old_index, new_begin, new_index);
	GAFF_ASSERT_MSG(IsNonShared(), "CopyInternal() called on a component that doesn't support non-shared behavior.");
	GAFF_ASSERT_MSG(false, "CopyInternal() not implemented with type '%s'.", Refl::Reflection<T>::GetName());
}

template <class T, class GetT, ECSComponentType type>
void ECSComponentBase<T, GetT, type>::SetInternal(void* component, int32_t page_index, const T& value)
{
	GAFF_REF(component, page_index, value);
	GAFF_ASSERT_MSG(IsNonShared(), "SetInternal() called on a component that doesn't support non-shared behavior.");
	GAFF_ASSERT_MSG(false, "SetInternal() not implemented with type '%s'.", Refl::Reflection<T>::GetName());
}

template <class T, class GetT, ECSComponentType type>
GetT ECSComponentBase<T, GetT, type>::GetInternal(const void* component, int32_t page_index)
{
	GAFF_REF(component, page_index);
	GAFF_ASSERT_MSG(IsNonShared(), "GetInternal() called on a component that doesn't support non-shared behavior.");
	GAFF_ASSERT_MSG(false, "GetInternal() not implemented with type '%s'.", Refl::Reflection<T>::GetName());
}



template <class T>
void ECSComponentDestructable<T>::Constructor(void* component, int32_t entity_index)
{
	new(reinterpret_cast<T*>(component) + entity_index) T();
}

template <class T>
void ECSComponentDestructable<T>::Destructor(void* component, int32_t entity_index)
{
	(reinterpret_cast<T*>(component) + entity_index)->~T();
}

NS_END
