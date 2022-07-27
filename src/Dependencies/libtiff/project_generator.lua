DepProject "libtiff"
	language "C"
	warnings "Default"

	files { "**.c", "**.h" }
	IncludeDirs { "port", "../zlib-ng" }
	excludes { "tif_win32.c" }

	filter { "system:windows" }
		files { "tif_win32.c" }
		excludes { "tif_unix.c" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	--filter { "action:not vs*", "toolset:gcc or clang" }
	--	buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()
