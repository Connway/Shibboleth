----
-- Location Functions
----
function GetGeneratedLocation()
	if _OPTIONS["preproc-pipeline"] and _OPTIONS["generate-preproc"] then
		return "%{wks.location}/../../../.."
	else
		return "%{wks.location}/../../.."
	end
end

function GetPreprocProjectLocation()
	return GetGeneratedLocation() .. "/preproc/project/" .. os.target() .. "/" .. _ACTION
end

function GetProjectLocation()
	return GetGeneratedLocation() .. "/project/" .. os.target() .. "/" .. _ACTION
end

function GetProjectLocationSupportPreproc(no_preproc)
	if no_preproc == nil then
		no_preproc = false
	end

	if no_preproc == false and _OPTIONS["preproc-pipeline"] and _OPTIONS["generate-preproc"] then
		return GetPreprocProjectLocation()
	else
		return GetProjectLocation()
	end
end

function GetDependenciesLocation()
	return GetProjectLocation() .. "/dependencies"
end

function GetFrameworksLocation()
	return GetProjectLocation() .. "/frameworks"
end

function GetModulesLocation()
	return GetProjectLocationSupportPreproc() .. "/modules"
end

function GetCoreLocation()
	return GetProjectLocationSupportPreproc() .. "/core"
end

function GetToolsLocation(no_preproc)
	return GetProjectLocationSupportPreproc(no_preproc) .. "/tools"
end

function GetTestsLocation()
	return GetProjectLocationSupportPreproc() .. "/tests"
end

function GetBuildLocation()
	return GetGeneratedLocation() .. "/build/" .. os.target() .. "/" .. _ACTION
end

function GetIntermediateLocation()
	return GetBuildLocation() .. "/intermediate"
end

function GetOutputLocation()
	return GetBuildLocation() .. "/output/%{cfg.platform}/%{cfg.buildcfg}/%{prj.name}"
end

function GetWorkingLocation()
	return GetGeneratedLocation() .. "/../workingdir"
end

function GetBinLocation()
	return GetWorkingLocation() .. "/bin"
end

function GetModulesGeneratedDirectory(module_name)
	if module_name == nil then
		return GetGeneratedLocation() .. "/preproc/Modules/"
	else
		return GetGeneratedLocation() .. "/preproc/Modules/" .. module_name .. "/"
	end
end

function GetModulesDirectory(module_name)
	if module_name == nil then
		return GetGeneratedLocation() .. "/../src/Modules/"
	else
		return GetGeneratedLocation() .. "/../src/Modules/" .. module_name .. "/"
	end
end

function GetModulesSourceDirectory(module_name)
	if _OPTIONS["preproc-pipeline"] and _OPTIONS["generate-preproc"] then
		return GetModulesGeneratedDirectory(module_name)
	else
		return GetModulesDirectory(module_name)
	end
end

function GetCoreGeneratedDirectory(core_name)
	if core_name == nil then
		return GetGeneratedLocation() .. "/preproc/Core/"
	else
		return GetGeneratedLocation() .. "/preproc/Core/" .. core_name .. "/"
	end
end

function GetCoreDirectory(core_name)
	if core_name == nil then
		return GetGeneratedLocation() .. "/../src/Core/"
	else
		return GetGeneratedLocation() .. "/../src/Core/" .. core_name .. "/"
	end
end

function GetCoreSourceDirectory(core_name)
	if _OPTIONS["preproc-pipeline"] and _OPTIONS["generate-preproc"] then
		return GetCoreGeneratedDirectory(core_name)
	else
		return GetCoreDirectory(core_name)
	end
end

function GetToolsGeneratedDirectory(tool_name)
	if tool_name == nil then
		return GetGeneratedLocation() .. "/preproc/Tools/"
	else
		return GetGeneratedLocation() .. "/preproc/Tools/" .. tool_name .. "/"
	end
end

function GetToolsDirectory(tool_name)
	if tool_name == nil then
		return GetGeneratedLocation() .. "/../src/Tools/"
	else
		return GetGeneratedLocation() .. "/../src/Tools/" .. tool_name .. "/"
	end
end

function GetToolsSourceDirectory(tool_name)
	if _OPTIONS["preproc-pipeline"] and _OPTIONS["generate-preproc"] then
		return GetToolsGeneratedDirectory(tool_name)
	else
		return GetToolsDirectory(tool_name)
	end
end

function GetTestsGeneratedDirectory(test_name)
	if test_name == nil then
		return GetGeneratedLocation() .. "/preproc/Tools/"
	else
		return GetGeneratedLocation() .. "/preproc/Tools/" .. test_name .. "/"
	end
end

function GetTestsDirectory(test_name)
	if test_name == nil then
		return GetGeneratedLocation() .. "/../src/Tests/"
	else
		return GetGeneratedLocation() .. "/../src/Tests/" .. test_name .. "/"
	end
end

function GetTestsSourceDirectory(test_name)
	if _OPTIONS["preproc-pipeline"] and _OPTIONS["generate-preproc"] then
		return GetTestsGeneratedDirectory(test_name)
	else
		return GetTestsDirectory(test_name)
	end
end

function GetDependenciesDirectory()
	return GetGeneratedLocation() .. "/../src/Dependencies
end



----
-- Project Functions
----
local all_dependencies = {}
local all_frameworks = {}
local all_module_libs = {}
local all_modules = {}
local all_core = {}
local all_tools = {}
local all_tests = {}

function GetAllModuleLibraries()
	return all_dependencies
end

function GetAllDependencies()
	return all_dependencies
end

function GetAllFrameworks()
	return all_frameworks
end

function GetAllModules()
	return all_modules
end

function GetAllCore()
	return all_core
end

function GetAllTools()
	return all_tools
end

function GetAllTools()
	return all_tests
end

function ProjectCommon(project_name, project_kind)
	project(project_name)
		kind(project_kind or "StaticLib")

		files { "project_generator.lua" }
end

function DependencyProject(project_name, project_kind)
	table.insert(all_dependencies, project_name)

	ProjectCommon(project_name, project_kind)
		location(GetDependenciesLocation())
		SetupConfigMap()
end

function FrameworkProject(project_name, project_kind)
	table.insert(all_frameworks, project_name)

	ProjectCommon(project_name, project_kind)
		location(GetFrameworksLocation())

		includedirs { "include" }
		flags { "FatalWarnings" }

		SetupConfigMap()
end

function CoreProject(project_name, project_kind)
	table.insert(all_core, project_name)

	if not _OPTIONS["preproc-pipeline"] or _OPTIONS["generate-preproc"] or no_preproc then
		project_kind = project_kind or "StaticLib"
	else
		project_kind = "None"
	end

	ProjectCommon(project_name, project_kind)
		location(GetCoreLocation())

		includedirs { "include" }
		flags { "FatalWarnings" }

		SetupConfigMap()

		if (project_kind == "WindowedApp") or (project_kind == "ConsoleApp") then
			postbuildcommands
			{
				"{MKDIR} " .. GetBinLocation(),
				"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} " .. GetBinLocation()
			}
		end
end

function ModuleProject(project_name, base_name, copy_dir)
	local lib_name = project_name
	local project_kind = "StaticLib"
	local is_base = (base_name and project_name:sub(-4, -1) == "Base")
	local is_lib = true

	if (project_name:sub(-6, -1) == "Module") then
		table.insert(all_modules, project_name)

		lib_name = project_name:sub(1, -7)
		project_kind = "SharedLib"
		is_lib = false

	else
		table.insert(all_module_libs, project_name)
	end

	if not _OPTIONS["preproc-pipeline"] or _OPTIONS["generate-preproc"] then
	else
		project_kind = "None"
	end

	local source_dir = ""
	--local base_dir = ""

	if base_name then
		source_dir = GetModulesSourceDirectory(base_name)
		--base_dir = GetModulesDirectory(base_name)
	else
		source_dir = GetModulesSourceDirectory(project_name)
		--base_dir = GetModulesDirectory(project_name)
	end

	project(project_name)
		kind(project_kind)

		location(GetModulesLocation())

		if is_lib then
			if not _OPTIONS["preproc-pipeline"] or _OPTIONS["generate-preproc"] then
				if not base_name or is_base then
					ModuleGen(base_name or project_name)
				end
			end

			files { source_dir .. "/project_generator.lua" }
			defines { "SHIB_STATIC" }

			if is_base then
				defines { "SHIB_IS_BASE" }
			end

		elseif not is_lib then
			ModuleIncludesAndLinks(lib_name, base_name)
			NewDeleteLinkFix()
			ModuleCopy(copy_dir)
		end

		flags { "FatalWarnings" }
		SetupConfigMap()
end

function DevModuleProject(project_name, base_name)
	ModuleProject(project_name, base_name, "DevModules")
end

function ToolProject(project_name, project_kind, no_preproc)
	table.insert(all_tools, project_name)

	if not _OPTIONS["preproc-pipeline"] or _OPTIONS["generate-preproc"] or no_preproc then
		project_kind = project_kind or "ConsoleApp"
	else
		project_kind = "None"
	end

	ProjectCommon(project_name, project_kind)
		location(GetToolsLocation(no_preproc))

		includedirs { "include" }
		flags { "FatalWarnings" }

		SetupConfigMap()
		ToolCopy()
end

function TestProject(project_name, project_kind)
	table.insert(all_tests, project_name)

	if not _OPTIONS["preproc-pipeline"] or _OPTIONS["generate-preproc"] or no_preproc then
		project_kind = project_kind or "ConsoleApp"
	else
		project_kind = "None"
	end

	ProjectCommon(project_name, project_kind)
		location(GetTestsLocation())

		includedirs { "include" }
		flags { "FatalWarnings" }

		SetupConfigMap()
		TestCopy()
end

function IncludeDirs(dirs)
	if _OPTIONS["preproc-pipeline"] and not _OPTIONS["generate-preproc"] then
		externalincludedirs(dirs)
	else
		includedirs(dirs)
	end
end

function Group(group_name)
	if not _OPTIONS["preproc-pipeline"] or not _OPTIONS["generate-preproc"] then
		group(group_name)
	end
end

function SetupConfigMap()
	configmap
	{
		["Static_Debug_D3D11"] = "Debug",
		["Static_Release_D3D11"] = "Release"
	}
end

function NewDeleteLinkFix()
	filter { "system:windows" }
		ignoredefaultlibraries { "msvcrt.lib", "msvcrtd.lib" }

		links
		{
			GetOutputLocation() .. "/../Engine/Engine%{cfg.buildtarget.suffix}.lib",
		}

	filter { "system:windows", "configurations:*Debug*" }
		links { "msvcrtd.lib" }

	filter { "system:windows", "configurations:not *Debug*" }
		links { "msvcrt.lib" }

	filter {}
end

function ModuleDependencies(module_name)
	return
	{
		"Memory",
		"Engine",
		"EASTL",
		"Gaff",
		"mpack",
		module_name
	}
end

function ModuleIncludesAndLinks(module_name, base_name)
	local source_dir = ""
	local base_dir = ""

	if base_name then
		source_dir = GetModulesSourceDirectory(base_name)
		base_dir = GetModulesDirectory(base_name)
	else
		source_dir = GetModulesSourceDirectory(module_name)
		base_dir = GetModulesDirectory(module_name)
	end

	IncludeDirs
	{
		source_dir .. "include",
		base_dir .. "../../Frameworks/Gaff/include",
		source_dir .. "../../Core/Memory/include",
		source_dir .. "../../Core/Engine/include",
		base_dir .. "../../Dependencies/EASTL/include"
	}

	local deps = ModuleDependencies(module_name)
	dependson(deps)
	links(deps)
end

function ModuleGen(module_name)
	if _OPTIONS["preproc-pipeline"] then
		return
	end

	prebuildmessage("Generating Gen_ReflectionInit.h for module " .. module_name .. "!")

	filter { "system:windows" }
		prebuildcommands
		{
			"{CHDIR} " .. GetWorkingLocation() .. "/tools",
			"CodeGenerator%{cfg.buildtarget.suffix} module_header --module " .. module_name
		}

	filter { "system:macosx" }
		prebuildcommands
		{
			"cd " .. GetWorkingLocation() .. "/tools && ./CodeGenerator%{cfg.buildtarget.suffix} module_header --module " .. module_name
		}

	filter { "system:linux" }
		prebuildcommands
		{
			"$(SHELL) $(.SHELLFLAGS) -e \"cd " .. GetWorkingLocation() .. "/tools && ./CodeGenerator%{cfg.buildtarget.suffix} module_header --module " .. module_name .. "\""
		}

	filter {}

	dependson { "CodeGenerator" }
end

function StaticHeaderGen()
	if _OPTIONS["preproc-pipeline"] then
		return
	end

	prebuildmessage("Generating Gen_StaticReflectionInit.h and Gen_ReflectionInit.h for static build!")

	filter { "system:windows" }
		prebuildcommands
		{
			"{CHDIR} " .. GetWorkingLocation() .. "/tools",
			"CodeGenerator%{cfg.buildtarget.suffix} static_header"
		}

	filter { "system:macosx" }
		prebuildcommands
		{
			"cd " .. GetWorkingLocation() .. "/tools && ./CodeGenerator%{cfg.buildtarget.suffix} static_header"
		}

	filter { "system:linux" }
		prebuildcommands
		{
			"$(SHELL) $(.SHELLFLAGS) -e \"cd " .. GetWorkingLocation() .. "/tools && ./CodeGenerator%{cfg.buildtarget.suffix} static_header\""
		}

	filter {}

	dependson { "CodeGenerator" }
end

function ToolGen(tool_name)
	if _OPTIONS["preproc-pipeline"] then
		return
	end

	prebuildmessage("Generating Gen_ReflectionInit.h for tool " .. tool_name .. "!")

	filter { "system:windows" }
		prebuildcommands
		{
			"{CHDIR} " .. GetWorkingLocation() .. "/tools",
			"CodeGenerator%{cfg.buildtarget.suffix} tool_header --tool " .. tool_name
		}

	filter { "system:macosx" }
		prebuildcommands
		{
			"cd " .. GetWorkingLocation() .. "/tools && ./CodeGenerator%{cfg.buildtarget.suffix} tool_header --tool " .. tool_name
		}

	filter { "system:linux" }
		prebuildcommands
		{
			"$(SHELL) $(.SHELLFLAGS) -e \"cd " .. GetWorkingLocation() .. "/tools && ./CodeGenerator%{cfg.buildtarget.suffix} tool_header --tool " .. tool_name .. "\""
		}

	filter {}

	dependson { "CodeGenerator" }
end

function ModuleCopy(dir)
	if dir == nil then
		dir = "Modules"
	end

	postbuildcommands
	{
		"{MKDIR} " .. GetWorkingLocation() .. "/" .. dir,
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} " .. GetWorkingLocation() .. "/" .. dir
	}
end

function TestCopy()
	postbuildcommands
	{
		"{MKDIR} " .. GetWorkingLocation() .. "/tests",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} " .. GetWorkingLocation() .. "/tests"
	}
end

function ToolCopy()
	postbuildcommands
	{
		"{MKDIR} " .. GetWorkingLocation() .. "/tools",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} " .. GetWorkingLocation() .. "/tools"
	}
end

function StaticLinks()
	local ProcessModule = function(dir)
		if not os.isfile(dir .. "/project_generator.lua") then
			return
		end

		local funcs = dofile(dir .. "/project_generator.lua")
		funcs.LinkDependencies()
	end

	local module_generators = os.matchdirs(GetGeneratedLocation() .. "/../src/Modules/*")
	table.foreachi(module_generators, ProcessModule)
end
