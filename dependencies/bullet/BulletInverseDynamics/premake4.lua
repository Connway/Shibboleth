project "BulletInverseDynamics"
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
		"IDMath.cpp",
		"MultiBodyTree.cpp",
		"details/MultiBodyTreeInitCache.cpp",
		"details/MultiBodyTreeImpl.cpp",
	}
