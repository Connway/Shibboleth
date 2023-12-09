/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include <Shibboleth_EntityResource.h>

NS_SHIBBOLETH

class LayerEntityData final : public Refl::IReflectionObject
{
public:

private:
	static bool Load(const ISerializeReader& reader, LayerEntityData& instance);
	static void Save(ISerializeWriter& writer, const LayerEntityData& instance);

	EntityResourcePtr _entity_resource;
	// Modifier operations

	SHIB_REFLECTION_CLASS_DECLARE(LayerEntityData);
};

class LayerResource final : public IResource
{
public:
	static constexpr bool Creatable = true;

	LayerResource(void);
	~LayerResource(void) override;

	void load(const ISerializeReader& reader, uintptr_t thread_id_int) override;

private:
	Vector< ResourcePtr<EntityResource> > _entity_resources;
	Vector<Entity*> _entities;
	//ResourceCallbackID _callback_id;

	//bool loadOverrides(
	//	const ISerializeReader& reader,
	//	ECSManager& ecs_mgr,
	//	const ECSArchetype& base_archetype,
	//	Gaff::Hash32 layer_name,
	//	Gaff::Hash32 scene_name,
	//	Gaff::Hash64& outArchetype
	//);

	//void archetypeLoaded(const Vector<IResource*>&);

	SHIB_REFLECTION_CLASS_DECLARE(LayerResource);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::LayerEntityData)
SHIB_REFLECTION_DECLARE(Shibboleth::LayerResource)
