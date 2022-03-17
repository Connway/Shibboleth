project "Memory"
	if _ACTION then
		location(GetEngineLocation())
	end

	kind "SharedLib"
	language "C++"
	defines { "IS_MEMORY" }

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Engine/include",
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

	filter { "configurations:*Debug* or *Optimized_Debug* or *Profile*" }
		dependson({ "TracyClient" })
		links({ "TracyClient" })

	filter { "configurations:*Debug* or *Optimized_Debug*" }
		defines { "CHECK_FOR_DOUBLE_FREE", "CHECK_FOR_LEAKS", "CHECK_FOR_MISALIGNED_POINTER" }

	filter { "configurations:*Release* or *Profile*" }
		defines { "CHECK_FOR_LEAKS" }

	filter { "system:windows"--[[, "options:symbols"--]] }
		links { "Dbghelp" }

	-- filter { "system:windows" }
	-- 	links { "iphlpapi.lib", "psapi.lib", "userenv.lib" }

	filter {}

	postbuildcommands
	{
		"{MKDIR} ../../../../../workingdir/bin",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../../workingdir/bin"
	}

	SetupConfigMap()
