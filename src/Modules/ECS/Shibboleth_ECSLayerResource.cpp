/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Shibboleth_ECSLayerResource.h"
#include "Shibboleth_ECSComponentCommon.h"
#include "Shibboleth_ECSManager.h"
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ResourceLogging.h>
#include <Shibboleth_Utilities.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ECSLayerResource)
	.classAttrs(
		Shibboleth::ResExtAttribute(u8".layer.bin"),
		Shibboleth::ResExtAttribute(u8".layer"),
		Shibboleth::MakeLoadFileCallbackAttribute(&Shibboleth::ECSLayerResource::loadLayer)
	)

	.base<Shibboleth::IResource>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::ECSLayerResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ECSLayerResource)

ECSLayerResource::ECSLayerResource(void):
	_reader_wrapper(ProxyAllocator("Resource"))
{
}

ECSLayerResource::~ECSLayerResource(void)
{
	if (_callback_id.cb_id > -1) {
		ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();
		res_mgr.removeCallback(_callback_id);
	}
}

bool ECSLayerResource::loadOverrides(
	const ISerializeReader& reader,
	ECSManager& ecs_mgr,
	const ECSArchetype& base_archetype,
	Gaff::Hash32 layer_name,
	Gaff::Hash32 scene_name,
	Gaff::Hash64& outArchetype)
{
	if (reader.isNull()) {
		outArchetype = base_archetype.getHash();
		return true;
	}

	if (!reader.isObject()) {
		LogErrorResource("ECSLayerResource - 'overrides' is not an object.");
		return false;
	}

	{
		const auto guard = reader.enterElementGuard(u8"shared_components");

		if (!reader.isNull() && !reader.isObject()) {
			LogErrorResource("ECSLayerResource - 'overrides:shared_components' is not an object.");
			return false;
		}
	}

	{
		const auto guard = reader.enterElementGuard(u8"components");

		if (!reader.isNull() && !reader.isObject()) {
			LogErrorResource("ECSLayerResource - 'overrides:components' is not an object.");
			return false;
		}
	}

	ECSArchetype new_archetype;
	new_archetype.copy(base_archetype);

	new_archetype.addShared<Layer>(); // Ensure we have a Layer component.
	new_archetype.addShared<Scene>(); // Ensure we have a Scene component.

	const bool success = new_archetype.finalize(reader, base_archetype, false);

	if (!success) {
		return false;
	}

	Layer* const layer = new_archetype.getSharedComponent<Layer>();
	layer->value = layer_name;

	Scene* const scene = new_archetype.getSharedComponent<Scene>();
	scene->value = scene_name;

	new_archetype.calculateHash();
	outArchetype = new_archetype.getHash();

	ecs_mgr.addArchetype(std::move(new_archetype), _archetype_refs.emplace_back());

	return true;
}

void ECSLayerResource::archetypeLoaded(const Vector<IResource*>&)
{
	_callback_id.res_id = Gaff::k_init_hash64;
	_callback_id.cb_id = -1;

	Gaff::Hash32 layer_name;
	Gaff::Hash32 scene_name;
	int32_t index = 0;

	const auto& reader = *_reader_wrapper.getReader();

	{
		const auto name_guard = reader.enterElementGuard(u8"layer");

		if (!reader.isNull() && !reader.isString()) {
			LogErrorResource("ECSLayerResource - 'layer' field is not a string.");
		}

		char8_t name_temp[256] = { 0 };
		reader.readString(name_temp, sizeof(name_temp), u8"<default>");
		layer_name = Gaff::FNV1aHash32String(name_temp);
	}

	{
		const auto name_guard = reader.enterElementGuard(u8"scene");

		if (!reader.isNull() && !reader.isString()) {
			LogErrorResource("ECSLayerResource - 'scene' field is not a string.");
		}

		char8_t name_temp[256] = { 0 };
		reader.readString(name_temp, sizeof(name_temp), u8"main");
		scene_name = Gaff::FNV1aHash32String(name_temp);
	}

	const auto objects_guard = reader.enterElementGuard(u8"objects");
	ECSManager& ecs_mgr = GetApp().getManagerTFast<ECSManager>();

	for (const auto& arch_res : _archetypes) {
		if (!arch_res->isLoaded()) {
			continue;
		}

		const auto element_guard = reader.enterElementGuard(index);
		const auto override_guard = reader.enterElementGuard(u8"overrides");
		Gaff::Hash64 archetype(0);

		if (loadOverrides(reader, ecs_mgr, arch_res->getArchetype(), layer_name, scene_name, archetype)) {
			const auto comps_guard = reader.enterElementGuard(u8"components");

			const EntityID id = (reader.isNull()) ? ecs_mgr.createEntity(archetype) : ecs_mgr.loadEntity(archetype, reader);

			if (id == EntityID_None) {
				LogErrorResource("ECSLayerResource - Failed to create entity for object at index %i.", index);
			}

		} else {
			LogErrorResource("ECSLayerResource - Failed to load archetype overrides for object at index %i.", index);
		}

		++index;
	}

	// Always mark succeeded, even if an object failed to load.
	succeeded();
}

void ECSLayerResource::loadLayer(IFile* file, uintptr_t /*thread_id_int*/)
{	
	if (!OpenJSONOrMPackFile(_reader_wrapper, getFilePath().getBuffer(), file, true)) {
		LogErrorResource("Failed to load layer '%s' with error: '%s'", getFilePath().getBuffer(), _reader_wrapper.getErrorText());
		failed();
		return;
	}

	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();
	const auto& reader = *_reader_wrapper.getReader();

	char8_t name[256] = { 0 };

	{
		const auto guard = reader.enterElementGuard(u8"name");

		if (!reader.isString()) {
			LogErrorDefault("Failed to load layer '%s'. Invalid name.", getFilePath().getBuffer());
			return;
		}

		reader.readString(name, ARRAY_SIZE(name));
	}

	{
		const auto guard = reader.enterElementGuard(u8"objects");

		if (!reader.isArray()) {
			LogErrorDefault("Failed to load layer '%s'.", (name) ? name : u8"<invalid_name>");
			return;
		}

		reader.forEachInArray([&](int32_t index) -> bool
		{
			char8_t archetype[256] = { 0 };

			{
				const auto guard = reader.enterElementGuard(u8"archetype");

				if (!reader.isNull() && !reader.isString()) {
					LogErrorDefault("Failed to load object at index %i. Archetype not specified.", index);
					return false;

				} else if (!reader.isNull()) {
					reader.readString(archetype, ARRAY_SIZE(archetype));
				}
			}

			if (archetype[0] == 0) {
				_archetypes.emplace_back(res_mgr.getResourceT<ECSArchetypeResource>(ECSManager::k_empty_archetype_res_name));
			} else {
				_archetypes.emplace_back(res_mgr.requestResourceT<ECSArchetypeResource>(archetype));
			}

			{
				const auto guard = reader.enterElementGuard(u8"overrides");

				if (reader.isNull()) {
					return false;
				} else if (!reader.isObject()) {
					LogErrorDefault("Failed to load object at index %i. Overrides field is not an object.", index);
					return false;
				}
			}

			return false;
		});
	}

	const auto callback = Gaff::MemberFunc(this, &ECSLayerResource::archetypeLoaded);
	Vector<IResource*> resources;
	resources.reserve(_archetypes.size());

	for (auto& arch_res : _archetypes) {
		resources.emplace_back(arch_res.get());
	}

	_callback_id = res_mgr.registerCallback(resources, callback);
}

NS_END
