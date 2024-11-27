/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#define SHIB_REFL_IMPL
#include "Config/Shibboleth_Config.h"
#include "Attributes/Shibboleth_EngineAttributesCommon.h"
#include "Log/Shibboleth_LogManager.h"
#include "Shibboleth_SerializeReader.h"
#include "Shibboleth_EngineDefines.h"
#include <Gaff_JSON.h>
#include <filesystem>
#include <cctype>

#ifndef SHIB_CONFIG_OVERRIDE_DIRECTORY
	#ifdef _DEBUG
		#define SHIB_CONFIG_OVERRIDE_DIRECTORY "Debug"
	#else
		#define SHIB_CONFIG_OVERRIDE_DIRECTORY "Release"
	#endif
#endif


SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::GlobalConfigAttribute)
	.classAttrs(
		Shibboleth::AttributeBucketAttribute()
	)

	.template base<Refl::IAttribute>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::GlobalConfigAttribute)

SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::InitFromConfigAttribute, Refl::IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::ConfigFileAttribute, Refl::IAttribute)

SHIB_SIMPLE_ATTRIBUTE_DEFINE(ConfigVarAttribute, Shibboleth)

SHIB_REFLECTION_IMPL(Shibboleth::Config)

NS_SHIBBOLETH

SHIB_REFLECTION_ATTRIBUTE_DEFINE(InitFromConfigAttribute)
SHIB_REFLECTION_ATTRIBUTE_DEFINE(GlobalConfigAttribute)
SHIB_REFLECTION_ATTRIBUTE_DEFINE(ConfigFileAttribute)

SHIB_REFLECTION_CLASS_DEFINE(Config)



void GlobalConfigAttribute::setConfig(const Config* config)
{
	_config = config;
}

const Config* GlobalConfigAttribute::getConfig(void) const
{
	return _config;
}

Error GlobalConfigAttribute::createAndLoadConfig(const Refl::IReflectionDefinition& ref_def)
{
	ProxyAllocator allocator{ ENGINE_ALLOCATOR }; // $TODO: Allocator attribute.
	Config* const config_instance = ref_def.createT<Config>(allocator);

	if (!config_instance) {
		LogErrorDefault(
			"GlobalConfigAttribute::createAndLoadConfig: Failed to create config instance of type '%s'.",
			reinterpret_cast<const char*>(ref_def.getReflectionInstance().getName())
		);

		config_instance->_error = Error::k_fatal_error;

		return Error::k_fatal_error;
	}

	setConfig(config_instance);

	// InitFromConfigAttribute will load the config in the createT call.
	return config_instance->_error;
}

void GlobalConfigAttribute::apply(Refl::IReflectionDefinition& ref_def)
{
	if (!ref_def.hasClassAttr<InitFromConfigAttribute>()) {
		InitFromConfigAttribute init_attr;
		ref_def.addClassAttr(init_attr);
	}
}



ConfigFileAttribute::ConfigFileAttribute(const char8_t* file_path):
	_file_path(file_path)
{
}

const char8_t* ConfigFileAttribute::getFilePath(void) const
{
	return _file_path;
}



InitFromConfigAttribute::InitFromConfigAttribute(Gaff::Flags<Flag> flags):
	_flags(flags)
{
}

void InitFromConfigAttribute::instantiated(void* object, const Refl::IReflectionDefinition& ref_def)
{
	if (_flags.testAll(Flag::InitOnInstantiate)) {
		loadConfig(object, ref_def);
	}
}

Error InitFromConfigAttribute::loadConfig(void* object, const Refl::IReflectionDefinition& ref_def, const U8String& relative_cfg_path) const
{
	// $TODO: In final packaged builds, this might be binary instead.

	const U8String config_path = u8"cfg/" + relative_cfg_path + u8".cfg";
	Gaff::JSON config_data;

	if (!config_data.parseFile(config_path.data())) {
		LogErrorDefault(
			"InitFromConfigAttribute::LoadConfig: Failed to parse config '%s'. %s",
			reinterpret_cast<const char*>(config_path.data()),
			reinterpret_cast<const char*>(config_data.getErrorText())
		);

		Config* const config = ref_def.template getInterface<Config>(object);
		GAFF_ASSERT(config);

		config->_error = Error::k_fatal_error;

		return Error::k_fatal_error;
	}

	// Load overrides if any.
	const U8String override_config_path = u8"cfg/overrides/" SHIB_CONFIG_OVERRIDE_DIRECTORY u8"/" + relative_cfg_path + u8".cfg";

	if (std::filesystem::exists(override_config_path.data())) {
		Gaff::JSON override_config_data;

		if (!override_config_data.parseFile(override_config_path.data())) {
			LogErrorDefault(
				"InitFromConfigAttribute::LoadConfig: Failed to parse config '%s'. %s",
				reinterpret_cast<const char*>(config_path.data()),
				reinterpret_cast<const char*>(config_data.getErrorText())
			);

			Config* const config = ref_def.template getInterface<Config>(object);
			GAFF_ASSERT(config);

			config->_error = Error::k_fatal_error;

			return Error::k_fatal_error;
		}

		override_config_data.forEachInObject([&](const char8_t* key, const Gaff::JSON& value) -> bool
		{
			config_data.setObject(key, value);
			return false;
		});
	}

	auto reader = MakeSerializeReader(config_data);

	if (_flags.testAll(Flag::UseConfigVarAttribute)) {
		Vector< eastl::pair<int32_t, const ConfigVarAttribute*> > config_vars;
		bool has_error = false;

		ref_def.getVarAttrs(config_vars);

		for (const auto& entry : config_vars) {
			const ScopeGuard guard = reader.enterElementGuard(ref_def.getVarName(entry.first).getBuffer());
			Refl::IReflectionVar* const var = ref_def.getVar(entry.first);

			if (!var->load(reader, object)) {
				LogErrorDefault(
					"InitFromConfigAttribute::loadConfig: Failed to load variable '%s' in config of type '%s'.",
					ref_def.getVarName(entry.first).getBuffer(),
					reinterpret_cast<const char*>(ref_def.getReflectionInstance().getName())
				);

				has_error = true;
			}
		}

		if (has_error) {
			Config* const config = ref_def.template getInterface<Config>(object);
			GAFF_ASSERT(config);

			config->_error = Error::k_fatal_error;

			return Error::k_fatal_error;
		}

	} else {
		if (!ref_def.load(reader, object)) {
			LogErrorDefault(
				"InitFromConfigAttribute::loadConfig: Failed to load config of type '%s'.",
				reinterpret_cast<const char*>(ref_def.getReflectionInstance().getName())
			);

			Config* const config = ref_def.template getInterface<Config>(object);
			GAFF_ASSERT(config);

			config->_error = Error::k_fatal_error;

			return Error::k_fatal_error;
		}
	}

	return Error::k_no_error;
}

Error InitFromConfigAttribute::loadConfig(void* object, const Refl::IReflectionDefinition& ref_def) const
{
	static constexpr eastl::u8string_view k_config_name_ending = u8"Config";
	U8String config_path = ref_def.getReflectionInstance().getName();

	// Strip out the word Config in classes with the naming pattern MyClassNameConfig.
	if (Gaff::EndsWith(config_path.data(), k_config_name_ending.data())) {
		config_path.erase(config_path.size() - k_config_name_ending.size());
	}

	// Strip out namespaces.
	if (const size_t colon_index = config_path.find_last_of(u8':'); colon_index != U8String::npos) {
		config_path.erase(0, colon_index + 1);
	}

	const ConfigFileAttribute* const config_file_attr = ref_def.getClassAttr<ConfigFileAttribute>();

	if (config_file_attr) {
		if (config_file_attr->getFilePath()) {
			config_path = config_file_attr->getFilePath();
		}

	// $TODO: This naively assumes no unicode characters in class names. Will break if non-ASCII characters are used in class names.
	} else if (!_flags.testAll(Flag::NoSnakeCaseConversion)) {
		for (int32_t i = 0; i < static_cast<int32_t>(config_path.size()); ++i) {
			if (std::isupper(static_cast<char>(config_path[i]))) {
				config_path[i] = static_cast<char8_t>(std::tolower(static_cast<char>(config_path[i])));

				if (i) {
					config_path.insert(config_path.begin() + i, u8'_');
					++i; // Skip over character we just converted.
				}
			}
		}
	}

	return loadConfig(object, ref_def, config_path);
}



NS_END
