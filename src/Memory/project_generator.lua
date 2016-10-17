group "Common"

project "Memory"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/memory")
	end

	dofile("../../utils/default_configs.lua")
	dofile("../../utils/config_map.lua")

	kind "SharedLib"
	language "C++"
	defines { "IS_MEMORY", "JEMALLOC_EXPORT=" }

	flags { "FatalWarnings" }

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

	filter { "action:vs*" }
		includedirs { "../../dependencies/jemalloc/include/msvc_compat" }

	filter { "system:windows", "options:symbols" }
		links { "Dbghelp" }

	filter { "configurations:Debug", "platforms:x86" }
		targetsuffix "32d"

	filter { "configurations:Release", "platforms:x86" }
		targetsuffix "32"

	filter { "configurations:Debug", "platforms:x64" }
		targetsuffix "64d"

	filter { "configurations:Release", "platforms:x64" }
		targetsuffix "64"

	filter { "configurations:Debug*"}
		defines { "JEMALLOC_DEBUG" }

	filter {}

	dofile("../../utils/os_conditionals.lua")
