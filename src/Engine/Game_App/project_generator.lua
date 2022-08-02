local proj_kind = "WindowedApp"

if _OPTIONS["console-app"] then
	proj_kind = "ConsoleApp"
end

EngineProject("Game_App", proj_kind)
	debugdir "../../../workingdir/bin"
	language "C++"

	local source_dir = GetEngineSourceDirectory("Game_App")
	local base_dir = GetEngineDirectory("Game_App")

	files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

	IncludeDirs
	{
		source_dir .. "../Engine/include",
		base_dir .. "../Memory/include",
		base_dir .. "../../Frameworks/Gaff/include",
		base_dir .. "../../Dependencies/rapidjson",
		base_dir .. "../../Dependencies/EASTL/include"
	}

	local deps =
	{
		"Engine",
		"Gaff",
		"Gleam",
		"Memory",
		"EASTL",
		"mpack"
	}

	dependson("BuildModuleLibraries")
	dependson(deps)
	links(deps)

	flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "Dbghelp", "iphlpapi", "psapi", "userenv" }

	--filter { "system:not windows" }
	--	linkoptions { "-Wl,-rpath,./bin" }

	filter { "configurations:*Static*" }
		defines { "SHIB_STATIC" }
		StaticLinks()

	filter { "configurations:not *Release*" }
		debugargs { "\"cfg\\dev.cfg\"" }

	filter {}

	postbuildcommands
	{
		"{MKDIR} ../../../../../../workingdir/bin",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../../workingdir/bin"
	}
