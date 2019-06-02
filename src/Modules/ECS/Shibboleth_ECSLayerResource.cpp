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

#include "Shibboleth_ECSLayerResource.h"
#include <Shibboleth_ResourceExtensionAttribute.h>
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Gaff_SerializeInterfaces.h>

SHIB_REFLECTION_DEFINE(ECSLayerResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ECSLayerResource)
	.classAttrs(
		ResExtAttribute(".layer.bin"),
		ResExtAttribute(".layer"),
		MakeLoadFileCallbackAttribute(&ECSLayerResource::loadLayer)
	)

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(ECSLayerResource)

ECSLayerResource::ECSLayerResource(void):
	_reader_wrapper(ProxyAllocator("Resource"))
{
}

ECSLayerResource::~ECSLayerResource(void)
{
}

void ECSLayerResource::loadOverrides(const Gaff::ISerializeReader& reader, const ECSArchetype& base_archetype)
{
	ECSArchetype new_archetype;
	new_archetype.finalize(reader, &base_archetype);
}

void ECSLayerResource::archetypeLoaded(IResource&)
{
	for (const auto& arch_res : _archetypes) {
		if (arch_res->getState() == IResource::RS_PENDING) {
			return;
		}
	}

	const auto& reader = *_reader_wrapper.getReader();
	const auto objects_guard = reader.enterElementGuard("objects");
	int32_t index = 0;

	for (const auto& arch_res : _archetypes) {
		const auto element_guard = reader.enterElementGuard(index);
		const auto override_guard = reader.enterElementGuard("overrides");

		if (reader.isNull() || !reader.isObject()) {
			continue;
		}

		/*if (!*/loadOverrides(reader, arch_res->getArchetype());/*) {*/
			//LogErrorResource("Failed to load archetype overrides for object at index %i.", index);
		//}

		++index;
	}

	// Always mark succeeded, even if an object failed to load.
	succeeded();
}

void ECSLayerResource::loadLayer(IFile* file)
{	
	if (!OpenJSONOrMPackFile(_reader_wrapper, getFilePath().getBuffer(), file, true)) {
		LogErrorResource("Failed to load layer '%s' with error: '%s'", getFilePath().getBuffer(), _reader_wrapper.getErrorText());
		failed();
		return;
	}

	ResourceManager& res_mgr = GetApp().getManagerTFast<ResourceManager>();
	ECSManager& ecs_mgr = GetApp().getManagerTFast<ECSManager>();
	const auto& reader = *_reader_wrapper.getReader();

	GAFF_REF(ecs_mgr);

	char name[256] = { 0 };

	{
		const auto guard = reader.enterElementGuard("name");

		if (!reader.isString()) {
			LogErrorDefault("Failed to load layer '%s'. Invalid name.", getFilePath().getBuffer());
			return;
		}

		reader.readString(name, ARRAY_SIZE(name));
	}

	{
		const auto guard = reader.enterElementGuard("objects");

		if (!reader.isArray()) {
			LogErrorDefault("Failed to load layer '%s'.", (name) ? name : "<invalid_name>");
			return;
		}

		reader.forEachInArray([&](int32_t index) -> bool
		{
			char archetype[256] = { 0 };

			{
				const auto guard = reader.enterElementGuard("archetype");

				if (!reader.isString()) {
					LogErrorDefault("Failed to load object at index %i. Malformed object definition.", index);
					return false;
				}

				reader.readString(archetype, ARRAY_SIZE(archetype));
			}

			_archetypes.emplace_back(res_mgr.requestResourceT<ECSArchetypeResource>(archetype));

			{
				const auto guard = reader.enterElementGuard("overrides");

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

	for (auto& arch_res : _archetypes) {
		arch_res->addLoadedCallback(Gaff::MemberFunc(this, &ECSLayerResource::archetypeLoaded));
	}
}

NS_END
