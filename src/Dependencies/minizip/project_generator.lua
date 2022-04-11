project "minizip"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.c", "**.h" }
	includedirs { "../zlib-ng" }

	filter { "system:not windows" }
		excludes { "iowin32.h", "iowin32.c" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter {}

	SetupConfigMap()
