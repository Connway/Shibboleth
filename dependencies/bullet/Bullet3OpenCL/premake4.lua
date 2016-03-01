function createProject(vendor)
	hasCL = findOpenCL(vendor)

	if (hasCL) then
		project ("Bullet3OpenCL_" .. vendor)
			if _ACTION then
				location ("../../../project/" .. _ACTION .. "/dependencies")
			end

			configurations { "Debug", "Release" }
			dofile("../../../utils/config_map.lua")

			initOpenCL(vendor)

			language "C++"
			kind "StaticLib"

			includedirs {
				".",".."
			}

			files {
				"**.cpp",
				"**.h"
			}

			characterset "MBCS"
	end
end

createProject("clew")
createProject("AMD")
createProject("Intel")
createProject("NVIDIA")
createProject("Apple")
