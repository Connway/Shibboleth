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

#include "Shibboleth_GraphicsResourceComponents.h"
#include <Shibboleth_ECSManager.h>

SHIB_REFLECTION_EXTERNAL_DEFINE(BufferCount)
SHIB_REFLECTION_EXTERNAL_DEFINE(Model)

NS_SHIBBOLETH

SHIB_REFLECTION_BUILDER_BEGIN(BufferCount)
	.classAttrs(
		ECSClassAttribute(nullptr, "Graphics")
	)

	.ctor<>()
SHIB_REFLECTION_BUILDER_END(BufferCount)

void BufferCount::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, int32_t value)
{
	ecs_mgr.getComponentShared<BufferCount>(archetype)->value = value;
}

void BufferCount::SetShared(ECSManager& ecs_mgr, EntityID id, int32_t value)
{
	ecs_mgr.getComponentShared<BufferCount>(id)->value = value;
}

int32_t BufferCount::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return ecs_mgr.getComponentShared<BufferCount>(archetype)->value;
}

int32_t BufferCount::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return ecs_mgr.getComponentShared<BufferCount>(id)->value;
}

void BufferCount::CopyShared(const void* old_value, void* new_value)
{
	reinterpret_cast<BufferCount*>(new_value)->value = reinterpret_cast<const BufferCount*>(old_value)->value;
}


SHIB_REFLECTION_BUILDER_BEGIN(Model)
	.classAttrs(
		ECSClassAttribute(nullptr, "Graphics")
	)

	.base< Resource<ModelResource> >()
	.ctor<>()
SHIB_REFLECTION_BUILDER_END(Model)

NS_END