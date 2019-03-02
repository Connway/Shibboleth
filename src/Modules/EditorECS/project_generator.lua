project "EditorECS"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_EditorECSModule.cpp" }

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Frameworks/Gaff/include",
		"../../Engine/Editor/include",
		"../../Engine/Engine/include",
		"../../Engine/Memory/include",
		"../ECS/include"
	}

	IncludeWxWidgets()



project "EditorECSModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_EditorECSModule.cpp" }

	ModuleGen("EditorECS")
	ModuleEditorCopy()

	flags { "FatalWarnings" }

	filter { "system:windows" }
		links
		{
			"comctl32",
			"Rpcrt4"
		}

	filter {}

	includedirs
	{
		"include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Frameworks/Gaff/include",
		"../../Engine/Editor/include",
		"../../Engine/Engine/include",
		"../../Engine/Memory/include",
		"../ECS/include"
	}

	IncludeWxWidgets()

	local deps =
	{
		"EditorECS",
		"Editor",
		"Engine",
		"Memory",
		"Gaff",
		"EASTL",
		"libpng",
		"zlib-ng",
		"mpack",
		"wxBase",
		"wxCore",
		"ECS"
	}

	dependson(deps)
	links(deps)

	-- NewDeleteLinkFix()
