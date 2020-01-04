local gen_header = [[
// This file is generated! Any modifications will be lost in subsequent builds!

#pragma once

#ifdef SHIB_STATIC
// Includes
%s
#include <Shibboleth_Reflection.h>

namespace %s::Gen
{
	void InitReflection(void)
	{
		// Initialize Enums.
		Gaff::InitEnumReflection();

%s
		// Initialize Attributes.
		Gaff::InitAttributeReflection();

%s
		// Initialize regular classes.
		Gaff::InitClassReflection();

%s
		// Register our module as the owners of the reflection.
%s	}
}
#endif

namespace Shibboleth
{
	class IApp;
}

namespace %s
{
	bool Initialize(Shibboleth::IApp& app);
	%s
}
]]

local gen_entry = [[
/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
		bool Initialize(Shibboleth::IApp& app)
		{
			Shibboleth::SetApp(app);
			%s::Gen::InitReflection();

			return true;
		}
	}

#else

	#include <Gaff_Defines.h>

	DYNAMICEXPORT_C bool InitModule(Shibboleth::IApp& app)
	{
		return %s::Initialize(app);
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

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC" }

		SetupConfigMap()
		ModuleGen("%s")

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
]]

local gen_static = [[
// This file is generated! Any modifications will be lost in subsequent builds!

#pragma once

// Includes
#undef SHIB_STATIC

#ifdef SHIB_EDITOR
%s
#endif

%s
#include "Shibboleth_Utilities.h"
#include <Gaff_Defines.h>

#define SHIB_STATIC

namespace Gen
{
	bool LoadModulesStatic(void)
	{
		Shibboleth::IApp& app = Shibboleth::GetApp();

		// Editor Modules
#ifdef SHIB_EDITOR
%s
#endif

		// Regular Modules
%s
		return true;
	}

	bool ShutdownModulesStatic(void)
	{
#ifdef SHIB_EDITOR
%s
#endif

		// Regular Modules
%s
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
			gen_entry:format(module_name, module_name)
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
		local file_enum_map = {}

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

				-- Detect enums
				match = line:match("SHIB_ENUM_REFLECTION_DECLARE%((.+)%)")

				if match then
					print(stripped_file)
					if not file_enum_map[stripped_file] then
						file_enum_map[stripped_file] = {}
					end

					table.insert(file_enum_map[stripped_file], last_namespace .. match)
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
		local init_attr_funcs = ""
		local init_enum_funcs = ""
		local init_funcs = ""
		local module_registers = ""
		local shutdown_func = ""

		for k,v in pairs(file_class_map) do
			include_files = include_files .. "#include <" .. k .. ">\n"

			for _,c in pairs(v) do
				if c:endswith("Attribute") then
					init_attr_funcs = init_attr_funcs .. "\t\tShibboleth::Reflection<" .. c .. ">::Init();\n"
				else
					init_funcs = init_funcs .. "\t\tShibboleth::Reflection<" .. c .. ">::Init();\n"
				end

				if module_registers == "" then
					module_registers = "\t\tShibboleth::ReflectionManager& refl_mgr = Shibboleth::GetApp().getReflectionManager();\n\n"
				end

				module_registers = module_registers .. "\t\trefl_mgr.registerOwningModule(Shibboleth::Reflection<" .. c .. ">::GetHash(), \"" .. _OPTIONS["module"] .. "\");\n"
			end
		end

		for k,v in pairs(file_enum_map) do
			include_files = include_files .. "#include <" .. k .. ">\n"

			for _,e in pairs(v) do
				init_enum_funcs = init_enum_funcs .. "\t\tShibboleth::Reflection<" .. e .. ">::Init();\n"

				if module_registers == "" then
					module_registers = "\t\tShibboleth::ReflectionManager& refl_mgr = Shibboleth::GetApp().getReflectionManager();\n\n"
				end

				module_registers = module_registers .. "\t\trefl_mgr.registerEnumOwningModule(Shibboleth::Reflection<" .. e .. ">::GetHash(), \"" .. _OPTIONS["module"] .. "\");\n"
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
			init_enum_funcs,
			init_attr_funcs,
			init_funcs,
			module_registers,
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
		local editor_modules = {}
		local modules = {}

		local module_generators = os.matchdirs("../src/Modules/*")
		table.foreachi(module_generators, function(dir)
			if not os.isfile(dir .. "/project_generator.lua") then
				return
			end

			if dir:find("Editor") then
				table.insert(editor_modules, dir)
			else
				table.insert(modules, dir)
			end
		end)

		local editor_shutdowns = ""
		local editor_includes = ""
		local editor_inits = ""
		local shutdowns = ""
		local includes = ""
		local inits = ""

		-- Will want to read module unload order and order Shutdown() calls.

		table.foreachi(editor_modules, function(dir)
			local module_name = dir:sub(16)
			editor_includes = editor_includes .. "\t#include <../../Modules/" .. module_name .. "/include/Gen_ReflectionInit.h>\n"
			editor_inits = editor_inits .. "\t\tGAFF_FAIL_RETURN(" .. module_name .. "::Initialize(app), false)\n"

			local match = io.readfile(dir .. "/Shibboleth_" .. module_name .. "Module.cpp"):match("ShutdownModule")

			if match then
				editor_shutdowns = editor_shutdowns .. "\t\t" .. module_name .. "::Shutdown();\n"
			end
		end)

		table.foreachi(modules, function(dir)
			local module_name = dir:sub(16)
			includes = includes .. "#include <../../Modules/" .. module_name .. "/include/Gen_ReflectionInit.h>\n"
			inits = inits .. "\t\tGAFF_FAIL_RETURN(" .. module_name .. "::Initialize(app), false)\n"

			local match = io.readfile(dir .. "/Shibboleth_" .. module_name .. "Module.cpp"):match("ShutdownModule")

			if match then
				shutdowns = shutdowns .. "\t\t" .. module_name .. "::Shutdown();\n"
			end
		end)

		if editor_shutdowns:len() > 0 then
			editor_shutdowns = editor_shutdowns:sub(1, -2)
		end

		if editor_includes:len() > 0 then
			editor_includes = editor_includes:sub(1, -2)
		end

		if editor_inits:len() > 0 then
			editor_inits = editor_inits:sub(1, -2)
		end

		if shutdowns:len() > 0 then
			shutdowns = shutdowns:sub(1, -2)
		end

		local header_string = gen_static:format(
			editor_includes,
			includes,
			editor_inits,
			inits,
			editor_shutdowns,
			shutdowns
		)

		local file_path = "../src/Engine/Engine/include/Gen_ReflectionInit.h"

		-- Don't write to the file if the generated header is the same as what's already on disk.
		-- Prevents build systems from re-building a module that hasn't changed.
		if not os.isfile(file_path) or header_string ~= io.readfile(file_path) then
			io.writefile(file_path, header_string)
		end
	end
}
