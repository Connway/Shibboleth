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

#pragma once

#include "Reflection/Shibboleth_Reflection.h"
#include "Shibboleth_Error.h"

NS_SHIBBOLETH

class IConfig : public Refl::IReflectionObject
{
};



class GlobalConfigAttribute final : public Refl::IAttribute
{
public:
	GlobalConfigAttribute(const IConfig* config);
	GlobalConfigAttribute(void) = default;

	void setConfig(const IConfig* config);
	const IConfig* getConfig(void) const;

	Error createAndLoadConfig(const Refl::IReflectionDefinition& ref_def);

	IAttribute* clone(void) const override;

private:
	const IConfig* _config = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(GlobalConfigAttribute);
};

class ConfigFileAttribute final : public Refl::IAttribute
{
public:
	ConfigFileAttribute(const char8_t* file_name = nullptr, const char8_t* directory = nullptr);

	const char8_t* getDirectory(void) const;
	const char8_t* getFileName(void) const;

	IAttribute* clone(void) const override;

private:
	const char8_t* const _file_name = nullptr;
	const char8_t* const _directory = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(ConfigFileAttribute);
};

class InitFromConfigAttribute final : public Refl::IAttribute
{
public:
	void instantiated(void* object, const Refl::IReflectionDefinition& ref_def) override;

	IAttribute* clone(void) const override;

	SHIB_REFLECTION_CLASS_DECLARE(InitFromConfigAttribute);
};



template <class T>
const T* GetConfig(void)
{
	static_assert(std::is_base_of_v<IConfig, T>, "Passed in class is not an IConfig.");

	const Refl::IReflectionDefinition& ref_def = Refl::Reflection<T>::GetReflectionDefinition();
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

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::InitFromConfigAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::GlobalConfigAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::ConfigFileAttribute)

NS_HASHABLE
	GAFF_CLASS_HASHABLE(Shibboleth::IConfig);
NS_END
