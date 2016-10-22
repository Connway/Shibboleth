project "Bullet3Dynamics"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/dependencies")
	end

	language "C++"
	kind "StaticLib"

	includedirs {
		".."
	}

	files {
		"**.cpp",
		"**.h"
	}
