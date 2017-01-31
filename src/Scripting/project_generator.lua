group "Modules/Scripting"

project "Scripting"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/scripting")
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
		"../Memory/include",
		"../Shared/include",
		"../Entity/include",
		"../Resource/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/angelscript/angelscript/include",
		"../../dependencies/angelscript/add_on/scriptbuilder",
		"../../dependencies/rapidjson",
		"../../frameworks/Gaff/include"
	}



project "ScriptingModule"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/scripting")
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_ScriptingModule.cpp" }

	ModuleGen("Scripting")
	ModuleCopy("Scripting")

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	includedirs
	{
		"include",
		"../Memory/include",
		"../Shared/include",
		"../Entity/include",
		"../Resource/include",
		"../../dependencies/angelscript/add_on/scriptbuilder",
		"../../dependencies/angelscript/angelscript/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/rapidjson",
		"../../frameworks/Gaff/include"
	}

	dependson
	{
		"Scripting", "Memory",
		"Gaff", "Shared",
		"EASTL", "angelscript",
		"Resource", "Entity"
	}

	links
	{
		"Scripting", "Memory",
		"Gaff", "Shared",
		"EASTL", "angelscript",
		"Resource", "Entity"
	}
