project "Gaff"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/frameworks")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	configuration "Debug"
		defines { "ENET_DEBUG" }

	configuration "windows"
		includedirs { "../../dependencies/dirent" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration {}

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../../dependencies/enet/include",
		"../../dependencies/ResIL/IL/include",
		"../../dependencies/ResIL/include",
		"../../dependencies/assimp/include",
		"../../dependencies/utf8-cpp",
		"../../dependencies/boxer/include",
		"../../dependencies/rapidjson"
	}
