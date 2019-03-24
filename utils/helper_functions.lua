function ModuleIncludesAndLinks(module_name)
	includedirs
	{
		"include",
		"../../Frameworks/Gaff/include",
	}

	local deps =
	{
		"Memory",
		"Engine",
		"EASTL",
		"Gaff",
		"mpack",
		module_name
	}

	dependson(deps)
	links(deps)
end

function ModuleGen(module_name)
	prebuildmessage("Generating Gen_ReflectionInit.h for module " .. module_name .. "!")
	prebuildcommands
	{
		"cd ../../../../../utils",
		"premake5 gen_module_header --module=" .. module_name
	}
end

function ModuleCopy()
	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/Modules",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/Modules"
	}
end

function ModuleEditorCopy()
	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/EditorModules",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/EditorModules"
	}
end

function NewDeleteLinkFix()
	if _ACTION == nil then
		return
	end

	filter { "system:windows" }
		ignoredefaultlibraries { "msvcrt.lib", "msvcrtd.lib" }

	filter { "system:windows", "configurations:Debug*", "platforms:x64" }
		links
		{
			"../../../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/Debug/Engine.lib",
			"msvcrtd.lib"
		}

	filter { "system:windows", "configurations:Release*", "platforms:x64" }
		links
		{
			"../../../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/Release/Engine.lib",
			"msvcrt.lib"
		}

	filter { "system:windows", "configurations:Optimized_Debug*", "platforms:x64" }
		links
		{
			"../../../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/Debug/Engine.lib",
			"msvcrtd.lib"
		}

	filter { "system:windows", "configurations:Profile*", "platforms:x64" }
		links
		{
			"../../../.generated/build/" .. os.target() .. "/" .. _ACTION .. "/output/x64/Release/Engine.lib",
			"msvcrt.lib"
		}

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
	return GetActionLocation() .. "/modules"
end

function GetEngineLocation()
	return GetActionLocation() .. "/engine"
end

function GetTestsLocation()
	if _ACTION then
		return "../../.generated/project/" .. os.target() .. "/" .. _ACTION .. "/tests"
	else
		return ""
	end
end

function RunFile(file)
	dofile(file)
end

function IncludeWxWidgets()
	includedirs { "../../Dependencies/wxWidgets/include" }
	defines
	{
		"NOPCH",
		-- "WXBUILDING",
		"wxUSE_ZLIB_H_IN_PATH",
		-- "wxUSE_GUI=1",
		-- "wxUSE_BASE=1",
		"WXUSINGDLL"
	}

	filter { "system:windows" }
		includedirs { "../../Dependencies/wxWidgets/include/msvc" }
		defines { "__WXMSW__" }

	filter {}
end
