project "EditorMainLoop"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	defines { "SHIB_STATIC" }

	ModuleGen("EditorMainLoop")

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Engine/Engine/include",
		"../../Engine/Memory/include",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include",
		"../../Dependencies/EASTL/include",
		"../Graphics/include"
	}

project "EditorMainLoopModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_EditorMainLoopModule.cpp" }

	ModuleEditorCopy()

	flags { "FatalWarnings" }

	ModuleIncludesAndLinks("EditorMainLoop")

	local deps =
	{
		"Gleam"
	}

	dependson(deps)
	links(deps)
