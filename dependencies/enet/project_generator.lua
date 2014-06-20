project "enet"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.c", "**.h" }
	includedirs { "include" }

	configuration "Debug"
		defines { "ENET_DEBUG" }

	configuration {}

	filter { "system:not windows" }
		defines { "HAS_SOCKLEN_T" }

	filter {}
