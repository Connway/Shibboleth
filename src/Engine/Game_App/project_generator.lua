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
		"Memory",
		"EASTL"
	}

	dependson(deps)
	links(deps)

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "iphlpapi.lib", "psapi.lib", "userenv.lib" }
		includedirs { "../../Dependencies/dirent" }
		links { "Dbghelp" }

	filter { "system:not windows" }
		linkoptions { "-Wl,-rpath,./bin" }

	filter {}

	postbuildcommands
	{
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../workingdir"
	}
