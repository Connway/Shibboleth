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

SHIB_TEMPLATE_REFLECTION_EXTERNAL_DEFINE(ECSComponentBaseNonShared, T)
SHIB_TEMPLATE_REFLECTION_EXTERNAL_DEFINE(ECSComponentBaseShared, T)
SHIB_TEMPLATE_REFLECTION_EXTERNAL_DEFINE(ECSComponentBaseBoth, T)

NS_SHIBBOLETH

SHIB_TEMPLATE_REFLECTION_BUILDER_BEGIN(ECSComponentBaseNonShared, T)
	.staticFunc("CopyDefaultToNonShared", &ECSComponentBaseNonShared<T>::CopyDefaultToNonShared)
	.staticFunc("CopyShared", &ECSComponentBaseNonShared<T>::CopyShared)

	.staticFunc("Load", &ECSComponentBaseNonShared<T>::Load)
	//.staticFunc("Copy", &ECSComponentBaseNonShared<T>::Copy)

	.staticFunc("IsNonShared", &ECSComponentBaseNonShared<T>::IsNonShared)
	.staticFunc("IsShared", &ECSComponentBaseNonShared<T>::IsShared)
SHIB_TEMPLATE_REFLECTION_BUILDER_END(ECSComponentBaseNonShared, T)

SHIB_TEMPLATE_REFLECTION_BUILDER_BEGIN(ECSComponentBaseShared, T)
	.staticFunc("CopyDefaultToNonShared", &ECSComponentBaseShared<T>::CopyDefaultToNonShared)
	.staticFunc("CopyShared", &ECSComponentBaseShared<T>::CopyShared)

	.staticFunc("Load", &ECSComponentBaseShared<T>::Load)
	//.staticFunc("Copy", &ECSComponentBaseShared<T>::Copy)

	.staticFunc("IsNonShared", &ECSComponentBaseShared<T>::IsNonShared)
	.staticFunc("IsShared", &ECSComponentBaseShared<T>::IsShared)
SHIB_TEMPLATE_REFLECTION_BUILDER_END(ECSComponentBaseShared, T)

SHIB_TEMPLATE_REFLECTION_BUILDER_BEGIN(ECSComponentBaseBoth, T)
	.staticFunc("CopyDefaultToNonShared", &ECSComponentBaseBoth<T>::CopyDefaultToNonShared)
	.staticFunc("CopyShared", &ECSComponentBaseBoth<T>::CopyShared)

	.staticFunc("Load", &ECSComponentBaseBoth<T>::Load)
	//.staticFunc("Copy", &ECSComponentBaseBoth<T>::Copy)

	.staticFunc("IsNonShared", &ECSComponentBase<T>::IsNonShared)
	.staticFunc("IsShared", &ECSComponentBase<T>::IsShared)
SHIB_TEMPLATE_REFLECTION_BUILDER_END(ECSComponentBase, T)

template <class T, ECSComponentType type>
void ECSComponentBase<T, type>::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const T& value)
{
	*ecs_mgr.getComponentShared<T>(archetype) = value;
}

template <class T, ECSComponentType type>
void ECSComponentBase<T, type>::SetShared(ECSManager& ecs_mgr, EntityID id, const T& value)
{
	*ecs_mgr.getComponentShared<T>(id) = value;
}

template <class T, ECSComponentType type>
void ECSComponentBase<T, type>::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, T&& value)
{
	*ecs_mgr.getComponentShared<T>(archetype) = std::move(value);
}

template <class T, ECSComponentType type>
void ECSComponentBase<T, type>::SetShared(ECSManager& ecs_mgr, EntityID id, T&& value)
{
	*ecs_mgr.getComponentShared<T>(id) = std::move(value);
}

template <class T, ECSComponentType type>
T& ECSComponentBase<T, type>::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return *ecs_mgr.getComponentShared<T>(archetype);
}

template <class T, ECSComponentType type>
T& ECSComponentBase<T, type>::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return *ecs_mgr.getComponentShared<T>(archetype);
}

template <class T, ECSComponentType type>
void ECSComponentBase<T, type>::CopyDefaultToNonShared(ECSManager& ecs_mgr, EntityID id, const void* shared)
{
	if constexpr (IsNonShared()) {
		T::Set(ecs_mgr, id, *reinterpret_cast<const T*>(shared));
	} else {
		GAFF_ASSERT_MSG(false, "Calling CopyDefaultToNonShared() on an ECS component that does not support non-shared data.");
		GAFF_REF(ecs_mgr, id, shared);
	}
}

template <class T, ECSComponentType type>
void ECSComponentBase<T, type>::CopyShared(const void* old_value, void* new_value)
{
	*reinterpret_cast<T*>(new_value) = *reinterpret_cast<const T*>(old_value);
}

template <class T, ECSComponentType type>
bool ECSComponentBase<T, type>::Load(ECSManager& ecs_mgr, EntityID id, const Gaff::ISerializeReader& reader)
{
	if constexpr (IsNonShared()) {
		T value;

		if (!Reflection<T>::Load(reader, value)) {
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

template <class T, ECSComponentType type>
constexpr bool ECSComponentBase<T, type>::IsNonShared(void)
{
	return type != ECSComponentType::Shared;
}

template <class T, ECSComponentType type>
constexpr bool ECSComponentBase<T, type>::IsShared(void)
{
	return type != ECSComponentType::NonShared;
}

NS_END
