project "EditorMainLoop"
	if _ACTION then
		location(GetModulesLocation())
	end

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_EditorMainLoopModule.cpp" }

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

	flags { "FatalWarnings" }

	-- filter { "system:windows" }
	-- 	links { "ws2_32.lib", "iphlpapi.lib", "psapi.lib", "userenv.lib" }
	-- 	includedirs { "../../Dependencies/dirent" }

	-- filter {}

	includedirs
	{
		"include",
		"../../Engine/Engine/include",
		"../../Engine/Memory/include",
		"../../Frameworks/Gaff/include",
		"../../Frameworks/Gleam/include",
		"../../Dependencies/EASTL/include"
	}

	local deps = {
		"EditorMainLoop",
		"Engine",
		"Gaff",
		"Gleam",
		"Memory",
		"EASTL",
		"mpack"
	}

	dependson(deps)
	links(deps)

	ModuleGen("EditorMainLoop")
	ModuleEditorCopy()
