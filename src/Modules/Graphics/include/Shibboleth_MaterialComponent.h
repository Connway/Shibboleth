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

#include "Shibboleth_MaterialResource.h"
#include "Shibboleth_TextureResource.h"
#include <Shibboleth_ECSEntity.h>

NS_SHIBBOLETH

class ECSManager;

class Material final
{
public:
	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, const Material& value);
	static void SetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype, Material&& value);
	static void SetShared(ECSManager& ecs_mgr, EntityID id, const Material& value);
	static void SetShared(ECSManager& ecs_mgr, EntityID id, Material&& value);

	static const Material& GetShared(ECSManager& ecs_mgr, Gaff::Hash64 archetype);
	static const Material& GetShared(ECSManager& ecs_mgr, EntityID id);

	static void CopyShared(const void* old_value, void* new_value);

	MaterialResourcePtr material;

	VectorMap<U8String, TextureResourcePtr> textures_vertex;
	VectorMap<U8String, TextureResourcePtr> textures_pixel;
	VectorMap<U8String, TextureResourcePtr> textures_domain;
	VectorMap<U8String, TextureResourcePtr> textures_geometry;
	VectorMap<U8String, TextureResourcePtr> textures_hull;
};

NS_END

SHIB_REFLECTION_DECLARE(Material)