project "Memory"
	location(GetEngineLocation(true))

	kind "SharedLib"
	language "C++"
	defines { "IS_MEMORY" }

	files { "**.h", "**.cpp", "**.inl" }

	local gen_dir = GetEngineGeneratedDirectory("Memory")
	local base_dir = GetEngineDirectory("Memory")

	includedirs
	{
		base_dir .. "include",
		gen_dir .. "../Engine/include",
		base_dir .. "../../Frameworks/Gaff/include",
		base_dir .. "../../Dependencies/mimalloc/include",
		base_dir .. "../../Dependencies/EASTL/include",
		base_dir .. "../../Dependencies/tracy"
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
