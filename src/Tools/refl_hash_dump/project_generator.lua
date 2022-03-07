project "refl_hash_dump"
	if _ACTION then
		location(GetToolsLocation())
	end

	kind "ConsoleApp"
	debugdir "../../../workingdir/tools"
	language "C++"

	files { "**.h", "**.cpp", "**.inl", "**.lua" }

	includedirs
	{
		"../../Engine/Engine/include",
		"../../Engine/Memory/include",
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
		-- links { "iphlpapi.lib", "psapi.lib", "userenv.lib" }
		links { "Dbghelp" }

	filter { "system:not windows" }
		linkoptions { "-Wl,-rpath,../bin" }

	filter {}

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/tools",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/tools"
	}
