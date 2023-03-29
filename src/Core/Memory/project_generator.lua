CoreProject("Memory", "SharedLib")
	language "C++"

	local source_dir = GetCoreSourceDirectory("Memory")
	local base_dir = GetCoreDirectory("Memory")

	files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }
	defines { "IS_MEMORY" }

	IncludeDirs
	{
		source_dir .. "include",
		source_dir .. "../../Core/Engine/include",
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
		"{MKDIR} ../../../../../workingdir/bin",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
	}

	SetupConfigMap()
