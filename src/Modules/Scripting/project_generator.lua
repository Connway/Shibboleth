group "Modules/Scripting"

project "Scripting"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/scripting")
	end

	kind "StaticLib"
	language "C++"
	rtti "On"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_ScriptingModule.cpp" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	includedirs
	{
		"include",
		"../Entity/include",
		"../Resource/include",
		"../../Memory/include",
		"../../Shared/include",
		"../../../dependencies/EASTL/include",
		"../../../dependencies/angelscript/angelscript/include",
		"../../../dependencies/angelscript/add_on/scriptbuilder",
		"../../../dependencies/angelscript/add_on/scriptarray",
		"../../../dependencies/rapidjson",
		"../../../dependencies/glm",
		"../../../dependencies/libuv/include",
		"../../../frameworks/Gaff/include",
		"../../../frameworks/Gleam/include"
	}



project "ScriptingModule"
	if _ACTION then
		location ("../../../project/" .. _ACTION .. "/scripting")
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_ScriptingModule.cpp" }

	ModuleGen("Scripting")
	ModuleCopy("Scripting")

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
		"../Entity/include",
		"../Resource/include",
		"../../../dependencies/angelscript/add_on/scriptbuilder",
		"../../../dependencies/angelscript/angelscript/include",
		"../../../dependencies/EASTL/include",
		"../../../dependencies/rapidjson",
		"../../../dependencies/glm",
		"../../../dependencies/libuv/include",
		"../../../frameworks/Gaff/include"
	}

	dependson
	{
		"Gaff",
		"Gleam",
		"Scripting",
		"Memory",
		"Shared",
		"EASTL",
		"angelscript",
		"Resource",
		"Entity",
		"libuv"
	}

	links
	{
		"Gaff",
		"Gleam",
		"Scripting",
		"Memory",
		"Shared",
		"EASTL",
		"angelscript",
		"Resource",
		"Entity",
		"libuv"
	}
