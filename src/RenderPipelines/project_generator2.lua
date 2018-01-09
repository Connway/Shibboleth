group "Graphics Modules"

project "RenderPipelines"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/renderpipelines")
	end

	kind "SharedLib"
	language "C++"

	targetname "ShibbolethRenderPipelines"

	files { "**.h", "**.cpp", "**.inl" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

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
