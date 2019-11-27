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

#include "Shibboleth_MaterialComponent.h"
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_ECSAttributes.h>
#include <Shibboleth_ECSManager.h>

SHIB_REFLECTION_EXTERNAL_DEFINE(Material)

NS_SHIBBOLETH

SHIB_REFLECTION_BUILDER_BEGIN(Material)
	.classAttrs(
		ECSClassAttribute(nullptr, "Graphics")
	)

	.staticFunc("CopyShared", &Material::CopyShared)

	.var("material", &Material::material)

	.var("textures_vertex", &Material::textures_vertex)
	.var("textures_pixel", &Material::textures_pixel)
	.var("textures_domain", &Material::textures_domain)
	.var("textures_geometry", &Material::textures_geometry)
	.var("textures_hull", &Material::textures_hull)
	.ctor<>()
SHIB_REFLECTION_BUILDER_END(Material)

void Material::SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const Material& value)
{
	*ecs_mgr.getComponentShared<Material>(archetype) = value;
}

void Material::SetShared(ECSManager& ecs_mgr, EntityID id, const Material& value)
{
	*ecs_mgr.getComponentShared<Material>(id) = value;
}

const Material& Material::GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype)
{
	return *ecs_mgr.getComponentShared<Material>(archetype);
}

const Material& Material::GetShared(ECSManager& ecs_mgr, EntityID id)
{
	return *ecs_mgr.getComponentShared<Material>(id);
}

void Material::CopyShared(const void* old_value, void* new_value)
{
	*reinterpret_cast<Material*>(new_value) = *reinterpret_cast<const Material*>(old_value);
}

NS_END
