project "ResIL"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	dofile("../../utils/default_configs.lua")
	dofile("../../utils/config_map.lua")

	kind "SharedLib"
	language "C"
	warnings "Default"

	files { "**.c", "**.cpp", "**.h" }

	excludes
	{
		"ILU/**.cpp",
		"ILU/**.c",
		"ILU/**.h",
		"IL/src/il_skia.cpp"
	}

	includedirs
	{
		"IL/include",
		"include",
		"../libtiff",
		"../libjpeg",
		"../libpng"
	}

	dependson { "libtiff", "libjpeg", "libpng", "zlib" }
	links { "libtiff", "libjpeg", "libpng", "zlib" }

	filter { "configurations:Debug", "platforms:x86" }
		targetsuffix "32d"

	filter { "configurations:Release", "platforms:x86" }
		targetsuffix "32"

	filter { "configurations:Debug", "platforms:x64" }
		targetsuffix "64d"

	filter { "configurations:Release", "platforms:x64" }
		targetsuffix "64"

	filter {}

project "ResILU"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	dofile("../../utils/default_configs.lua")
	dofile("../../utils/config_map.lua")

	kind "SharedLib"
	language "C"
	warnings "Default"

	files { "ILU/**.c", "ILU/**.cpp", "ILU/**.h" }
	includedirs { "ILU/include", "include" }

	dependson { "ResIL" }
	links { "ResIL" }

	filter { "configurations:Debug", "platforms:x86" }
		targetsuffix "32d"

	filter { "configurations:Release", "platforms:x86" }
		targetsuffix "32"

	filter { "configurations:Debug", "platforms:x64" }
		targetsuffix "64d"

	filter { "configurations:Release", "platforms:x64" }
		targetsuffix "64"

	filter {}
