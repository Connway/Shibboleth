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

SHIB_TEMPLATE_REFLECTION_EXTERNAL_DEFINE(ECSComponentBase, T)

NS_SHIBBOLETH

SHIB_TEMPLATE_REFLECTION_BUILDER_BEGIN(ECSComponentBase, T)
	.staticFunc("CopySharedToNonShared", &ECSComponentBase<T>::CopySharedToNonShared)
	.staticFunc("CopyShared", &ECSComponentBase<T>::CopyShared)
	//.staticFunc("Copy", &ECSComponentBase<T>::Copy)

	.staticFunc("Load", &ECSComponentBase<T>::Load)

	.ctor<>()
SHIB_TEMPLATE_REFLECTION_BUILDER_END(ECSComponentBase, T)

template <class T>
void ECSComponentBase<T>::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const T& value)
{
	*ecs_mgr.getComponentShared<T>(archetype) = value;
}

template <class T>
void ECSComponentBase<T>::SetShared(ECSManager& ecs_mgr, EntityID id, const T& value)
{
	*ecs_mgr.getComponentShared<T>(id) = value;
}

template <class T>
bool ECSComponentBase<T>::Load(ECSManager& ecs_mgr, EntityID id, const Gaff::ISerializeReader& reader)
{
	T value;

	if (!Reflection<T>::Load(reader, value)) {
		// $TODO: Log error.
		return false;
	}

	T::Set(ecs_mgr, id, value);
	return true;
}

template <class T>
T& ECSComponentBase<T>::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return *ecs_mgr.getComponentShared<T>(archetype);
}

template <class T>
T& ECSComponentBase<T>::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return *ecs_mgr.getComponentShared<T>(archetype);
}

template <class T>
void ECSComponentBase<T>::CopySharedToNonShared(ECSManager& ecs_mgr, EntityID id, const void* shared)
{
	T::Set(ecs_mgr, id, *reinterpret_cast<const T*>(shared));
}

template <class T>
void ECSComponentBase<T>::CopyShared(const void* old_value, void* new_value)
{
	*reinterpret_cast<T*>(new_value) = *reinterpret_cast<const T*>(old_value);
}

NS_END
