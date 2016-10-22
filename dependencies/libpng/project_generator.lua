project "libpng"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.h", "**.c" }
	includedirs { "../zlib" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter { "configurations:Debug* or Optimized_Debug*" }
		defines { "PNG_DEBUG" }

	filter { "action:gmake", "toolset:gcc or clang" }
		buildoptions { "-fPIC" }

	filter {}
