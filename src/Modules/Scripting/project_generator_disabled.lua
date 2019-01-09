project "Scripting"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_ScriptingModule.cpp" }

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../Entity/include",
		"../Resource/include",
		"../../Engine/Memory/include",
		"../../Engine/Engine/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/angelscript/angelscript/include",
		"../../Dependencies/angelscript/add_on/scriptbuilder",
		"../../Dependencies/angelscript/add_on/scriptarray",
		"../../Dependencies/rapidjson",
		"../../Dependencies/glm",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include"
	}



project "ScriptingModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_ScriptingModule.cpp" }

	ModuleGen("Scripting")
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
		"../Entity/include",
		"../Resource/include",
		"../../Dependencies/angelscript/add_on/scriptbuilder",
		"../../Dependencies/angelscript/angelscript/include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/glm",
		"../../Frameworks/Gaff/include"
	}

	local deps =
	{
		"Gaff",
		"Gleam",
		"Memory",
		"Engine",
		"EASTL",
		"angelscript",
		"Resource",
		"Entity",
		"Scripting"
	}

	dependson(deps)
	links(deps)
