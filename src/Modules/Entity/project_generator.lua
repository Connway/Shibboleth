group "Modules/Entity"

project "Entity"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/entity")
	end

	kind "StaticLib"
	language "C++"
	rtti "On"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_EntityModule.cpp" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	includedirs
	{
		"include",
		"../../Memory/include",
		"../../Shared/include",
		"../../../dependencies/EASTL/include",
		"../../../dependencies/rapidjson",
		"../../../dependencies/glm",
		"../../../dependencies/libuv/include",
		"../../../dependencies/mpack",
		"../../../frameworks/Gaff/include",
		"../../../frameworks/Gleam/include",
		"../Resource/include"
	}


project "EntityModule"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/entity")
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
		"../../Memory/include",
		"../../Shared/include",
		"../../../dependencies/EASTL/include",
		"../../../dependencies/rapidjson",
		"../../../dependencies/glm",
		"../../../dependencies/libuv/include",
		"../../../dependencies/mpack",
		"../../../frameworks/Gaff/include",
		"../../../frameworks/Gleam/include",
		"../Resource/include"
	}

	local dependencies =
	{
		"Memory",
		"Gaff",
		"Gleam",
		"Shared",
		"EASTL",
		"libuv",
		"Entity",
		"Resource",
		"mpack"
	}

	dependson(dependencies)
	links(dependencies)

	NewDeleteLinkFix()