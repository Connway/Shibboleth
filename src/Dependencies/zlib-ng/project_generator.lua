project "zlib-ng"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.h", "**.c" }
	excludes { "arch/aarch64/*.*", "arch/arm/*.*", "arch/s390/*.*" }

	includedirs { "." }

	defines { "WITH_GZFILEOP", "Z_LARGE64", "MY_ZCALLOC" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	-- filter { "action:not vs*", "toolset:gcc or clang" }
	-- 	buildoptions { "-fPIC" }

	filter {}

	SetupConfigMap()
