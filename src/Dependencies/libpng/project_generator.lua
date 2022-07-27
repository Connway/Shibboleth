DepProject "libpng"
	language "C"
	warnings "Default"

	files { "**.h", "**.c" }
	IncludeDirs { "../zlib-ng" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter { "configurations:*Debug*" }
		defines { "PNG_DEBUG" }

	--filter { "action:not vs*", "toolset:gcc or clang" }
	--	buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()
