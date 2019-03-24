project "EditorECS"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	defines { "SHIB_STATIC" }

	ModuleGen("EditorECS")

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

	ModuleEditorCopy()

	flags { "FatalWarnings" }

	filter { "system:windows" }
		links
		{
			"comctl32",
			"Rpcrt4"
		}

	filter {}

	ModuleIncludesAndLinks("EditorECS")

	local deps =
	{
		"Editor",
		"libpng",
		"zlib-ng",
		"wxBase",
		"wxCore",
		"ECS"
	}

	dependson(deps)
	links(deps)
