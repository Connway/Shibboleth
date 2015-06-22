project "zlib"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	configurations { "Debug", "Release" }
	dofile("../../config_map.lua")

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
