project "Bullet3Dynamics"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/dependencies")
	end

	dofile("../../../utils/default_configs.lua")
	dofile("../../../utils/config_map.lua")

	language "C++"
	kind "StaticLib"

	includedirs {
		".."
	}

	files {
		"**.cpp",
		"**.h"
	}
