local gen_header = [[
// This file is generated! Any modifications will be lost in subsequent builds!

#pragma once

#ifdef SHIB_STATIC
// Includes
%s
#include <Shibboleth_Reflection.h>
#include <Shibboleth_IApp.h>

namespace
{
	template <class T>
	void RegisterOwningModule(void)
	{
		if constexpr (std::is_enum<T>::value) {
			Shibboleth::GetApp().getReflectionManager().registerEnumOwningModule(Reflection::Reflection<T>::GetHash(), "%s");
		} else {
			Shibboleth::GetApp().getReflectionManager().registerOwningModule(Reflection::Reflection<T>::GetHash(), "%s");
		}
	}

	template <class T>
	void InitReflectionT(Shibboleth::InitMode mode)
	{
		if constexpr (std::is_enum<T>::value) {
			if (mode == Shibboleth::InitMode::EnumsAndFirstInits) {
				Reflection::Reflection<T>::Init();
				RegisterOwningModule<T>();
			}
		} else if constexpr (std::is_base_of<Gaff::IAttribute, T>::value) {
			if (mode == Shibboleth::InitMode::Attributes) {
				Reflection::Reflection<T>::Init();
				RegisterOwningModule<T>();
			}
		} else {
			if (mode == Shibboleth::InitMode::Regular) {
				Reflection::Reflection<T>::Init();
				RegisterOwningModule<T>();
			}
		}
	}
}

namespace %s::Gen
{
	void InitNonOwnedReflection(void)
	{
		// Initialize Enums.
		Gaff::InitEnumReflection();

		// Initialize Attributes.
		Gaff::InitAttributeReflection();

		// Initialize regular classes.
		Gaff::InitClassReflection();
	}

	void InitReflection(Shibboleth::InitMode mode)
	{
		GAFF_REF(mode);
%s
	}
}

#else
	#include <Gaff_Defines.h>

	namespace Shibboleth
	{
		enum class InitMode : int8_t;
		class IApp;
	}
#endif

namespace %s
{
	bool Initialize(Shibboleth::IApp& app, Shibboleth::InitMode mode);
	void InitializeNonOwned(void);
	%s

#ifdef SHIB_STATIC
	void InitializeNonOwned(void)
	{
		Gen::InitNonOwnedReflection();
	}
#endif
}
]]

local gen_entry = [[
/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Gen_ReflectionInit.h"

#ifdef SHIB_STATIC

	#include <Shibboleth_Utilities.h>

	namespace %s
	{
		bool Initialize(Shibboleth::IApp& app, Shibboleth::InitMode mode)
		{
			if (mode == Shibboleth::InitMode::EnumsAndFirstInits) {
				Shibboleth::SetApp(app);

			} else if (mode == Shibboleth::InitMode::Regular) {
				// Initialize Enums.
				Gaff::InitEnumReflection();

				// Initialize Attributes.
				Gaff::InitAttributeReflection();
			}

			%s::Gen::InitReflection(mode);

			return true;
		}
	}

#else

	#include <Gaff_Defines.h>

	DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app)
	{
		return %s::Initialize(app);
	}

	DYNAMICEXPORT_C void InitModuleNonOwned(void)
	{
		%s::InitiailizeNonOwned();
	}

	DYNAMICEXPORT_C bool SupportsHotReloading(void)
	{
		return false;
	}

#endif
]]

local gen_project = [[
local GenerateProject = function()
	local base_dir = GetModulesDirectory("%s")

	project "%s"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl", base_dir .. "**.lua" }
		defines { "SHIB_STATIC" }

		ModuleGen("%s")
		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Frameworks/Gaff/include"
		}

	project "%sModule"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_%sModule.cpp" }

		ModuleCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("%s")
		SetupConfigMap()
end

local LinkDependencies = function()
	local deps = ModuleDependencies("%s")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
]]

local gen_static = [[
// This file is generated! Any modifications will be lost in subsequent builds!

#pragma once

// Module Includes
#ifdef SHIB_STATIC
	#undef SHIB_STATIC

%s
	#define SHIB_STATIC
#endif

// Engine reflection includes
%s
#include "Shibboleth_Utilities.h"
#include <Gaff_Defines.h>

#ifdef SHIB_STATIC

namespace Engine::Gen
{
	bool LoadModulesStatic(Shibboleth::InitMode mode)
	{
		Shibboleth::IApp& app = Shibboleth::GetApp();

		// Regular Modules
%s
		return true;
	}

	bool ShutdownModulesStatic(void)
	{
		// Regular Modules
%s
	}
}

#endif

namespace
{
	template <class T>
	void RegisterOwningModule(void)
	{
		if constexpr (std::is_enum<T>::value) {
			Shibboleth::GetApp().getReflectionManager().registerEnumOwningModule(Reflection::Reflection<T>::GetHash(), "Engine");
		} else {
			Shibboleth::GetApp().getReflectionManager().registerOwningModule(Reflection::Reflection<T>::GetHash(), "Engine");
		}
	}

	template <class T>
	void InitReflectionT(Shibboleth::InitMode mode)
	{
		if constexpr (std::is_enum<T>::value) {
			if (mode == Shibboleth::InitMode::EnumsAndFirstInits) {
				Reflection::Reflection<T>::Init();
				RegisterOwningModule<T>();
			}
		} else if constexpr (std::is_base_of<Gaff::IAttribute, T>::value) {
			if (mode == Shibboleth::InitMode::Attributes) {
				Reflection::Reflection<T>::Init();
				RegisterOwningModule<T>();
			}
		} else {
			if (mode == Shibboleth::InitMode::Regular) {
				Reflection::Reflection<T>::Init();
				RegisterOwningModule<T>();
			}
		}
	}
}

namespace Engine
{
	bool Initialize(Shibboleth::InitMode mode)
	{
%s

		return true;
	}
}

]]


newoption
{
	trigger = "module",
	value = "NAME",
	description = "The name of the module to generate the 'Gen_ReflectionInit.h' file for. ('gen_module_*' only)"
}

newaction
{
	trigger = "gen_module_create",
	description = "Generates the module folder and the 'Shibboleth<module_name>Module.cpp' file.",
	execute = function()
		local module_name = _OPTIONS["module"]
		local path = "../src/Modules/" .. module_name

		-- Make src/Modules/<module_name> and /include directories.
		os.mkdir(path)
		os.mkdir(path .. "/include")

		-- Make Shibboleth_<module_name>Module.cpp file.
		io.writefile(
			path .. "/Shibboleth_" .. module_name .. "Module.cpp",
			gen_entry:format(module_name, module_name, module_name, module_name)
		)

		-- Make project_generator.lua file.
		io.writefile(
			path .. "/project_generator.lua",
			gen_project:format(
				module_name, module_name,
				module_name, module_name,
				module_name, module_name,
				module_name
			)
		)
	end
}

newaction
{
	trigger = "gen_module_header",
	description = "Generates the 'Gen_ReflectionInit.h' file for a module.",
	execute = function()
		local module_name = _OPTIONS["module"]
		local source_folder = "../src/Modules/" .. module_name
		local include_folder = source_folder .. "/include"

		local file_class_map = {}

		function ParseFile(file)
			local stripped_file = file:sub(include_folder:len() + 2)

			if file == "Gen_ReflectionInit.h" then
				return
			end

			local lines = io.readfile(file):explode("\n")
			local last_namespace = ""

			for _, line in next, lines do
				local match = line:match("NS_(.+)")

				if match then
					if match:find("END") == nil then
						last_namespace = match:sub(1, 1) .. match:sub(2, -2):lower() .. "::"
					end

					goto continue
				end

				-- Detect classes
				match = line:match("SHIB_REFLECTION_DECLARE%((.+)%)")

				if match then
					if not file_class_map[stripped_file] then
						file_class_map[stripped_file] = {}
					end

					table.insert(file_class_map[stripped_file], last_namespace .. match)
					goto continue
				end

				::continue::
			end
		end


		if os.isfile(source_folder) then
			print("Module directory is actually a file!")
			return
		end

		if os.isfile(include_folder) then
			print("Module include directory is actually a file!")
			return
		end

		if not os.isdir(source_folder) then
			print("Module directory does not exist!")
			return
		end

		if not os.isdir(include_folder) then
			print("Module include directory does not exist!")
			return
		end

		local headers = os.matchfiles(include_folder .. "/**.h")
		table.foreachi(headers, ParseFile)

		local include_files = ""
		local init_funcs = ""
		local shutdown_func = ""

		for k,v in pairs(file_class_map) do
			include_files = include_files .. "#include <" .. k .. ">\n"

			for _,c in pairs(v) do
				init_funcs = init_funcs .. "\t\tInitReflectionT<" .. c .. ">(mode);\n"
			end
		end

		local module_file = source_folder .. "/Shibboleth_" .. module_name .. "Module.cpp"
		local match = io.readfile(module_file):match("ShutdownModule")

		if match then
			shutdown_func = "void Shutdown(void);"
		end

		local header_string = gen_header:format(
			include_files,
			module_name,
			module_name,
			module_name,
			init_funcs,
			module_name,
			shutdown_func
		)

		local file_path = include_folder .. "/Gen_ReflectionInit.h"

		-- Don't write to the file if the generated header is the same as what's already on disk.
		-- Prevents build systems from re-building a module that hasn't changed.
		if not os.isfile(file_path) or header_string ~= io.readfile(file_path) then
			io.writefile(file_path, header_string)
		end
	end
}

newaction
{
	trigger = "gen_static_header",
	description = "Generates the 'Gen_ReflectionInit.h' file for a static build.",
	execute = function()
		local dev_modules = {}
		local modules = {}

		local module_generators = os.matchdirs("../src/Modules/*")
		table.foreachi(module_generators, function(dir)
			if not os.isfile(dir .. "/project_generator.lua") then
				return
			end

			if dir:find("Dev") then
				table.insert(dev_modules, dir)
			else
				table.insert(modules, dir)
			end
		end)

		local shutdowns = ""
		local includes = ""
		local inits = ""

		-- Will want to read module unload order and order Shutdown() calls.

		local app_config = io.readfile("../workingdir/cfg/app.cfg")
		local init_order = {}

		if app_config ~= nil and app_config ~= "" then
			init_order = json.decode(app_config).module_load_order or {}
		end

		for _,v in ipairs(init_order) do
			table.foreachi(modules, function(dir)
				local module_name = dir:sub(16)

				if module_name == v then
					includes = includes .. "\t#include <../../Modules/" .. module_name .. "/include/Gen_ReflectionInit.h>\n"
					inits = inits .. "\t\tGAFF_FAIL_RETURN(" .. module_name .. "::Initialize(app, mode), false)\n"

					local match = io.readfile(dir .. "/Shibboleth_" .. module_name .. "Module.cpp"):match("ShutdownModule")

					if match then
						shutdowns = shutdowns .. "\t\t" .. module_name .. "::Shutdown();\n"
					end
				end
			end)
		end


		table.foreachi(modules, function(dir)
			local module_name = dir:sub(16)

			for _,v in ipairs(init_order) do
				if module_name == v then
					return
				end
			end

			includes = includes .. "\t#include <../../Modules/" .. module_name .. "/include/Gen_ReflectionInit.h>\n"
			inits = inits .. "\t\tGAFF_FAIL_RETURN(" .. module_name .. "::Initialize(app, mode), false)\n"

			local match = io.readfile(dir .. "/Shibboleth_" .. module_name .. "Module.cpp"):match("ShutdownModule")

			if match then
				shutdowns = shutdowns .. "\t\t" .. module_name .. "::Shutdown();\n"
			end
		end)

		if shutdowns:len() > 0 then
			shutdowns = shutdowns:sub(1, -2)
		end

		local source_folder = "../src/Engine/Engine"
		local include_folder = source_folder .. "/include"

		local file_class_map = {}

		function ParseFile(file)
			local stripped_file = file:sub(include_folder:len() + 2)

			if file == "Gen_ReflectionInit.h" then
				return
			end

			local lines = io.readfile(file):explode("\n")
			local last_namespace = ""

			for _, line in next, lines do
				if line:match("#define") then
					goto continue
				end

				local match = line:match("NS_(.+)")

				if match then
					if match:find("END") == nil then
						last_namespace = match:sub(1, 1) .. match:sub(2, -2):lower() .. "::"
					end

					goto continue
				end

				-- Detect classes
				match = line:match("SHIB_REFLECTION_DECLARE%((.+)%)")

				if match then
					if not file_class_map[stripped_file] then
						file_class_map[stripped_file] = {}
					end

					table.insert(file_class_map[stripped_file], last_namespace .. match)
					goto continue
				end

				::continue::
			end
		end

		local headers = os.matchfiles(include_folder .. "/**.h")
		table.foreachi(headers, ParseFile)

		local include_files = ""
		local init_funcs = ""

		for k,v in pairs(file_class_map) do
			include_files = include_files .. "#include \"" .. k .. "\"\n"

			for _,c in pairs(v) do
				init_funcs = init_funcs .. "\t\tInitReflectionT<" .. c .. ">(mode);\n"
			end
		end

		local header_string = gen_static:format(
			includes,
			include_files,
			inits,
			shutdowns,
			init_funcs
		)

		local file_path = "../src/Engine/Engine/include/Gen_ReflectionInit.h"

		-- Don't write to the file if the generated header is the same as what's already on disk.
		-- Prevents build systems from re-building a module that hasn't changed.
		if not os.isfile(file_path) or header_string ~= io.readfile(file_path) then
			io.writefile(file_path, header_string)
		end
	end
}
