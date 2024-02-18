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
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::ConfigVarAttribute, IAttribute)



NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(InitFromConfigAttribute)
SHIB_REFLECTION_CLASS_DEFINE(GlobalConfigAttribute)
SHIB_REFLECTION_CLASS_DEFINE(ConfigFileAttribute)
SHIB_REFLECTION_CLASS_DEFINE(ConfigVarAttribute)



GlobalConfigAttribute::GlobalConfigAttribute(const Refl::IReflectionObject* config):
	_config(config)
{
}

void GlobalConfigAttribute::setConfig(const Refl::IReflectionObject* config)
{
	_config = config;
}

const Refl::IReflectionObject* GlobalConfigAttribute::getConfig(void) const
{
	return _config;
}

Error GlobalConfigAttribute::createAndLoadConfig(const Refl::IReflectionDefinition& ref_def)
{
	ProxyAllocator allocator; // $TODO: Set a real allocator.
	Refl::IReflectionObject* const config_instance = ref_def.CREATET(Refl::IReflectionObject, allocator);

	if (!config_instance) {
		LogErrorDefault(
			"GlobalConfigAttribute::createAndLoadConfig: Failed to create config instance of type '%s'.",
			reinterpret_cast<const char*>(ref_def.getReflectionInstance().getName())
		);

		return Error::k_fatal_error;
	}

	setConfig(config_instance);

	// InitFromConfigAttribute will load the config in the createT call.
	return Error::k_no_error;
}

void GlobalConfigAttribute::apply(Refl::IReflectionDefinition& ref_def)
{
	if (!ref_def.hasClassAttr<InitFromConfigAttribute>()) {
		InitFromConfigAttribute init_attr;
		ref_def.addClassAttr(init_attr);
	}
}

Refl::IAttribute* GlobalConfigAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(GlobalConfigAttribute, allocator.getPoolIndex("Reflection"), allocator, _config);
}



ConfigFileAttribute::ConfigFileAttribute(const char8_t* file_path):
	_file_path(file_path)
{
}

const char8_t* ConfigFileAttribute::getFilePath(void) const
{
	return _file_path;
}

Refl::IAttribute* ConfigFileAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(ConfigFileAttribute, allocator.getPoolIndex("Reflection"), allocator, _file_path);
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

Refl::IAttribute* InitFromConfigAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(GlobalConfigAttribute, allocator.getPoolIndex("Reflection"), allocator);
}

Error InitFromConfigAttribute::loadConfig(void* object, const Refl::IReflectionDefinition& ref_def, const U8String& relative_cfg_path) const
{
	const U8String config_path = u8"cfg/" + relative_cfg_path + u8".cfg";
	Gaff::JSON config_data;

	if (!config_data.parseFile(config_path.data())) {
		LogErrorDefault(
			"InitFromConfigAttribute::LoadConfig: Failed to parse config '%s'. %s",
			reinterpret_cast<const char*>(config_path.data()),
			reinterpret_cast<const char*>(config_data.getErrorText())
		);

		return Error::k_simple_error;
	}

	auto reader = MakeSerializeReader(config_data);

	if (_flags.testAll(Flag::UseConfigVarAttribute)) {
		Shibboleth::Vector< eastl::pair<int32_t, const ConfigVarAttribute*> > config_vars;
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
			return Error::k_simple_error;
		}

	} else {
		if (!ref_def.load(reader, object)) {
			LogErrorDefault(
				"InitFromConfigAttribute::loadConfig: Failed to load config of type '%s'.",
				reinterpret_cast<const char*>(ref_def.getReflectionInstance().getName())
			);

			return Error::k_simple_error;
		}
	}

	return Error::k_no_error;
}

Error InitFromConfigAttribute::loadConfig(void* object, const Refl::IReflectionDefinition& ref_def) const
{
	static constexpr eastl::u8string_view k_config_name_ending = u8"Config";
	Shibboleth::U8String config_path = ref_def.getReflectionInstance().getName();

	// Strip out the word Config in classes with the naming pattern MyClassNameConfig.
	if (Gaff::EndsWith(config_path.data(), k_config_name_ending.data())) {
		config_path.erase(config_path.size() - k_config_name_ending.size() - 1);
	}

	const Shibboleth::ConfigFileAttribute* const config_file_attr = ref_def.getClassAttr<Shibboleth::ConfigFileAttribute>();

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



Refl::IAttribute* ConfigVarAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(ConfigVarAttribute, allocator.getPoolIndex("Reflection"), allocator);
}

NS_END
