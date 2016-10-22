project "BulletInverseDynamics"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/dependencies")
	end

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
