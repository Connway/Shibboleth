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

#include "Shibboleth_ECSManager.h"
#include "Shibboleth_ECSAttributes.h"
#include <Shibboleth_IFileSystem.h>
#include <Gaff_Function.h>
#include <Gaff_JSON.h>

SHIB_REFLECTION_DEFINE(ECSManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ECSManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(ECSManager)

bool ECSManager::init(void)
{
	// Load all archetypes from config directory
	auto callback = Gaff::MemberFunc(this, &ECSManager::loadFile);
	IApp& app = GetApp();
	const char* const archetype_dir = app.getConfigs()["archetype_dir"].getString("archetypes");
	app.getFileSystem().forEachFile(archetype_dir, callback);
	return true;
}

void ECSManager::addArchetype(const ECSArchetype& archetype, const char* name)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) == _entity_pages.end());
	EntityData& data = _entity_pages[archetype.getHash()];
	data.num_entities_per_page = (EA_KIBIBYTE(64) - sizeof(EntityPage*)) / archetype.size();
	data.archetype = archetype;

	ProxyAllocator allocator("ECS");
	data.shared_components = SHIB_ALLOC_ALIGNED(archetype.sharedSize(), 16, allocator);

	_archtypes[Gaff::FNV1aHash64String(name)] = archetype.getHash();
}

void ECSManager::addArchetype(ECSArchetype&& archetype, const char* name)
{
	GAFF_ASSERT(_entity_pages.find(archetype.getHash()) == _entity_pages.end());

	EntityData& data = _entity_pages[archetype.getHash()];
	data.num_entities_per_page = (EA_KIBIBYTE(64) - sizeof(EntityPage*)) / archetype.size();
	data.archetype = std::move(archetype);

	ProxyAllocator allocator("ECS");
	data.shared_components = SHIB_ALLOC_ALIGNED(data.archetype.sharedSize(), 16, allocator);

	_archtypes[Gaff::FNV1aHash64String(name)] = archetype.getHash();
}

const ECSArchetype& ECSManager::getArchetype(Gaff::Hash64 name) const
{
	const auto it = _archtypes.find(name);
	GAFF_ASSERT(it && it != _archtypes.end());
	return _entity_pages.find(it->second)->second.archetype;
}

const ECSArchetype& ECSManager::getArchetype(const char* name) const
{
	return getArchetype(Gaff::FNV1aHash64String(name));
}

bool ECSManager::loadFile(const char* file_name, IFile* file)
{
	Gaff::JSON json;

	if (!json.parse(file->getBuffer())) {
		// $TODO: Log error.
		return false;
	}

	const Gaff::JSON shared_components = json["shared_components"];
	const Gaff::JSON components = json["components"];

	ECSArchetype archetype;
	archetype.setSharedName(file_name);

	archetype.fromJSON(json);

	return false;
}

NS_END
