group "Graphics Modules"

project "RenderPipelines"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/renderpipelines")
	end

	dofile("../../utils/module_suffix.lua")

	kind "SharedLib"
	language "C++"

	flags { "FatalWarnings" }

	targetname "ShibbolethRenderPipelines"

	files { "**.h", "**.cpp", "**.inl" }

	filter { "system:windows" }
		includedirs { "../../dependencies/dirent" }

	filter {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Game/include",
		"../Managers/include",
		"../Components/include",
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include",
		"../../frameworks/esprit/include",
		"../../dependencies/rapidjson",
		"../../dependencies/utf8-cpp",
		"../../dependencies/assimp/include"
	}

	dependson
	{
		"Shared", "Gaff",
		"Gleam", "Memory",
		"Managers", "Components",
		"Game", "assimp",
		"esprit"
	}

	links
	{
		"Shared", "Gaff",
		"Gleam", "Memory",
		"Managers", "Components",
		"Game", "assimp",
		"esprit"
	}

	dofile("../../utils/os_conditionals.lua")
