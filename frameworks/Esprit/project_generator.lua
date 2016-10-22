project "Esprit"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/frameworks")
	end

	kind "StaticLib"
	language "C++"

	filter { "options:simd" }
		defines { "USE_SIMD" }

	filter { "options:simd_set_aligned"}
		defines { "SIMD_SET_ALIGNED" }

	filter { "configurations:not *Clang", "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../Gleam/include",
		"../../dependencies/EASTL/include"
	}
