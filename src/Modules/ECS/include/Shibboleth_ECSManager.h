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

#include "Shibboleth_ECSArchetype.h"
#include "Shibboleth_ECSEntity.h"
#include <Shibboleth_Reflection.h>
#include <Shibboleth_IManager.h>

NS_SHIBBOLETH

class IFile;

template <class... Components>
void RemoveSharedComponentHelper(ECSArchetype& archetype)
{
	archetype.removeShared<Components>()...;
}

template <class... Components>
void RemoveComponentHelper(ECSArchetype& archetype)
{
	archetype.remove<Components>()...;
}

template <class... Components>
void AddSharedComponentHelper(ECSArchetype& archetype)
{
	archetype.addShared<Components>()...;
}

template <class... Components>
void AddComponentHelper(ECSArchetype& archetype)
{
	archetype.add<Components>()...;
}


class ECSManager final : public IManager
{
public:
	struct ArchetypeModifier final
	{
		void (*removeShared)(ECSArchetype&);
		void (*addShared)(ECSArchetype&);
		void (*remove)(ECSArchetype&);
		void (*add)(ECSArchetype&);
	};

	template <class T>
	void* getComponent(EntityID id)
	{
		return getComponent(id, Reflection<T>::GetHash());
	}

	template <class... Components>
	void removeSharedComponents(EntityID id)
	{
		ECSArchetype archetype;
		archetype.copy(getArchetype(id));
		archetype.removeShared<Components>()...;
		archetype.finalize();

		addArchetype(std::move(archetype));

		// migrate entity.
	}

	template <class... Components>
	void removeComponents(EntityID id)
	{
		ECSArchetype archetype;
		archetype.copy(getArchetype(id));
		archetype.remove<Components>()...;
		archetype.finalize();

		addArchetype(std::move(archetype));

		// migrate entity.
	}

	template <class... Components>
	void addSharedComponents(EntityID id)
	{
		ECSArchetype archetype;
		archetype.copy(getArchetype(id));
		archetype.addShared<Components>()...;
		archetype.finalize();

		addArchetype(std::move(archetype));

		// migrate entity.
	}

	template <class... Components>
	void addComponents(EntityID id)
	{
		ECSArchetype archetype;
		archetype.copy(getArchetype(id));
		archetype.add<Components>()...;
		archetype.finalize();

		addArchetype(std::move(archetype));

		// migrate entity.
	}

	void modify(EntityID& id, ArchetypeModifier modifier)
	{
		ECSArchetype archetype;
		archetype.copy(getArchetype(id));

		if (modifier.removeShared) {
			modifier.removeShared(archetype);
		}

		if (modifier.remove) {
			modifier.remove(archetype);
		}

		if (modifier.addShared) {
			modifier.addShared(archetype);
		}

		if (modifier.add) {
			modifier.add(archetype);
		}

		archetype.finalize();
		addArchetype(std::move(archetype));

		//migrate(id, old_archetype, archetype)
	}

	bool init(void) override;

	void addArchetype(ECSArchetype&& archetype, const char* name);
	void addArchetype(ECSArchetype&& archetype);

	const ECSArchetype& getArchetype(Gaff::Hash64 name) const;
	const ECSArchetype& getArchetype(const char* name) const;
	const ECSArchetype& getArchetype(EntityID id) const;

	EntityID createEntityByName(Gaff::Hash64 name);
	EntityID createEntityByName(const char* name);

	EntityID createEntity(const ECSArchetype& archetype);
	EntityID createEntity(Gaff::Hash64 archetype);

	void destroyEntity(EntityID id);

	void* getComponent(EntityID id, Gaff::Hash64 component);
	int32_t getPageIndex(EntityID id) const;

private:
	struct EntityData;

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable : 4324)
#endif

	struct alignas(16) EntityPage final
	{
		int32_t num_entities;
		int32_t next_index;
		// Data after this.
	};

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif

	struct EntityData final
	{
		ECSArchetype archetype;

		int32_t num_entities_per_page = 0;
		int32_t num_entities = 0;

		Vector< UniquePtr<EntityPage> > pages;
		Vector<int32_t> free_indices;
	};

	struct Entity final
	{
		EntityData* data = nullptr;
		EntityPage* page = nullptr;
		int32_t index = -1;
	};

	VectorMap< Gaff::Hash64, UniquePtr<EntityData> > _entity_pages;
	VectorMap<Gaff::Hash64, Gaff::Hash64> _archtypes;

	Vector<Entity> _entities;
	Vector<int32_t> _free_ids;
	int32_t _next_id = 0;

	bool loadFile(const char* file_name, IFile* file);

	SHIB_REFLECTION_CLASS_DECLARE(ECSManager);
};

NS_END

SHIB_REFLECTION_DECLARE(ECSManager)
