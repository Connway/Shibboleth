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

#include "Shibboleth_ECSLayer.h"
#include <Shibboleth_SerializeInterfaces.h>
#include <Shibboleth_ECSManager.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_AppUtils.h>

NS_SHIBBOLETH

ECSLayer::ECSLayer(void)
{
}

ECSLayer::~ECSLayer(void)
{
}

void ECSLayer::load(const ISerializeReader& reader)
{
	ECSManager& ecs_mgr = GetManagerTFast<ECSManager>();
	GAFF_REF(ecs_mgr);

	char8_t name[256] = { 0 };

	{
		const auto guard = reader.enterElementGuard(u8"name");

		if (reader.isString()) {
			reader.readString(name, ARRAY_SIZE(name));
		}
	}

	{
		const auto guard = reader.enterElementGuard(u8"objects");

		if (!reader.isArray()) {
			LogErrorDefault("Failed to load layer '%s'.", (name[0]) ? name : u8"<invalid_name>");
			return;
		}

		reader.forEachInArray([&](int32_t index) -> bool
		{
				char8_t archetype[256] = { 0 };

			{
				const auto guard = reader.enterElementGuard(u8"archetype");

				if (!reader.isString()) {
					LogErrorDefault("Failed to load object at index %i. Malformed object definition.", index);
					return false;
				}

				reader.readString(archetype, ARRAY_SIZE(archetype));
			}

			{
				const auto guard = reader.enterElementGuard(u8"overrides");

				if (reader.isNull()) {
					return false;
				} else if (!reader.isObject()) {
					LogErrorDefault("Failed to load object at index %i. Overrides field is not an object.", index);
					return false;
				}

				//loadOverrides(reader, archetype_instance);
			}

			return false;
		});
	}
}

void ECSLayer::save(ISerializeWriter& writer)
{
	GAFF_REF(writer);
	// $TODO: Implement
}

const U8String& ECSLayer::getName(void) const
{
	return _name;
}

void ECSLayer::setName(const char8_t* name)
{
	_name = name;
}

NS_END
