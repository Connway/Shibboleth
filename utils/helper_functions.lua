function ModuleDependencies(module_name)
	return {
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
		source_dir .. "../../Engine/Memory/include",
		source_dir .. "../../Engine/Engine/include",
		base_dir .. "../../Dependencies/EASTL/include"
	}

	local deps = ModuleDependencies(module_name)
	dependson(deps)
	links(deps)
end

function StaticLinks()
	local ProcessModule = function(dir)
		if not os.isfile(dir .. "/project_generator.lua") then
			return
		end

		local funcs = dofile(dir .. "/project_generator.lua")
		funcs.LinkDependencies()
	end

	local module_generators = os.matchdirs("../../Modules/*")
	table.foreachi(module_generators, ProcessModule)
end

function ModuleCopy(dir)
	if dir == nil then
		dir = "Modules"
	end

	postbuildcommands
	{
		"{MKDIR} ../../../../../../workingdir/" .. dir,
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../../workingdir/" .. dir
	}
end

function TestCopy()
	postbuildcommands
	{
		"{MKDIR} ../../../../../../workingdir/tests",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../../workingdir/tests"
	}
end

function NewDeleteLinkFix()
	filter { "system:windows" }
		ignoredefaultlibraries { "msvcrt.lib", "msvcrtd.lib" }

		links
		{
			"../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/%{cfg.platform}/%{cfg.buildcfg}/Engine/Engine%{cfg.buildtarget.suffix}.lib",
		}

	filter { "system:windows", "configurations:*Debug*" }
		links { "msvcrtd.lib" }

	filter { "system:windows", "configurations:not *Debug*" }
		links { "msvcrt.lib" }

	filter {}
end

function GetActionLocation(no_preproc)
	if no_preproc == nil then
		no_preproc = false
	end

	if no_preproc == false and _OPTIONS["generate-preproc"] then
		return "../../../.generated/preproc/project/" .. os.target() .. "/" .. _ACTION
	else
		return "../../../.generated/project/" .. os.target() .. "/" .. _ACTION
	end
end

function GetDependenciesLocation(no_preproc)
	if no_preproc == nil then
		no_preproc = false
	end

	return GetActionLocation(no_preproc) .. "/dependencies"
end

function GetFrameworkLocation(no_preproc)
	if no_preproc == nil then
		no_preproc = false
	end

	return GetActionLocation(no_preproc) .. "/frameworks"
end

function GetModulesLocation()
	if _OPTIONS["generate-preproc"] then
		return "../.generated/preproc/project/" .. os.target() .. "/" .. _ACTION .. "/modules"
	else
		return "../.generated/project/" .. os.target() .. "/" .. _ACTION .. "/modules"
	end
end

function GetEngineLocation(no_preproc)
	if no_preproc == nil then
		no_preproc = false
	end

	return GetActionLocation(no_preproc) .. "/engine"
end

function GetToolsLocation(no_preproc)
	if no_preproc == nil then
		no_preproc = false
	end

	return GetActionLocation(no_preproc) .. "/tools"
end

function GetTestsLocation(no_preproc)
	if no_preproc == nil then
		no_preproc = false
	end

	return GetActionLocation(no_preproc) .. "/tests"
end

function GetModulesSourceDirectory(module_name)
	if _OPTIONS["generate-preproc"] then
		return "../.generated/preproc/Modules/" .. module_name .. "/"
	else
		return "../src/Modules/" .. module_name .. "/"
	end
end

function GetModulesGeneratedDirectory(module_name)
	return "../.generated/preproc/Modules/" .. module_name .. "/"
end

function GetModulesDirectory(module_name)
	return "../src/Modules/" .. module_name .. "/"
end

function GetEngineSourceDirectory(module_name)
	if _OPTIONS["generate-preproc"] then
		return "../../../.generated/preproc/Engine/" .. module_name .. "/"
	else
		return "../../Engine/" .. module_name .. "/"
	end
end

function GetEngineGeneratedDirectory(module_name)
	return "../../../.generated/preproc/Engine/" .. module_name .. "/"
end

function GetEngineDirectory(module_name)
	return "../../Engine/" .. module_name .. "/"
end

function GetToolsSourceDirectory(tool_name)
	if _OPTIONS["generate-preproc"] then
		return "../../../.generated/preproc/Tools/" .. tool_name .. "/"
	else
		return "../../Tools/" .. tool_name .. "/"
	end
end

function GetToolsGeneratedDirectory(tool_name)
	return "../../../.generated/preproc/Tools/" .. tool_name .. "/"
end

function GetToolsDirectory(tool_name)
	return "../../Tools/" .. tool_name .. "/"
end

function GetTestsSourceDirectory(test_name)
	--if _OPTIONS["generate-preproc"] then
	--	return "../../.generated/preproc/Tests/" .. test_name .. "/"
	--else
		return "../../../src/Tests/" .. test_name .. "/"
	--end
end

function GetTestsGeneratedDirectory()
	return "../../.generated/preproc/Tests/"
end

function GetTestsDirectory(test_name)
	return "../../../src/Tests/" .. test_name .. "/"
end

function RunFile(file)
	dofile(file)
end

function SetupConfigMap()
	configmap
	{
		["Static_Debug_D3D11"] = "Debug",
		["Static_Release_D3D11"] = "Release"
	}
end

function QtSettingsModule(modules, base_dir, source_dir)
	if base_dir == nil then
		base_dir = ""
	end

	if source_dir == nil then
		source_dir = base_dir
	end

	defines { "QT_DISABLE_DEPRECATED_BEFORE=0x060000" }

	qtgenerateddir(source_dir .. ".generated/" .. os.target())
	qtprefix "Qt6"

	qtmodules(modules)

	filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Debug", "platforms:x64" }
		qtsuffix "d"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Release", "platforms:x64" }
	-- 	qtsuffix "64"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Profile", "platforms:x64" }
	-- 	qtsuffix "64p"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Optimized_Debug", "platforms:x64" }
	-- 	qtsuffix "64od"

	filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Static_Debug*", "platforms:x64" }
		qtsuffix "d"

	-- filter { "kind:SharedLib or WindowedApp or ConsoleApp", "configurations:Static_Release*", "platforms:x64" }
	-- 	qtsuffix "64s"

	filter {}
end

function QtSettings(modules, base_dir, source_dir)
	if base_dir == nil then
		base_dir = ""
	end

	if source_dir == nil then
		source_dir = base_dir
	end

	files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.qrc", source_dir .. "**.ui" }
	excludes { "**/moc*.*" }

	QtSettingsModule(modules, base_dir, source_dir)
end

local all_dependencies = {}
local all_frameworks = {}
local all_module_libs = {}
local all_modules = {}
local all_engine = {}
local all_tools = {}

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

function GetAllEngine()
	return all_engine
end

function GetAllTools()
	return all_tools
end

function FrameworkProject(project_name, project_kind, no_preproc)
	if no_preproc == nil then
		no_preproc = false
	end

	table.insert(all_frameworks, project_name)

	project(project_name)
		location(GetFrameworkLocation(no_preproc))

		if _OPTIONS["generate-preproc"] or no_preproc then
			kind(project_kind or "StaticLib")
		else
			kind "None"
		end

		for _,v in ipairs(configs) do
			filter { "configurations:" .. v, "platforms:x64" }
				targetdir("../../../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/" .. v .. "/" .. project_name)
		end

		filter {}
end

function DepProject(project_name, project_kind, no_preproc)
	if no_preproc == nil then
		no_preproc = false
	end

	table.insert(all_dependencies, project_name)

	project(project_name)
		location(GetDependenciesLocation(no_preproc))

		if _OPTIONS["generate-preproc"] or no_preproc then
			kind(project_kind or "StaticLib")
		else
			kind "None"
		end

		for _,v in ipairs(configs) do
			filter { "configurations:" .. v, "platforms:x64" }
				targetdir("../../../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/" .. v .. "/" .. project_name)
		end

		filter {}
end

function ToolProject(project_name, project_kind, no_preproc)
	if no_preproc == nil then
		no_preproc = false
	end

	table.insert(all_tools, project_name)

	project(project_name)
		location(GetToolsLocation(no_preproc))

		if _OPTIONS["generate-preproc"] or no_preproc then
			kind(project_kind or "ConsoleApp")
		else
			kind "None"
		end

		for _,v in ipairs(configs) do
			filter { "configurations:" .. v, "platforms:x64" }
				targetdir("../../../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/" .. v .. "/" .. project_name)
		end

		filter {}
end

function EngineProject(project_name, project_kind, no_preproc)
	if no_preproc == nil then
		no_preproc = false
	end

	table.insert(all_engine, project_name)

	project(project_name)
		location(GetEngineLocation(no_preproc))

		if _OPTIONS["generate-preproc"] or no_preproc then
			kind(project_kind or "StaticLib")
		else
			kind "None"
		end

		for _,v in ipairs(configs) do
			filter { "configurations:" .. v, "platforms:x64" }
				targetdir("../../../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/" .. v .. "/" .. project_name)
		end

		filter {}
end

function ModuleProject(project_name, project_kind)
	if (project_name:sub(-6, -1) == "Module") then
		table.insert(all_modules, project_name)
	else
		table.insert(all_module_libs, project_name)
	end

	project(project_name)
		location(GetModulesLocation())

		if _OPTIONS["generate-preproc"] then
			kind(project_kind or "StaticLib")
		else
			kind "None"
		end

		for _,v in ipairs(configs) do
			filter { "configurations:" .. v, "platforms:x64" }
				targetdir("../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/" .. v .. "/" .. project_name)
		end

		filter {}
end

function TestProject(project_name, project_kind)
	table.insert(all_tools, project_name)

	project(project_name)
		location(GetTestsLocation())

		if _OPTIONS["generate-preproc"] then
			kind(project_kind or "ConsoleApp")
		else
			kind "None"
		end

		for _,v in ipairs(configs) do
			filter { "configurations:" .. v, "platforms:x64" }
				targetdir("../../../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/" .. v .. "/" .. project_name)
		end

		filter {}
end

function Group(group_name)
	--if not _OPTIONS["generate-preproc"] then
		group(group_name)
	--end
end

function IncludeDirs(dirs)
	if not _OPTIONS["generate-preproc"] then
		sysincludedirs(dirs)

		-- Swap over to externalincludedirs when 5.0 beta2 comes out.
		--externalincludedirs(dirs)
	else
		includedirs(dirs)
	end
end
