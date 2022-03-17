project "Game_App"
	if _ACTION then
		location(GetEngineLocation())
	end

	filter { "options:console-app" }
		kind "ConsoleApp"

	filter { "options:not console-app" }
		kind "WindowedApp"

	filter {}

	debugdir "../../../workingdir"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"../Engine/include",
		"../Memory/include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/EASTL/include"
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

	dependson(deps)
	links(deps)

	flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "Dbghelp", "iphlpapi", "psapi", "userenv" }

	filter { "system:not windows" }
		linkoptions { "-Wl,-rpath,./bin" }

	filter { "configurations:Static_*" }
		defines { "SHIB_STATIC" }
		StaticLinks()

	filter { "configurations:not Release*" }
		debugargs { "\"cfg\\dev.cfg\"" }

	filter {}

	postbuildcommands
	{
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir"
	}
