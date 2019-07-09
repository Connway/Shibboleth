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

#include "Shibboleth_ModelComponent.h"
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ECSAttributes.h>
#include <Shibboleth_ECSManager.h>

SHIB_REFLECTION_EXTERNAL_DEFINE(Model)

NS_SHIBBOLETH

SHIB_REFLECTION_BUILDER_BEGIN(Model)
	.classAttrs(
		ECSClassAttribute(nullptr, "Graphics")//,
		//ECSVarAttribute<ModelResourcePtr>()
	)

	.staticFunc("CopyShared", &Model::CopyShared)
	//.staticFunc("Copy", &Model::Copy)

	.var("Path", &Model::value)
	.ctor<>()
SHIB_REFLECTION_BUILDER_END(Model)

void Model::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const ModelResourcePtr& value)
{
	ecs_mgr.getComponentShared<Model>(archetype)->value = value;
}

void Model::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, ModelResourcePtr&& value)
{
	ecs_mgr.getComponentShared<Model>(archetype)->value = std::move(value);
}

void Model::SetShared(ECSManager& ecs_mgr, EntityID id, const ModelResourcePtr& value)
{
	ecs_mgr.getComponentShared<Model>(id)->value = value;
}

void Model::SetShared(ECSManager& ecs_mgr, EntityID id, ModelResourcePtr&& value)
{
	ecs_mgr.getComponentShared<Model>(id)->value = std::move(value);
}

const ModelResourcePtr& Model::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return ecs_mgr.getComponentShared<Model>(archetype)->value;
}

const ModelResourcePtr& Model::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return ecs_mgr.getComponentShared<Model>(id)->value;
}

void Model::CopyShared(const void* old_value, void* new_value)
{
	reinterpret_cast<Model*>(new_value)->value = reinterpret_cast<const Model*>(old_value)->value;
}

NS_END
