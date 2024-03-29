project "zlib-ng"
	location(GetDependenciesLocation())

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.h", "**.c" }
	excludes { "arch/arm/*.*", "arch/s390/*.*", "arch/power/*.*" }

	includedirs { "." }

	defines { "WITH_GZFILEOP", "_LARGEFILE64_SOURCE", "_LFS64_LARGEFILE" }

	filter { "system:windows" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter { "system:linux" }
		buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()
