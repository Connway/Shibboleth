group "Common"

project "Game"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/game")
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }

	filter {}

	removefiles { "disable/**.*" }

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Managers/include",
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include",
		"../../frameworks/esprit/include",
		"../../dependencies/rapidjson",
		"../../dependencies/assimp/include",
		"../../dependencies/minizip",
		"../../dependencies/zlib",
		"../../dependencies/bullet",
		"../../dependencies/EASTL/include",
		"../../dependencies/glm"
	}

	dofile("../../utils/os_conditionals.lua")
