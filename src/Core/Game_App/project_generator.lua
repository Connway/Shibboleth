local proj_kind = "WindowedApp"

if _OPTIONS["console-app"] then
	proj_kind = "ConsoleApp"
end

CoreProject("Game_App", proj_kind)
	debugdir "../../../workingdir/bin"
	language "C++"

	local source_dir = GetCoreSourceDirectory("Game_App")
	local base_dir = GetCoreDirectory("Game_App")

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

	if _OPTIONS["preproc-pipeline"] then
		dependson("BuildModuleLibraries")
	end

	dependson(deps)
	links(deps)

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
		"{MKDIR} ../../../../../workingdir/bin",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
	}
