group "Game Library"

project "Game"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/game")
	end

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl" }

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }

	filter {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Managers/include",
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include",
		"../../frameworks/esprit/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/rapidjson",
		"../../dependencies/LuaState",
		"../../dependencies/LuaJIT/src",
		"../../dependencies/utf8-cpp",
		"../../dependencies/assimp/include",
		"../../dependencies/minizip",
		"../../dependencies/zlib",
		"../../dependencies/bullet"
	}

	dofile("../../utils/os_conditionals.lua")
