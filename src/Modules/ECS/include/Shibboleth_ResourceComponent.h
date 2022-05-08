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

#include "Shibboleth_ECSComponentBase.h"
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_IResource.h>

NS_SHIBBOLETH

template <class T>
class Resource
{
public:
	static_assert(std::is_base_of<IResource, T>::value, "Resource<T>: T must be derived from IResource.");
	using ResourceType = Gaff::RefPtr<T>;

	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const typename ResourceType& value);
	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, typename ResourceType&& value);
	static void SetShared(ECSManager& ecs_mgr, ECSEntityID id, const typename ResourceType& value);
	static void SetShared(ECSManager& ecs_mgr, ECSEntityID id, typename ResourceType&& value);

	static typename ResourceType& GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype);
	static typename ResourceType& GetShared(ECSManager& ecs_mgr, ECSEntityID id);

	static void CopyShared(const void* old_value, void* new_value);

	static constexpr bool IsNonShared(void);
	static constexpr bool IsShared(void);

	typename ResourceType value;
};

NS_END

SHIB_TEMPLATE_REFLECTION_DECLARE(Shibboleth::Resource, T)

#include "Shibboleth_ResourceComponent.inl"
