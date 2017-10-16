group "Modules/World"

project "World"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/resource")
	end

	kind "StaticLib"
	language "C++"
	rtti "On"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_WorldModule.cpp" }

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


project "WorldModule"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/resource")
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_WorldModule.cpp" }

	ModuleGen("World")
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
		"Resource", "Memory",
		"Gaff", "Shared",
		"EASTL", "libuv"
	}

	links
	{
		"Resource", "Memory",
		"Gaff", "Shared",
		"EASTL", "libuv"
	}
