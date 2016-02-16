project "LinearMath"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/dependencies")
	end

	configurations { "Debug", "Release" }
	dofile("../../../utils/config_map.lua")

	language "C++"
	kind "StaticLib"
	includedirs {
		"..",
	}
	files {
		"*.cpp",
		"*.h"
	}
