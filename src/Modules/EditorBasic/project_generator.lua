project "EditorBasic"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_EditorBasicModule.cpp" }

	filter { "configurations:not Analyze*" }
		flags { "FatalWarnings" }

	filter {}

	includedirs
	{
		"include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include",
		"../../Engine/Editor/include",
		"../../Engine/Shared/include",
		"../../Engine/Memory/include",
		"../Graphics/include"
	}

	IncludeWxWidgets()



project "EditorBasicModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_EditorBasicModule.cpp" }

	ModuleGen("EditorBasic")
	ModuleEditorCopy()

	filter { "configurations:not Analyze*" }
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
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include",
		"../../Engine/Editor/include",
		"../../Engine/Shared/include",
		"../../Engine/Memory/include"
	}

	IncludeWxWidgets()

	local deps =
	{
		"EditorBasic",
		"Editor",
		"Shared",
		"Memory",
		"Gaff",
		"Gleam",
		"EASTL",
		"libpng",
		"zlib-ng",
		"wxBase",
		"wxCore"
	}

	dependson(deps)
	links(deps)

	-- NewDeleteLinkFix()