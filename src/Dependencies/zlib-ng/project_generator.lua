DepProject "zlib-ng"
	language "C"
	warnings "Default"

	files { "**.h", "**.c" }

	includedirs { "." }

	defines { "WITH_GZFILEOP" }

	filter { "platforms:x64" }
		excludes { "arch/arm/*.*", "arch/s390/*.*", "arch/power/*.*" }

	filter { "platforms:arm64" }
		excludes { "arch/x86/*.*", "arch/s390/*.*", "arch/power/*.*" }

	filter { "system:windows" }
		defines { "_CRT_SECURE_NO_WARNINGS", "_LARGEFILE64_SOURCE", "_LFS64_LARGEFILE" }

	filter { "system:linux" }
		buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()
