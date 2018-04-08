project "Editor_App"
	if _ACTION then
		location(GetEngineLocation())
	end

	debugdir "../../../workingdir"
	kind "WindowedApp"
	language "C++"
	targetname "Editor"

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/nana/include"
	}

	local deps =
	{
		"Shared",
		"Gaff",
		"Memory",
		"EASTL",
		"libjpeg",
		"libpng",
		"zlib-ng",
		"nana"
	}

	dependson(deps)
	links(deps)

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }
		includedirs { "../../Dependencies/dirent" }
		links { "Dbghelp" }

	filter { "system:not windows" }
		linkoptions { "-Wl,-rpath,./bin" }

	filter {}

	postbuildcommands
	{
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../workingdir"
	}
