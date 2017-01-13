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

	includedirs
	{
		"include",
		"../Memory/include",
		"../Shared/include",
		"../Game/include",
		"../Resource/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/angelscript/angelscript/include",
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

	includedirs
	{
		"include",
		"../Memory/include",
		"../Shared/include",
		"../Game/include",
		"../Resource/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/rapidjson",
		"../../frameworks/Gaff/include"
	}

	dependson
	{
		"Scripting", "Memory",
		"Gaff", "Shared",
		"EASTL", "angelscript",
		"Game", "Resource"
	}

	links
	{
		"Scripting", "Memory",
		"Gaff", "Shared",
		"EASTL", "angelscript",
		"Game", "Resource"
	}
