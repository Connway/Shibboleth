DependencyProject "libtiff"
	language "C++"
	warnings "Default"

	files { "**.c", "**.h", "**.cxx" }
	includedirs { "port", "../zlib-ng", "../zstd" }
	removefiles { "libtiff/tif_win32.c", "libtiff/mkspans.c" }

	filter { "system:windows" }
		files { "libtiff/tif_win32.c" }
		removefiles { "libtiff/tif_unix.c" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	--filter { "action:not vs*", "toolset:gcc or clang" }
	--	buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()
