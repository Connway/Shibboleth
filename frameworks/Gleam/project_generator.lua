project "Gleam"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/frameworks")
	end

	dofile("../../utils/default_configs.lua")
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"

	filter { "options:simd" }
		defines { "USE_SIMD" }

	filter { "options:simd_set_aligned"}
		defines { "SIMD_SET_ALIGNED" }

	filter { "system:windows", "configurations:not *Clang" }
		flags { "FatalWarnings" }

	filter { "system:not windows" }
		flags { "FatalWarnings" }

	filter {}

	configuration "windows"
		includedirs { "../../dependencies/dirent" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration {}

	defines { "GLEW_STATIC" }

	files { "**.h", "**.cpp" }

	includedirs
	{
		"include",
		"../Gaff/include",
		"../../dependencies/glew/include",
		"../../dependencies/EASTL/include"
	}
