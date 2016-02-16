project "minizip"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C"
	warnings "Default"

	files { "**.c", "**.h" }
	includedirs { "../zlib" }

	configuration "not windows"
		excludes { "iowin32.h", "iowin32.c" }

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration {}
