project "Gaff"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "StaticLib"
	language "C++"

	configuration "Debug"
		defines { "ENET_DEBUG" }

	configuration "windows"
		includedirs { "../dirent" }

	configuration {}

	files { "**.h", "**.cpp", "**.inl" }

	includedirs
	{
		"include",
		"../jansson",
		"../enet/include",
		"../ResIL/IL/include",
		"../ResIL/include",
		"../assimp/include"
	}