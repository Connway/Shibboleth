project "libpng"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.h", "**.c" }
	includedirs { "../zlib" }

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration "Debug"
		defines { "PNG_DEBUG" }

	configuration {}

	filter { "action:gmake" }
		buildoptions { "-fPIC" }

	filter {}
