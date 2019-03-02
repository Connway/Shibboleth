project "Resource"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_ResourceModule.cpp" }

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Engine/include",
		"../../Dependencies/EASTL/include",
		"../../Frameworks/Gaff/include"
	}


project "ResourceModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_ResourceModule.cpp" }

	ModuleGen("Resource")
	ModuleCopy()

	flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }

	filter {}

	NewDeleteLinkFix()

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Engine/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Frameworks/Gaff/include"
	}

	local deps =
	{
		"Memory",
		"Gaff",
		"Engine",
		"EASTL",
		"mpack",
		"Resource"
	}

	dependson(deps)
	links(deps)
