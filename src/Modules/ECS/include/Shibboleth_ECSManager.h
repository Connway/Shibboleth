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

#include <Shibboleth_ECSArchetype.h>
#include <Shibboleth_Reflection.h>
#include <Shibboleth_IManager.h>

NS_SHIBBOLETH

class ECSManager final : public IManager
{
public:
	bool init(void) override;

	void addArchetype(const ECSArchetype& archetype, const char* name);
	void addArchetype(ECSArchetype&& archetype, const char* name);
	const ECSArchetype& getArchetype(Gaff::Hash64 name) const;
	const ECSArchetype& getArchetype(const char* name) const;

private:
	struct alignas(16) EntityPage
	{
		EntityPage* next_page = nullptr;
		uint8_t data[EA_KIBIBYTE(64) - sizeof(EntityPage*)]; // 64KiB
	};

	VectorMap<Gaff::Hash64, ECSArchetype> _archetypes;
	VectorMap<Gaff::Hash64, EntityPage*> _entity_pages; // Key: Archetype. Key of * is entities without an archetype.
	VectorMap< Gaff::Hash64, Vector<int32_t> > _component_map;
	Vector<void*> _entities;

	SHIB_REFLECTION_CLASS_DECLARE(ECSManager);
};

NS_END

SHIB_REFLECTION_DECLARE(ECSManager)
