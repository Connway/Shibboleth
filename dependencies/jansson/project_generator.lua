project "jansson"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	defines { "HAVE_STDINT_H" }
	warnings "Default"

	files { "**.c", "**.h" }

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration {}
