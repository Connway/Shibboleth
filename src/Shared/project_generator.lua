group "Common"

project "Shared"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/shared")
	end

	configurations { "Debug", "Release" }
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
		"../../dependencies/Gaff/include",
		"../../dependencies/utf8-cpp"
	}

	dofile("../../utils/os_conditionals.lua")
