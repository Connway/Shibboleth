group "Common"

project "Shared"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/shared")
	end

	dofile("../../utils/default_configs.lua")
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }

	filter { "options:simd" }
		defines { "USE_SIMD" }

	filter { "options:simd_set_aligned"}
		defines { "SIMD_SET_ALIGNED" }

	filter {}

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../Memory/include",
		"../../dependencies/rapidjson",
		"../../frameworks/Gaff/include",
		"../../dependencies/EASTL/include"
	}

	dofile("../../utils/os_conditionals.lua")
