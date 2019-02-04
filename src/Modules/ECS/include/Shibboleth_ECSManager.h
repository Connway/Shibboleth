/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

class ECSManager final : public IManager
{
public:
	template <class T>
	void* getComponent(EntityID id)
	{
		return getComponent(id, Reflection<T>::GetHash());
	}

	bool init(void) override;

	//void addArchetype(const ECSArchetype& archetype, const char* name);
	void addArchetype(ECSArchetype&& archetype, const char* name);
	const ECSArchetype& getArchetype(Gaff::Hash64 name) const;
	const ECSArchetype& getArchetype(const char* name) const;

	EntityID createEntityByName(Gaff::Hash64 name);
	EntityID createEntityByName(const char* name);

	EntityID createEntity(const ECSArchetype& archetype);
	EntityID createEntity(Gaff::Hash64 archetype);

	void* getComponent(EntityID id, Gaff::Hash64 component);

private:
	//struct alignas(16) EntityPage final
	//{
	//	int8_t data[EA_KIBIBYTE(64) - sizeof(EntityPage*)]; // 64KiB
	//	EntityPage* next_page = nullptr;
	//};

	struct EntityData;

	struct EntityPage final
	{
		int32_t num_entities = 0;
		int32_t next_index = 0;
		UniquePtr<void> data;

		Vector<int32_t> free_indices;
		EntityData* owner = nullptr;
	};

	struct EntityData final
	{
		ECSArchetype archetype;

		int32_t num_entities_per_page = 0;
		int32_t num_entities = 0;

		Vector< UniquePtr<EntityPage> > pages;
	};

	VectorMap< Gaff::Hash64, UniquePtr<EntityData> > _entity_pages;
	VectorMap<Gaff::Hash64, Gaff::Hash64> _archtypes;

	bool loadFile(const char* file_name, IFile* file);

	SHIB_REFLECTION_CLASS_DECLARE(ECSManager);
};

NS_END

SHIB_REFLECTION_DECLARE(ECSManager)
