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

#pragma once

#include "Shibboleth_Defines.h"
#include <Gaff_Defines.h>

NS_SHIBBOLETH

// App
constexpr const char8_t* const k_config_app_working_dir = u8"app_working_dir";
constexpr const char8_t* const k_config_app_log_dir = u8"app_log_dir";
constexpr const char8_t* const k_config_app_editor_mode = u8"app_editor_mode";
constexpr const char8_t* const k_config_app_hot_reload_modules = u8"app_hot_reload_modules";
constexpr const char8_t* const k_config_app_read_file_threads = u8"app_read_file_threads";
constexpr const char8_t* const k_config_app_file_system = u8"app_file_system";
constexpr const char8_t* const k_config_app_no_load_modules = u8"app_no_load_modules";
constexpr const char8_t* const k_config_app_no_managers = u8"app_no_managers";
constexpr const char8_t* const k_config_app_no_main_loop = u8"app_no_main_loop";
constexpr const char8_t* const k_config_app_main_loop = u8"app_main_loop";

constexpr const char8_t* const k_config_app_default_log_dir = u8"./logs";
constexpr const char8_t* const k_config_app_read_file_pool_name = u8"Read File";
constexpr int32_t k_config_app_default_read_file_threads = 1;

// Modules
constexpr const char8_t* const k_config_module_unload_order = u8"module_unload_order";
constexpr const char8_t* const k_config_module_load_order = u8"module_load_order";
constexpr const char8_t* const k_config_module_directories = u8"module_directories";

NS_END
