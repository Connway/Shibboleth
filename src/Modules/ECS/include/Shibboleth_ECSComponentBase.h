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

NS_GAFF
	class ISerializeReader;
NS_END

NS_SHIBBOLETH

class ECSManager;

template <class T>
class ECSComponentBase
{
public:
	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const T& value);
	static void SetShared(ECSManager& ecs_mgr, EntityID id, const T& value);

	static bool Load(ECSManager& ecs_mgr, EntityID id, const Gaff::ISerializeReader& reader);

	static T& GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype);
	static T& GetShared(ECSManager& ecs_mgr, EntityID id);

	static void CopySharedToNonShared(ECSManager& ecs_mgr, EntityID id, const void* shared);
	static void CopyShared(const void* old_value, void* new_value);
};

NS_END

SHIB_TEMPLATE_REFLECTION_DECLARE(ECSComponentBase, T)

#include "Shibboleth_ECSComponentBase.inl"
