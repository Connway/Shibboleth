project "libtiff"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.c", "**.h" }
	includedirs { "../libjpeg", "../zlib" }
	excludes { "tif_win32.c" }

	configuration "windows"
		files { "tif_win32.c" }
		excludes { "tif_unix.c" }

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration {}

	filter { "action:gmake" }
		buildoptions { "-fPIC" }

	filter {}