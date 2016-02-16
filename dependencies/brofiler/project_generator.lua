if _OPTIONS["brofiler"] and _OS == "windows" then

	project "brofiler"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/dependencies")
		end

		configurations { "Debug", "Release" }
		dofile("../../utils/config_map.lua")

		kind "SharedLib"
		language "C++"
		warnings "Default"

		files
		{
			"**.cpp",
			"**.h",
			"**.asm"
		}

		defines { "PROFILER_EXPORTS" }
		removeflags { "Unicode" }

end
