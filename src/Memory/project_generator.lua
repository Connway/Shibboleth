group "Common"

project "Memory"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/memory")
	end

	kind "SharedLib"
	language "C++"
	defines { "IS_MEMORY", "JEMALLOC_EXPORT=" }

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Shared/include",
		"../../frameworks/Gaff/include",
		"../../dependencies/rpmalloc",
		"../../dependencies/EASTL/include",
		"../../dependencies/dirent"
	}

	dependson { "Gaff", "EASTL", "rpmalloc" }
	links { "Gaff", "EASTL", "rpmalloc" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows", "options:symbols" }
		links { "Dbghelp" }

	filter { "configurations:Debug* or Optimized_Debug*"}
		defines { "JEMALLOC_DEBUG" }

	filter {}

	postbuildcommands
	{
		"{COPY} %{cfg.targetdir}/%{cfg.buildtarget.name} ../../../workingdir/App/bin"
	}

	dofile("../../utils/os_conditionals.lua")
