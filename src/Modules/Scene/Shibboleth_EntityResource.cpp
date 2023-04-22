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

#include "Shibboleth_EntityResource.h"
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_ResourceLogging.h>
#include <Shibboleth_EntityManager.h>
#include <Shibboleth_AppUtils.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EntityResource)
	.classAttrs(
		Shibboleth::ResExtAttribute(u8".entity.bin"),
		Shibboleth::ResExtAttribute(u8".entity"),
		Shibboleth::MakeLoadFileCallbackAttribute(&Shibboleth::EntityResource::loadEntity)
	)

	.template base<Shibboleth::IResource>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::EntityResource)

namespace
{
	static constexpr const char8_t* const k_entity_resource_schema =
	u8R"({
		"type": "object",

		"properties":
		{
			"name": { "type": "string" },

			"components":
			{
				"type": "object",
				"additionalProperties":
				{
					"type": "object",

					"properties":
					{
						"type": { "type": "string" },
						"parent":
						{
							"oneOf":
							[
								{ "type": "string" },
								{ "type": "null" }
							]
						}
					},

					"required": ["type"]
				},
			}
		}
	})";
}


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(EntityResource)

EntityResource::EntityResource(void):
	_reader_wrapper(ProxyAllocator("Resource")),
	_entity_definition(GetManagerTFast<EntityManager>())
{
}

EntityResource::~EntityResource(void)
{
}

void EntityResource::loadEntity(IFile* file, uintptr_t /*thread_id_int*/)
{
	if (!OpenJSONOrMPackFile(_reader_wrapper, getFilePath().getBuffer(), file, true, k_entity_resource_schema)) {
		LogErrorResource("Failed to load entity '%s' with error: '%s'", getFilePath().getBuffer(), _reader_wrapper.getErrorText());
		failed();
		return;
	}

	const auto& reader = *_reader_wrapper.getReader();

	Refl::Reflection<Entity>::GetInstance().load(reader, _entity_definition);

	// Use filename as default name.
	if (_entity_definition.getName().empty()) {
		U8String name = getFilePath().getString();
		const size_t extenstion_start_index = name.rfind(u8'.');
		const size_t last_separator_index = name.rfind(u8'/');

		GAFF_ASSERT(extenstion_start_index != U8String::npos);
		GAFF_ASSERT(last_separator_index != U8String::npos);
		GAFF_ASSERT(last_separator_index < extenstion_start_index);

		name = name.substr(last_separator_index + 1, extenstion_start_index - (last_separator_index + 1));

		_entity_definition.setName(name);
	}

	const ReflectionManager& refl_mgr = GetApp().getReflectionManager();
	ProxyAllocator allocator("Resource");

	// Create and load components.
	{
		const auto guard = reader.enterElementGuard(u8"components");

		if (reader.isObject()) {
			reader.forEachInObject([&](const char8_t* comp_name) -> bool
			{
				char8_t comp_type[512] = { 0 };

				{
					const auto type_guard = reader.enterElementGuard("type");
					reader.readString(comp_type, std::size(comp_type));
				}

				const Refl::IReflectionDefinition* const ref_def = refl_mgr.getReflection(Gaff::FNV1aHash64String(comp_type));

				if (!ref_def) {
					LogErrorResource("Failed to fully load entity '%s'. Unknown component type '%s'.", getFilePath().getBuffer(), comp_type);
					return false;
				}

				EntityComponent* const component = ref_def->createT<EntityComponent>(allocator);

				if (!component) {
					LogErrorResource("Failed to fully load entity '%s'. Component type '%s' failed to allocate.", getFilePath().getBuffer(), comp_type);
					return false;
				}

				if (!ref_def->load(reader, component->getBasePointer())) {
					LogErrorResource("Failed to fully load entity '%s'. Component type '%s' failed to load.", getFilePath().getBuffer(), comp_type);

					SHIB_FREET(component, allocator);
					return false;
				}

				component->setName(comp_name);

				_entity_definition.addComponent(*component);

				return false;
			});
		}
	}

	_reader_wrapper.freeReader();
}

NS_END
