project "Scene"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	defines { "SHIB_STATIC" }

	ModuleGen("Scene")

	flags { "FatalWarnings" }

	includedirs
	{
		"include",
		"../../Engine/Memory/include",
		"../../Engine/Engine/include",
		"../../Dependencies/EASTL/include",
		-- "../../Dependencies/rapidjson",
		-- "../../Dependencies/glm",
		-- "../../Dependencies/mpack",
		"../../Frameworks/Gaff/include",
		-- "../../Frameworks/Gleam/include"
	}

project "SceneModule"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "SharedLib"
	language "C++"

	files { "Shibboleth_SceneModule.cpp" }

	ModuleCopy()

	flags { "FatalWarnings" }

	ModuleIncludesAndLinks("Scene")
	-- NewDeleteLinkFix()
