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

#include "Shibboleth_ModelResource.h"
#include <Shibboleth_ResourceComponent.h>

NS_SHIBBOLETH

// $TODO: Make it easier for making components that have a single value class/type in them.
class BufferCount final
{
public:
	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, int32_t value);
	static void SetShared(ECSManager& ecs_mgr, EntityID id, int32_t value);

	static int32_t GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype);
	static int32_t GetShared(ECSManager& ecs_mgr, EntityID id);

	static void CopyShared(const void* old_value, void* new_value);

	int32_t value;
};

class Model final : public Resource<ModelResource>
{
};

NS_END

SHIB_REFLECTION_DECLARE(BufferCount)
SHIB_REFLECTION_DECLARE(Model)