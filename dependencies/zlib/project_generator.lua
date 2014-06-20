project "zlib"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.h", "**.c" }

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }

		configuration {}

	filter { "action:gmake" }
		buildoptions { "-fPIC" }

	filter {}
