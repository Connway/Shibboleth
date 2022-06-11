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

	includedirs
	{
		source_dir .. "include",
		base_dir .. "../../Frameworks/Gaff/include",
		base_dir .. "../../Engine/Memory/include",
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

function NewDeleteLinkFix()
	filter { "system:windows" }
		ignoredefaultlibraries { "msvcrt.lib", "msvcrtd.lib" }

		links
		{
			"../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/%{cfg.platform}/%{cfg.buildcfg}/Engine%{cfg.buildtarget.suffix}.lib",
		}

	filter { "system:windows", "configurations:*Debug*" }
		links { "msvcrtd.lib" }

	filter { "system:windows", "configurations:not *Debug*" }
		links { "msvcrt.lib" }

	filter {}
end

function GetActionLocation(no_preproc)
	if no_preproc == false and _OPTIONS["generate-preproc"] then
		return "../../../.generated/preproc/project/" .. os.target() .. "/" .. _ACTION
	else
		return "../../../.generated/project/" .. os.target() .. "/" .. _ACTION
	end
end

function GetDependenciesLocation()
	return GetActionLocation(true) .. "/dependencies"
end

function GetFrameworkLocation()
	return GetActionLocation(true) .. "/frameworks"
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

function GetTestsLocation()
	return "../../.generated/preproc/project/" .. os.target() .. "/" .. _ACTION .. "/tests"
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

function GetTestsSourceDirectory()
	if _OPTIONS["generate-preproc"] then
		return "../../.generated/preproc/Tests/"
	else
		return "../../src/Tests/"
	end
end

function GetTestsGeneratedDirectory()
	return "../../.generated/preproc/Tests/"
end

function GetTestsDirectory()
	return "../../src/Tests/"
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

function GenProject(project_name, project_kind)
	project(project_name)
		if _OPTIONS["generate-preproc"] then
			kind(project_kind or "StaticLib")

		else
			if (project_name:sub(-6, -1) ~= "Module") then
				dependson { "RunProjectBuild" }
			end

			kind "Makefile"

			buildcommands
			{
				"{CHDIR} ../../../../../workingdir/tools",
				"ProjectBuild%{cfg.buildtarget.suffix} build --project " .. project_name .. " --config %{cfg.buildcfg}"
			}

			cleancommands
			{
				"{CHDIR} ../../../../../workingdir/tools",
				"ProjectBuild%{cfg.buildtarget.suffix} build --clean --project " .. project_name .. " --config %{cfg.buildcfg}"
			}
		end
end

function Group(group_name)
	if not _OPTIONS["generate-preproc"] then
		group(group_name)
	end
end
