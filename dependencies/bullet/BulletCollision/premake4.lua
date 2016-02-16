project "BulletCollision"
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
		"*.h",
		"BroadphaseCollision/*.cpp",
		"BroadphaseCollision/*.h",
		"CollisionDispatch/*.cpp",
		"CollisionDispatch/*.h",
		"CollisionShapes/*.cpp",
		"CollisionShapes/*.h",
		"Gimpact/*.cpp",
		"Gimpact/*.h",
		"NarrowPhaseCollision/*.cpp",
		"NarrowPhaseCollision/*.h",
	}
