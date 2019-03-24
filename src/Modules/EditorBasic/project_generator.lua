project "EditorBasic"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	defines { "SHIB_STATIC" }

	ModuleGen("EditorBasic")

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Dependencies/EASTL/include",
		"../../Dependencies/rapidjson",
		"../../Dependencies/mpack",
		"../../Dependencies/glm",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include",
		"../../Engine/Editor/include",
		"../../Engine/Engine/include",
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
		"../../Frameworks/Gaff/include"
	}

	ModuleIncludesAndLinks("EditorBasic")
	NewDeleteLinkFix()

	local deps =
	{
		"Editor",
		"Gleam",
		"libpng",
		"zlib-ng",
		"wxBase",
		"wxCore",
		"wxPropGrid"
	}

	dependson(deps)
	links(deps)
