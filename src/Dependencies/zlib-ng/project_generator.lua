project "zlib-ng"
	if _ACTION then
		location(GetDependenciesLocation())
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.h", "**.c" }
	excludes { "arch/aarch64/*.*", "arch/arm/*.*" }

	includedirs { "." }

	defines { "WITH_GZFILEOP", "Z_LARGE64" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	-- filter { "action:gmake", "toolset:gcc or clang" }
	-- 	buildoptions { "-fPIC" }

	filter {}