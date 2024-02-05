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

SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::InitFromConfigAttribute, IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::GlobalConfigAttribute, IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::ConfigFileAttribute, IAttribute)

namespace
{
	static Shibboleth::Error LoadConfig(void* object, const Refl::IReflectionDefinition& ref_def)
	{
		static constexpr eastl::u8string_view k_config_name_ending = u8"Config";
		Shibboleth::U8String config_path = ref_def.getReflectionInstance().getName();

		if (Gaff::EndsWith(config_path.data(), k_config_name_ending.data())) {
			config_path.erase(config_path.size() - k_config_name_ending.size() - 1);
		}

		const Shibboleth::ConfigFileAttribute* const config_file_attr = ref_def.getClassAttr<Shibboleth::ConfigFileAttribute>();

		if (config_file_attr) {
			if (config_file_attr->getFileName()) {
				config_path = config_file_attr->getFileName();
			}

			if (config_file_attr->getDirectory()) {
				config_path = Shibboleth::U8String(config_file_attr->getDirectory()) + u8"/" + config_path;
			}
		}

		config_path = u8"cfg/" + config_path + u8".cfg";

		Gaff::JSON config_data;

		if (!config_data.parseFile(config_path.data())) {
			LogErrorDefault(
				"LoadConfig: Failed to parse config '%s'. %s",
				reinterpret_cast<const char*>(config_path.data()),
				reinterpret_cast<const char*>(config_data.getErrorText())
			);

			return Shibboleth::Error::k_simple_error;
		}

		auto reader = Shibboleth::MakeSerializeReader(config_data);

		if (!ref_def.load(reader, object)) {
			LogErrorDefault(
				"LoadConfig: Failed to load config of type '%s'.",
				reinterpret_cast<const char*>(ref_def.getReflectionInstance().getName())
			);

			return Shibboleth::Error::k_simple_error;
		}

		return Shibboleth::Error::k_no_error;
	}
}


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(GlobalConfigAttribute)
SHIB_REFLECTION_CLASS_DEFINE(ConfigFileAttribute)



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

	return LoadConfig(config_instance->getBasePointer(), ref_def);
}

Refl::IAttribute* GlobalConfigAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(GlobalConfigAttribute, allocator.getPoolIndex("Reflection"), allocator, _config);
}



ConfigFileAttribute::ConfigFileAttribute(const char8_t* file_name, const char8_t* directory):
	_file_name(file_name), _directory(directory)
{
}

const char8_t* ConfigFileAttribute::getDirectory(void) const
{
	return _directory;
}

const char8_t* ConfigFileAttribute::getFileName(void) const
{
	return _file_name;
}

Refl::IAttribute* ConfigFileAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(ConfigFileAttribute, allocator.getPoolIndex("Reflection"), allocator, _directory);
}



void InitFromConfigAttribute::instantiated(void* object, const Refl::IReflectionDefinition& ref_def)
{
	LoadConfig(object, ref_def);
}

Refl::IAttribute* InitFromConfigAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(GlobalConfigAttribute, allocator.getPoolIndex("Reflection"), allocator);
}

NS_END
