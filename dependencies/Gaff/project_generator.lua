project "Gaff"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	configurations { "Debug", "Release" }
	dofile("../../config_map.lua")

	kind "StaticLib"
	language "C++"

	configuration "Debug"
		defines { "ENET_DEBUG" }

	configuration "windows"
		includedirs { "../dirent" }
		defines { "_CRT_SECURE_NO_WARNINGS" }

	configuration {}

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../jansson",
		"../enet/include",
		"../ResIL/IL/include",
		"../ResIL/include",
		"../assimp/include",
		"../utf8-cpp"
	}
