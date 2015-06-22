project "enet"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	configurations { "Debug", "Release" }
	dofile("../../config_map.lua")

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.c", "**.h" }
	includedirs { "include" }

	configuration "Debug"
		defines { "ENET_DEBUG" }

	configuration {}

	filter { "system:windows" }
		defines { "_WINSOCK_DEPRECATED_NO_WARNINGS" }

	filter { "system:not windows" }
		defines { "HAS_SOCKLEN_T" }

	filter {}
