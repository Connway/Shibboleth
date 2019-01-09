project "ECS"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_ECSModule.cpp" }

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Engine/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/glm",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include"
	}

project "ECSModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_ECSModule.cpp" }

	ModuleGen("ECS")
	ModuleCopy()

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Engine/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/glm",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include"
	}

	local deps =
	{
		"Memory",
		"Engine",
		"EASTL",
		"Gaff",
		"Gleam",
		"ECS"
	}

	dependson(deps)
	links(deps)

	NewDeleteLinkFix()
