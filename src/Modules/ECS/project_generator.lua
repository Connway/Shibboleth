project "ECS"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_ECSModule.cpp" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Shared/include",
		"../../Dependencies/EASTL/include",
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

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Shared/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/glm",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include"
	}

	local deps =
	{
		"Memory",
		"Shared",
		"EASTL",
		"Gaff",
		"Gleam",
		"ECS"
	}

	dependson(deps)
	links(deps)

	NewDeleteLinkFix()