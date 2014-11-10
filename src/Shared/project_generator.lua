project "Shared"
	if _ACTION then
		location ("../../project/" .. _ACTION)
	end

	kind "StaticLib"
	language "C++"

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

	filter { "options:simd" }
		defines { "USE_SIMD" }

	filter { "options:simd_set_aligned"}
		defines { "SIMD_SET_ALIGNED" }

	filter {}

	files { "**.h", "**.cpp", "**.inl" }
	excludes { "include/Shibboleth_Memory.h", "Shibboleth_Memory.cpp" }

	includedirs
	{
		"include",
		"../../dependencies/jansson",
		"../../dependencies/Gaff/include",
		"../../dependencies/utf8-cpp"
	}

project "Memory"
	if _ACTION then
		location ("../../project/" .. _ACTION)
	end

	kind "SharedLib"
	language "C++"
	defines { "IS_MEMORY" }

	files { "include/Shibboleth_Memory.h", "Shibboleth_Memory.cpp" }

	includedirs
	{
		"include",
		"../../dependencies/Gaff/include"
	}

	filter { "configurations:Debug", "platforms:x86" }
		targetsuffix "32d"

	filter { "configurations:Release", "platforms:x86" }
		targetsuffix "32"

	filter { "configurations:Debug", "platforms:x64" }
		targetsuffix "64d"

	filter { "configurations:Release", "platforms:x64" }
		targetsuffix "64"

	filter {}
