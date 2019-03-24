project "ECS"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	defines { "SHIB_STATIC" }

	ModuleGen("ECS")

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Engine/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/glm",
		"../../Dependencies/mpack",
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

	ModuleCopy()

	flags { "FatalWarnings" }

	ModuleIncludesAndLinks("ECS")
	NewDeleteLinkFix()

	local deps =
	{
		"Gleam"
	}

	dependson(deps)
	links(deps)

