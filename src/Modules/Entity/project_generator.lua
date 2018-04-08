project "Entity"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_EntityModule.cpp" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Shared/include",
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

	ModuleGen("Entity")
	ModuleCopy()

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }

	filter {}

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Shared/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/glm",
		"../../Dependencies/mpack",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include",
		"../Resource/include"
	}

	local deps =
	{
		"Memory",
		"Gaff",
		"Gleam",
		"Shared",
		"EASTL",
		"Entity",
		"Resource",
		"mpack"
	}

	dependson(deps)
	links(deps)

	NewDeleteLinkFix()
