/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Config/Shibboleth_Config.h"
#include "Log/Shibboleth_LogManager.h"
#include "Shibboleth_SerializeReader.h"

SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::ConfigDirectoryAttribute, IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::GlobalConfigAttribute, IAttribute)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ConfigDirectoryAttribute)
SHIB_REFLECTION_CLASS_DEFINE(GlobalConfigAttribute)



ConfigDirectoryAttribute::ConfigDirectoryAttribute(const char8_t* directory):
	_directory(directory)
{
}

const char8_t* ConfigDirectoryAttribute::getDirectory(void) const
{
	return _directory;
}

Refl::IAttribute* ConfigDirectoryAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(ConfigDirectoryAttribute, allocator.getPoolIndex("Reflection"), allocator, _directory);
}




GlobalConfigAttribute::GlobalConfigAttribute(const IConfig* config):
	_config(config)
{
}

void GlobalConfigAttribute::setConfig(const IConfig* config)
{
	_config = config;
}

const IConfig* GlobalConfigAttribute::getConfig(void) const
{
	return _config;
}

Error GlobalConfigAttribute::createAndLoadConfig(const Refl::IReflectionDefinition& ref_def)
{
	static constexpr eastl::u8string_view k_config_name_ending = u8"Config";
	U8String config_path = ref_def.getReflectionInstance().getName();

	if (Gaff::EndsWith(config_path.data(), k_config_name_ending.data())) {
		config_path.erase(config_path.size() - k_config_name_ending.size() - 1);
	}

	const ConfigDirectoryAttribute* const config_dir_attr = ref_def.getClassAttr<ConfigDirectoryAttribute>();

	if (config_dir_attr) {
		config_path += U8String(u8"/") + config_dir_attr->getDirectory();
	}

	config_path = u8"cfg/" + config_path + u8".cfg";

	ProxyAllocator allocator; // $TODO: Set a real allocator.
	IConfig* const config_instance = ref_def.CREATET(Shibboleth::IConfig, allocator);

	if (!config_instance) {
		LogErrorDefault(
			"GlobalConfigAttribute::createAndLoadConfig: Failed to create config instance of type '%s'.",
			reinterpret_cast<const char*>(ref_def.getReflectionInstance().getName())
		);

		return Error::k_fatal_error;
	}

	setConfig(config_instance);

	Gaff::JSON config_data;

	if (!config_data.parseFile(config_path.data())) {
		LogErrorDefault(
			"GlobalConfigAttribute::createAndLoadConfig: Failed to parse config '%s'. %s",
			reinterpret_cast<const char*>(config_path.data()),
			reinterpret_cast<const char*>(config_data.getErrorText())
		);

		return Error::k_simple_error;
	}

	auto reader = MakeSerializeReader(config_data);

	if (!ref_def.load(reader, config_instance->getBasePointer())) {
		LogErrorDefault(
			"GlobalConfigAttribute::createAndLoadConfig: Failed to load config of type '%s'.",
			reinterpret_cast<const char*>(ref_def.getReflectionInstance().getName())
		);

		return Error::k_simple_error;
	}

	return Error::k_no_error;
}

Refl::IAttribute* GlobalConfigAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(GlobalConfigAttribute, allocator.getPoolIndex("Reflection"), allocator, _config);
}

NS_END
