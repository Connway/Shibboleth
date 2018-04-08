project "expat"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.h", "**.c" }

	filter { "action:vs*" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter {}
