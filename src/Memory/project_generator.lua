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
		"../../dependencies/jemalloc/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/dirent"
	}

	dependson { "Gaff", "EASTL", "jemalloc" }
	links { "Gaff", "EASTL", "jemalloc" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "action:vs*" }
		includedirs { "../../dependencies/jemalloc/include/msvc_compat" }

	filter { "system:windows", "options:symbols" }
		links { "Dbghelp" }

	filter { "configurations:Debug* or Optimized_Debug*"}
		defines { "JEMALLOC_DEBUG" }

	filter {}

	dofile("../../utils/os_conditionals.lua")
