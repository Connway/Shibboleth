project "Esprit"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/frameworks")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"

	filter { "options:simd" }
		defines { "USE_SIMD" }

	filter { "options:simd_set_aligned"}
		defines { "SIMD_SET_ALIGNED" }

	filter {}

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../Gleam/include",
		"../../dependencies/utf8-cpp"
	}
