group "Game Library"

project "Game"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/game")
	end

	configurations { "Debug", "Release" }
	dofile("../../config_map.lua")

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Managers/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/jansson",
		"../../dependencies/Gleam/include",
		"../../dependencies/Gaff/include",
		"../../dependencies/LuaState",
		"../../dependencies/LuaJIT-2.0.3/src",
		"../../dependencies/utf8-cpp",
		"../../dependencies/assimp/include",
		"../../dependencies/esprit/include",
		"../../dependencies/minizip",
		"../../dependencies/zlib"
	}
