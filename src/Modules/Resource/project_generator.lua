group "Modules/Resource"

project "Resource"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/resource")
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
		"../../Memory/include",
		"../../Shared/include",
		"../../../dependencies/EASTL/include",
		"../../../dependencies/libuv/include",
		"../../../frameworks/Gaff/include"
	}


project "ResourceModule"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/resource")
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_ResourceModule.cpp" }

	ModuleGen("Resource")
	ModuleCopy()

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter { "system:windows" }
		links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }

	filter {}

	includedirs
	{
		"include",
		"../../Memory/include",
		"../../Shared/include",
		"../../../dependencies/EASTL/include",
		"../../../dependencies/rapidjson",
		"../../../dependencies/libuv/include",
		"../../../frameworks/Gaff/include"
	}

	dependson
	{
		"Memory",
		"Gaff",
		"Shared",
		"EASTL",
		"libuv",
		"Resource"
	}

	links
	{
		"Memory",
		"Gaff",
		"Shared",
		"EASTL",
		"libuv",
		"Resource"
	}
