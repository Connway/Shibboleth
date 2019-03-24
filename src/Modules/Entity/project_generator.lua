project "Entity"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	defines { "SHIB_STATIC" }

	ModuleGen("Entity")

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
		"../../Frameworks/Gleam/include",
		"../Resource/include"
	}


project "EntityModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_EntityModule.cpp" }

	ModuleCopy()

	flags { "FatalWarnings" }

	ModuleIncludesAndLinks("Entity")
	NewDeleteLinkFix()

	local deps =
	{
		"Gleam",
		"Resource"
	}

	dependson(deps)
	links(deps)
