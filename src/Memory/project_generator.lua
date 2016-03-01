group "Common"

project "Memory"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/memory")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "SharedLib"
	language "C++"
	defines { "IS_MEMORY" }

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Shared/include",
		"../../dependencies/Gaff/include",
		"../../dependencies/utf8-cpp",
		"../../dependencies/dirent"
	}

	dependson { "Shared", "Gaff" }
	links { "Shared", "Gaff" }

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

	filter {}

	dofile("../../utils/os_conditionals.lua")
