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

#pragma once

#include "Reflection/Shibboleth_Reflection.h"
#include "Shibboleth_Error.h"
#include <Gaff_Flags.h>

NS_SHIBBOLETH

class Config : public Refl::IReflectionObject
{
	SHIB_REFLECTION_CLASS_DECLARE(Config);
};



class GlobalConfigAttribute final : public Refl::IAttribute
{
public:
	GlobalConfigAttribute(const GlobalConfigAttribute& attr) = default;
	GlobalConfigAttribute(void) = default;

	void setConfig(const Config* config);
	const Config* getConfig(void) const;

	Error createAndLoadConfig(const Refl::IReflectionDefinition& ref_def);

	void apply(Refl::IReflectionDefinition& ref_def) override;

private:
	const Config* _config = nullptr;

	SHIB_REFLECTION_ATTRIBUTE_DECLARE(GlobalConfigAttribute);
};

class ConfigFileAttribute final : public Refl::IAttribute
{
public:
	ConfigFileAttribute(const ConfigFileAttribute& attr) = default;
	ConfigFileAttribute(const char8_t* file_path = nullptr);

	const char8_t* getFilePath(void) const;

private:
	const char8_t* const _file_path = nullptr;

	SHIB_REFLECTION_ATTRIBUTE_DECLARE(ConfigFileAttribute);
};

class InitFromConfigAttribute final : public Refl::IAttribute
{
public:
	enum class Flag
	{
		UseConfigVarAttribute,
		InitOnInstantiate,
		NoSnakeCaseConversion,

		Count
	};

	InitFromConfigAttribute(const InitFromConfigAttribute& attr) = default;
	InitFromConfigAttribute(void) = default;

	InitFromConfigAttribute(Gaff::Flags<Flag> flags);

	void instantiated(void* object, const Refl::IReflectionDefinition& ref_def) override;

	Error loadConfig(void* object, const Refl::IReflectionDefinition& ref_def, const U8String& relative_cfg_path) const;
	Error loadConfig(void* object, const Refl::IReflectionDefinition& ref_def) const;

private:
	Gaff::Flags<Flag> _flags { Flag::InitOnInstantiate };

	SHIB_REFLECTION_ATTRIBUTE_DECLARE(InitFromConfigAttribute);
};



template <class T>
const T* GetConfig(void)
{
	static_assert(std::is_base_of_v<Refl::IReflectionObject, T>, "Passed in class is not an IReflectionObject.");

	const auto& ref_def = Refl::Reflection<T>::GetReflectionDefinition();
	const auto* const attr = ref_def.template getClassAttr<GlobalConfigAttribute>();

	return (attr) ? static_cast<const T*>(attr->getConfig()) : nullptr;
}

template<class T>
const T& GetConfigRef(void)
{
	const T* config = GetConfig<T>();
	GAFF_ASSERT(config);

	return *config;
}

template <class T>
Error InitConfig(void)
{
	static_assert(std::is_base_of_v<Refl::IReflectionObject, T>, "Passed in class is not an IReflectionObject.");

	const auto& ref_def = Refl::Reflection<T>::GetReflectionDefinition();
	const auto* const attr = ref_def.template getClassAttr<InitFromConfigAttribute>();

	const T& config = GetConfigRef<T>();

	return attr->loadConfig(const_cast<T*>(&config), ref_def);
}

NS_END

SHIB_SIMPLE_ATTRIBUTE_DECLARE(ConfigVarAttribute, Shibboleth)

SHIB_REFLECTION_DECLARE(Shibboleth::InitFromConfigAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::GlobalConfigAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::ConfigFileAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::Config)

#ifdef SHIB_REFL_IMPL
	SHIB_REFLECTION_BUILD_BEGIN(Shibboleth::Config)
	SHIB_REFLECTION_BUILD_END(Shibboleth::Config)
#endif
