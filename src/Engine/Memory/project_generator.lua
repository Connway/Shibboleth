EngineProject("Memory", "SharedLib")
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	defines { "IS_MEMORY" }

	local source_dir = GetEngineSourceDirectory("Memory")
	local base_dir = GetEngineDirectory("Memory")

	IncludeDirs
	{
		source_dir .. "include",
		source_dir .. "../../Engine/Engine/include",
		base_dir .. "../../Frameworks/Gaff/include",
		base_dir .."../../Dependencies/mimalloc/include",
		base_dir .."../../Dependencies/EASTL/include",
		base_dir .."../../Dependencies/tracy"
	}

	local deps =
	{
		"Gaff",
		"EASTL",
		"mimalloc"
	}

	dependson(deps)
	links(deps)

	flags { "FatalWarnings" }

	filter { "configurations:*Debug* or *Profile*" }
		dependson({ "TracyClient" })
		links({ "TracyClient" })

	filter { "configurations:*Debug*" }
		defines { "CHECK_FOR_DOUBLE_FREE", "CHECK_FOR_LEAKS", "CHECK_FOR_MISALIGNED_POINTER" }

	filter { "configurations:*Release* or *Profile*" }
		defines { "CHECK_FOR_LEAKS" }

	filter { "system:windows" }
	-- 	links { "iphlpapi.lib", "psapi.lib", "userenv.lib" }
		links { "Dbghelp" }

	filter {}

	postbuildcommands
	{
		"{MKDIR} ../../../../../../workingdir/bin",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../../workingdir/bin"
	}

	SetupConfigMap()
