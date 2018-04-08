project "Memory"
	if _ACTION then
		location(GetEngineLocation())
	end

	kind "SharedLib"
	language "C++"
	defines { "IS_MEMORY" }

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Shared/include",
		"../../Frameworks/Gaff/include",
		"../../Dependencies/rpmalloc",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/dirent"
	}

	dependson { "Gaff", "EASTL", "rpmalloc" }
	links { "Gaff", "EASTL", "rpmalloc" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows", "options:symbols" }
		links { "Dbghelp" }

	filter {}

	postbuildcommands
	{
		"{MKDIR} ../../../../workingdir/bin",
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../../workingdir/bin"
	}
