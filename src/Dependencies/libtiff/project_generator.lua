project "libtiff"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.c", "**.h" }
	includedirs { "../libjpeg", "../zlib-ng" }
	excludes { "tif_win32.c" }

	configuration "windows"
		files { "tif_win32.c" }
		excludes { "tif_unix.c" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter { "action:gmake", "toolset:gcc or clang" }
		buildoptions { "-fPIC" }

	filter {}
