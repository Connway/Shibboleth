project "Memory"
	if _ACTION then
		location(GetEngineLocation())
	end

	kind "SharedLib"
	language "C++"
	defines { "IS_MEMORY" }

	files { "**.h", "**.cpp", "**.inl", "**.lua" }

	includedirs
	{
		"include",
		"../Engine/include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/rpmalloc",
		"../../Dependencies/EASTL/include"
	}

	dependson { "Gaff", "EASTL", "rpmalloc" }
	links { "Gaff", "EASTL", "rpmalloc" }

	flags { "FatalWarnings" }

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
