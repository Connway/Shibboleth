EngineProject("Memory", "SharedLib", true)
	language "C++"
	defines { "IS_MEMORY" }

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../../Engine/Engine/include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/mimalloc/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/tracy"
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
		"{MKDIR} ../../../../../workingdir/bin",
		"{COPYFILE} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
	}

	SetupConfigMap()
