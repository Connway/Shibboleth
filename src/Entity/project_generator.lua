group "Modules/Entity"

project "Entity"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/entity")
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
		"../Memory/include",
		"../Shared/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/rapidjson",
		"../../dependencies/glm",
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include"
	}


project "EntityModule"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/entity")
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_EntityModule.cpp" }

	ModuleGen("Entity")
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
		"Entity", "Memory",
		"Gaff", "Shared",
		"EASTL"
	}

	links
	{
		"Entity", "Memory",
		"Gaff", "Shared",
		"EASTL"
	}
