group "Modules/Resource"

project "Resource"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/resource")
	end

	kind "StaticLib"
	language "C++"
	rtti "On"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_ResourceModule.cpp" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	includedirs
	{
		"include",
		"../Memory/include",
		"../Shared/include",
		"../../dependencies/EASTL/include",
		"../../frameworks/Gaff/include"
	}


project "ResourceModule"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/resource")
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_ResourceModule.cpp" }

	ModuleGen("Resource")
	ModuleCopy()

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	includedirs
	{
		"include",
		"../Memory/include",
		"../Shared/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/rapidjson",
		"../../frameworks/Gaff/include"
	}

	dependson
	{
		"Resource", "Memory",
		"Gaff", "Shared",
		"EASTL"
	}

	links
	{
		"Resource", "Memory",
		"Gaff", "Shared",
		"EASTL"
	}
