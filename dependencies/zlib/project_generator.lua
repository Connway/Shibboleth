project "zlib"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.h", "**.c" }

	defines { "Z_LARGE64" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter { "action:gmake", "toolset:gcc or clang" }
		buildoptions { "-fPIC" }

	filter {}
