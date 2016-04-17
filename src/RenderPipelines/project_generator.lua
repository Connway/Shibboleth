group "Graphics Modules"

project "RenderPipelines"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/renderpipelines")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	dofile("../../utils/module_suffix.lua")

	kind "SharedLib"
	language "C++"

	flags { "FatalWarnings" }

	targetname "ShibbolethRenderPipelines"

	files { "**.h", "**.cpp", "**.inl" }

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Game/include",
		"../Managers/include",
		"../Components/include",
		"../../dependencies/rapidjson",
		"../../dependencies/Gleam/include",
		"../../dependencies/Gaff/include",
		"../../dependencies/esprit/include",
		"../../dependencies/utf8-cpp",
		"../../dependencies/assimp/include"
	}

	dependson
	{
		"Shared", "Gaff",
		"Gleam", "Memory",
		"Managers", "Components",
		"Game", "assimp",
		"esprit", "Boxer"
	}

	links
	{
		"Shared", "Gaff",
		"Gleam", "Memory",
		"Managers", "Components",
		"Game", "assimp",
		"esprit", "Boxer"
	}

	dofile("../../utils/os_conditionals.lua")
