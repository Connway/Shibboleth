project "Memory"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/memory")
	end

	kind "SharedLib"
	language "C++"
	defines { "IS_MEMORY" }

	files { "include/Shibboleth_Memory.h", "Shibboleth_Memory.cpp" }

	includedirs
	{
		"include",
		"../Shared/include",
		"../../dependencies/Gaff/include"
	}

	dependson { "Shared", "Gaff" }
	links { "Shared", "Gaff" }

	filter { "configurations:Debug", "platforms:x86" }
		targetsuffix "32d"

	filter { "configurations:Release", "platforms:x86" }
		targetsuffix "32"

	filter { "configurations:Debug", "platforms:x64" }
		targetsuffix "64d"

	filter { "configurations:Release", "platforms:x64" }
		targetsuffix "64"

	filter {}
