project "ResIL"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

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

project "ResILU"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "SharedLib"
	language "C"
	warnings "Default"

	files { "ILU/**.c", "ILU/**.cpp", "ILU/**.h" }
	includedirs { "ILU/include", "include" }

	dependson { "ResIL" }
	links { "ResIL" }
