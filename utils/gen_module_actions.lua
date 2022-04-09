local gen_entry = [[
/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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
				Refl::InitEnumReflection();

				// Initialize Attributes.
				Refl::InitAttributeReflection();
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
