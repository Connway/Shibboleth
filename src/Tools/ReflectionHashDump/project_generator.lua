ToolProject "ReflectionHashDump"
	debugdir "../../../workingdir/tools"
	language "C++"

	local source_dir = GetToolsSourceDirectory("ReflectionHashDump")
	local base_dir = GetToolsDirectory("ReflectionHashDump")

	files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }

	includedirs
	{
		source_dir .. "../../Engine/Engine/include",
		base_dir .. "../../Engine/Memory/include",
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

	dependson(deps)
	links(deps)

	flags { "FatalWarnings" }

	filter { "system:windows" }
		-- links { "iphlpapi.lib", "psapi.lib", "userenv.lib" }
		links { "Dbghelp" }

	filter { "system:linux" }
		links { "dl", "pthread" }

	filter {}

	postbuildcommands
	{
		"{MKDIR} ../../../../../../workingdir/tools",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../../workingdir/tools"
	}
