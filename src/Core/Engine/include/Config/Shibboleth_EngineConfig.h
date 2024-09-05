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

#include "Shibboleth_EngineDefines.h"
#include "Shibboleth_Config.h"

NS_SHIBBOLETH

class EngineConfig final : public Refl::IReflectionObject
{
public:
	static constexpr const char8_t* const k_read_file_pool_name = u8"read_file";

	enum class Flag
	{
		EditorMode,
		NoManagers,
		NoLoadModules,
		NoMainLoop,

		Count
	};

	Vector< HashString64<> > module_unload_order{ ENGINE_ALLOCATOR };
	Vector< HashString64<> > module_load_order{ ENGINE_ALLOCATOR };
	Vector<U8String> module_directories{ ENGINE_ALLOCATOR };

	U8String working_dir{ u8".", ENGINE_ALLOCATOR };
	U8String log_dir{ ENGINE_ALLOCATOR };

	const Refl::IReflectionDefinition* file_system = nullptr;
	const Refl::IReflectionDefinition* main_loop = nullptr;

	int32_t read_file_threads = 1;

	Gaff::Flags<Flag> flags;

	SHIB_REFLECTION_CLASS_DECLARE(EngineConfig);
};

// // App
// constexpr const char8_t* const k_config_app_working_dir = u8"app_working_dir";
// constexpr const char8_t* const k_config_app_log_dir = u8"app_log_dir";
// constexpr const char8_t* const k_config_app_editor_mode = u8"app_editor_mode";
// constexpr const char8_t* const k_config_app_read_file_threads = u8"app_read_file_threads";
// constexpr const char8_t* const k_config_app_file_system = u8"app_file_system";
// constexpr const char8_t* const k_config_app_no_load_modules = u8"app_no_load_modules";
// constexpr const char8_t* const k_config_app_no_managers = u8"app_no_managers";
// constexpr const char8_t* const k_config_app_no_main_loop = u8"app_no_main_loop";
// constexpr const char8_t* const k_config_app_main_loop = u8"app_main_loop";

// constexpr int32_t k_config_app_default_read_file_threads = 1;

// // Modules
// // $TODO: Convert these to proper config files.
// constexpr const char8_t* const k_config_module_unload_order = u8"module_unload_order";
// constexpr const char8_t* const k_config_module_load_order = u8"module_load_order";
// constexpr const char8_t* const k_config_module_directories = u8"module_directories";

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::EngineConfig::Flag)
SHIB_REFLECTION_DECLARE(Shibboleth::EngineConfig)
