project "Scene"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_SceneModule.cpp" }

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Engine/include",
		"../../Dependencies/EASTL/include",
		-- "../../Dependencies/rapidjson",
		-- "../../Dependencies/glm",
		-- "../../Dependencies/mpack",
		"../../Frameworks/Gaff/include",
		-- "../../Frameworks/Gleam/include"
	}

project "SceneModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_SceneModule.cpp" }

	ModuleGen("Scene")
	ModuleCopy()

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Engine/include",
		"../../Dependencies/EASTL/include",
		-- "../../Dependencies/rapidjson",
		-- "../../Dependencies/glm",
		"../../Dependencies/mpack",
		"../../Frameworks/Gaff/include",
		-- "../../Frameworks/Gleam/include"
	}

	local deps =
	{
		"Memory",
		"Engine",
		"EASTL",
		"Gaff",
		-- "Gleam",
		"Scene",
		"mpack"
	}

	dependson(deps)
	links(deps)

	NewDeleteLinkFix()
