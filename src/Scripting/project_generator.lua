group "Scripting"

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
		"../../dependencies/EASTL/include",
		"../../dependencies/chaiscript",
		"../../dependencies/angelscript/angelscript/include",
		"../../dependencies/rapidjson",
		"../../frameworks/Gaff/include"
	}

	filter { "action:vs*" }
		buildoptions { "/bigobj" }

	filter {}

project "ScriptingModule"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/scripting")
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_ScriptingModule.cpp" }

	prebuildmessage "Generating Gen_ReflectionInit.h!"
	prebuildcommands
	{
		"py ../../../utils/gen_module_file.py ../../../src/Scripting"
	}

	includedirs
	{
		"include",
		"../Memory/include",
		"../Shared/include",
		"../Game/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/rapidjson",
	-- 	"../../dependencies/chaiscript",
	-- 	"../../dependencies/angelscript/angelscript/include",
		"../../frameworks/Gaff/include"
	}

	dependson
	{
		"Scripting", "Memory",
		"Gaff", "Shared",
		"EASTL", "angelscript",
		"Game"
	}

	links
	{
		"Scripting", "Memory",
		"Gaff", "Shared",
		"EASTL", "angelscript",
		"Game"
	}
