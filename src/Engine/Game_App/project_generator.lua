project "Game_App"
	if _ACTION then
		location(GetEngineLocation())
	end

	if _OPTIONS["console_app"] then
		kind "ConsoleApp"
	else
		kind "WindowedApp"
	end

	debugdir "../../../workingdir"
	language "C++"

	files { "**.h", "**.cpp", "**.inl", "**.lua" }

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
		links { "iphlpapi.lib", "psapi.lib", "userenv.lib" }
		links { "Dbghelp" }

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
