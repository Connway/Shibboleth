project "BulletDynamics"
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
		"Dynamics/*.cpp",
                "Dynamics/*.h",
                "ConstraintSolver/*.cpp",
                "ConstraintSolver/*.h",
                "Featherstone/*.cpp",
                "Featherstone/*.h",
                "MLCPSolvers/*.cpp",
                "MLCPSolvers/*.h",
                "Vehicle/*.cpp",
                "Vehicle/*.h",
                "Character/*.cpp",
                "Character/*.h"
	}