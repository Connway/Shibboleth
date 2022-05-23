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

#include "CodeGen_ModuleGenerator.h"
#include "CodeGen_ReflectionHeaderGenerator.h"
#include "CodeGen_Utils.h"
#include <Gaff_Utils.h>
#include <Gaff_File.h>
#include <argparse.hpp>
#include <filesystem>

MSVC_DISABLE_WARNING_PUSH(4189)
#include <fmt/core.h>
MSVC_DISABLE_WARNING_POP()

static constexpr const char/*8_t*/* k_gen_module_code =
/*u8*/R"(#include "Gen_ReflectionInit.h"

#ifdef SHIB_STATIC

	#include <Shibboleth_IModule.h>

	namespace {}
	{{
		class Module final : public Shibboleth::IModule
		{{
		public:
			void initReflectionEnums(void) override;
			void initReflectionAttributes(void) override;
			void initReflectionClasses(void) override;
		}};

		void Module::initReflectionEnums(void)
		{{
			// Should NOT add other code here.
			Gen::{}::InitReflection(InitMode::Enums);
		}}

		void Module::initReflectionAttributes(void)
		{{
			// Should NOT add other code here.
			Gen::{}::InitReflection(InitMode::Attributes);
		}}

		void Module::initReflectionClasses(void)
		{{
			// Should NOT add other code here.
			Gen::{}::InitReflection(InitMode::Classes);
		}}

		Shibboleth::IModule* CreateModule(void)
		{{
			return SHIB_ALLOCT({}::Module, Shibboleth::ProxyAllocator("Input"));
		}}
	}}

#else

	#include <Gaff_Defines.h>

	DYNAMICEXPORT_C Shibboleth::IModule* CreateModule(void)
	{{
		return {}::CreateModule();
	}}

#endif
)";

static constexpr const char/*8_t*/* k_gen_module_project =
/*u8*/R"(local GenerateProject = function()
	local base_dir = GetModulesDirectory("{}")

	project "{}"
		location(GetModulesLocation())

		kind "StaticLib"
		language "C++"

		files {{ base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }}
		defines {{ "SHIB_STATIC" }}

		ModuleGen("{}")
		SetupConfigMap()

		flags {{ "FatalWarnings" }}

		includedirs
		{{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include"
		}}

	project "{}Module"
		location(GetModulesLocation())

		kind "SharedLib"
		language "C++"

		files {{ base_dir .. "Shibboleth_{}Module.cpp" }}

		ModuleCopy()

		flags {{ "FatalWarnings" }}

		ModuleIncludesAndLinks("{}")
		NewDeleteLinkFix()
		SetupConfigMap()

		local deps =
		{{
		}}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("{}")

	dependson(deps)
	links(deps)
end

return {{ GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }}
)";

static int CreateProjectFiles(const std::string& path, const std::string& name, const argparse::ArgumentParser& program)
{
	std::string prefix = "";

	if (program.is_used(k_arg_prefix)) {
		prefix = program.get(k_arg_prefix) + "_";
	}

	const std::string final_module_text = GetLicenseText(program) + fmt::format(
		k_gen_module_code,
		name.data(),
		name.data(),
		name.data(),
		name.data(),
		name.data(),
		name.data()
	);

	std::string gen_file_path = path + "/" + prefix + name + "Module.cpp";
	Gaff::File gen_file(gen_file_path.data(), Gaff::File::OpenMode::Write);

	if (!gen_file.isOpen()) {
		std::cerr << "Failed to open output file '" << reinterpret_cast<const char*>(gen_file_path.data()) << "'." << std::endl;
		return -6;
	}

	if (!gen_file.writeString(final_module_text.data())) {
		std::cerr << "Failed to write to output file '" << reinterpret_cast<const char*>(gen_file_path.data()) << "'." << std::endl;
		return -7;
	}

	gen_file.close();

	gen_file_path = path + "/project_generator.lua";
	
	if (!gen_file.open(gen_file_path.data(), Gaff::File::OpenMode::Write)) {
		std::cerr << "Failed to open output file '" << reinterpret_cast<const char*>(gen_file_path.data()) << "'." << std::endl;
		return -6;
	}

	const std::string final_project_text = fmt::format(
		k_gen_module_project,
		name.data(),
		name.data(),
		name.data(),
		name.data(),
		name.data(),
		name.data(),
		name.data()
	);

	if (!gen_file.writeString(final_project_text.data())) {
		std::cerr << "Failed to write to output file '" << reinterpret_cast<const char*>(gen_file_path.data()) << "'." << std::endl;
		return -7;
	}

	return 0;
}

void ModuleGenerator_AddArguments(argparse::ArgumentParser& program)
{
	program.add_argument(k_arg_force_module_overwrite, k_arg_force_module_overwrite_short)
		.help("Force overwrites already existing modules.")
		.default_value(false)
		.implicit_value(true);

	program.add_argument(k_arg_prefix, k_arg_prefix_short)
		.help("Prefix for generated module file.");
}

int ModuleGenerator_Run(const argparse::ArgumentParser& program)
{
	const std::string action = program.get("action");
	std::string name;
	std::string path;

	if (action == "create_module") {
		if (!program.is_used(k_arg_module)) {
			std::cerr << "Did not provide '--module' parameter." << std::endl;
			return -2;
		}

		name = program.get(k_arg_module);
		path = "src/Modules/" + name;

	} else if (action == "create_tool") {
		if (!program.is_used(k_arg_tool)) {
			std::cerr << "Did not provide '--tool' parameter." << std::endl;
			return -2;
		}

		name = program.get(k_arg_tool);
		path = "src/Tools/" + name;
	
	} else {
		std::cerr << "Unknown action '" << action << "'." << std::endl;
		return -3;
	}

	if (std::filesystem::exists(path) && !program.get<bool>(k_arg_force_module_overwrite)) {
		std::cerr << "'" << path << "' already exists." << std::endl;
		return -4;
	}

	if (const auto out_dir = std::filesystem::absolute(path).u8string(); !Gaff::CreateDir(out_dir.data(), 0777)) {
		std::cerr << "Failed to create output directory '" << reinterpret_cast<const char*>(out_dir.data()) << "'." << std::endl;
		return -5;
	}

	if (const auto out_dir = std::filesystem::absolute(path).u8string() + u8"/include"; !Gaff::CreateDir(out_dir.data(), 0777)) {
		std::cerr << "Failed to create output directory '" << reinterpret_cast<const char*>(out_dir.data()) << "'." << std::endl;
		return -5;
	}

	return CreateProjectFiles(path, name, program);
}
