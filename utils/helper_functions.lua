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
	local base_dir = ""

	if base_name then
		base_dir = GetModulesDirectory(base_name)
	else
		base_dir = GetModulesDirectory(module_name)
	end

	includedirs
	{
		base_dir .. "include",
		base_dir .. "../../Frameworks/Gaff/include"
	}

	local deps = ModuleDependencies(module_name)
	dependson(deps)
	links(deps)
end

function StaticHeaderGen()
	prebuildmessage("Generating Gen_ReflectionInit.h for static build!")

	filter { "system:windows" }
		prebuildcommands
		{
			"{CHDIR} ../../../../../workingdir/tools",
			"CodeGenerator%{cfg.buildtarget.suffix} static_header"
		}

	filter { "system:not windows" }
		prebuildcommands
		{
			"{CHDIR} ../../../../../workingdir/tools && ./CodeGenerator%{cfg.buildtarget.suffix} static_header"
		}

	filter {}

	dependson { "CodeGenerator" }
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

function ToolGen(tool_name)
	prebuildmessage("Generating Gen_ReflectionInit.h for tool " .. tool_name .. "!")

	filter { "system:windows" }
		prebuildcommands
		{
			"{CHDIR} ../../../../../workingdir/tools",
			"CodeGenerator%{cfg.buildtarget.suffix} tool_header --tool " .. tool_name
		}

	filter { "system:not windows" }
		prebuildcommands
		{
			"{CHDIR} ../../../../../workingdir/tools && ./CodeGenerator%{cfg.buildtarget.suffix} tool_header --tool " .. tool_name
		}

	filter {}

	dependson { "CodeGenerator" }
end

function ModuleGen(module_name)
	prebuildmessage("Generating Gen_ReflectionInit.h for module " .. module_name .. "!")

	filter { "system:windows" }
		prebuildcommands
		{
			"{CHDIR} ../../../../../workingdir/tools",
			"CodeGenerator%{cfg.buildtarget.suffix} module_header --module " .. module_name
		}

	filter { "system:not windows" }
		prebuildcommands
		{
			"{CHDIR} ../../../../../workingdir/tools && ./CodeGenerator%{cfg.buildtarget.suffix} module_header --module " .. module_name
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
		"{MKDIR} ../../../../../workingdir/" .. dir,
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/" .. dir
	}
end

function NewDeleteLinkFix()
	if _ACTION == nil then
		return
	end

	filter { "system:windows" }
		ignoredefaultlibraries { "msvcrt.lib", "msvcrtd.lib" }

	filter { "system:windows" }
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

function GetActionLocation()
	if _ACTION then
		return "../../../.generated/project/" .. os.target() .. "/" .. _ACTION
	else
		return ""
	end
end

function GetDependenciesLocation()
	return GetActionLocation() .. "/dependencies"
end

function GetFrameworkLocation()
	return GetActionLocation() .. "/frameworks"
end

function GetModulesLocation()
	return "../.generated/project/" .. os.target() .. "/" .. _ACTION .. "/modules"
end

function GetEngineLocation()
	return GetActionLocation() .. "/engine"
end

function GetToolsLocation()
	return GetActionLocation() .. "/tools"
end

function GetTestsLocation()
	if _ACTION then
		return "../../.generated/project/" .. os.target() .. "/" .. _ACTION .. "/tests"
	else
		return ""
	end
end

function GetModulesDirectory(module_name)
	return "../src/Modules/" .. module_name .. "/"
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
