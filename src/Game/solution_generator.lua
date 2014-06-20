solution "Game"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/game")
	end

	platforms { "x86", "x64" }
	configurations { "Debug", "Release" }
	warnings "Extra"
	flags { "Unicode" }

	nativewchar "Default"
	floatingpoint "Fast"
	debugdir "../../workingdir"

	filter { "platforms:x86" }
		architecture "x32"
		vectorextensions "SSE2"

	filter { "platforms:x64" }
		architecture "x64"

	filter { "configurations:Debug", "platforms:x86" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x86/Debug"

	filter { "configurations:Debug", "platforms:x64" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x64/Debug"

	filter { "configurations:Release", "platforms:x86" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x86/Release"

	filter { "configurations:Release", "platforms:x64" }
		objdir "../../build/intermediate"
		targetdir "../../build/output/x64/Release"

	filter { "language:C++", "action:gmake" }
		buildoptions { "-std=c++11", "-x c++" }

	filter { "configurations:Debug", "action:gmake", "options:not debug_optimization" }
		optimize "Off"

	filter { "configurations:Debug", "action:gmake", "options:debug_optimization" }
		optimize "Debug"

	filter { "configurations:Debug", "platforms:x86" }
		targetsuffix "32d"

	filter { "configurations:Release", "platforms:x86" }
		targetsuffix "32"

	filter { "configurations:Debug", "platforms:x64" }
		targetsuffix "64d"

	filter { "configurations:Release", "platforms:x64" }
		targetsuffix "64"

	filter {}

	configuration "vs*"
		buildoptions { "/sdl" }

	configuration "Debug"
		flags { "Symbols" }
		defines { "_DEBUG", "DEBUG" }

	configuration "Release"
		flags { "LinkTimeOptimization", "ReleaseRuntime" }
		optimize "Speed"

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

group "Dependencies"
	dofile("../../dependencies/jansson/include_external.lua")
	dofile("../../dependencies/libjpeg/include_external.lua")
	dofile("../../dependencies/libpng/include_external.lua")
	dofile("../../dependencies/libtiff/include_external.lua")
	dofile("../../dependencies/OtterUI/include_external.lua")
	dofile("../../dependencies/ResIL/include_external.lua")
	dofile("../../dependencies/zlib/include_external.lua")
	dofile("../../dependencies/Gleam/include_external.lua")
	dofile("../../dependencies/Gaff/include_external.lua")

group ""
	dofile("../Shared/include_external.lua")

	project "Game"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

		files
		{
			"include/Shibboleth_ComponentManager.h",
			"include/Shibboleth_GameObject.h",
			"include/Shibboleth_IComponent.h",
			"include/Shibboleth_IResourceLoader.h",
			"include/Shibboleth_OtterUI*.h",
			"include/Shibboleth_RenderManager.h",
			"include/Shibboleth_ResourceDefines.h",
			"include/Shibboleth_ResourceManager.h",
			"include/Shibboleth_ShaderLoader.h",
			"include/Shibboleth_ShaderProgramLoader.h",
			"include/Shibboleth_TextureLoader.h",

			"Shibboleth_ComponentManager.cpp",
			"Shibboleth_OtterUI*.cpp",
			"Shibboleth_RenderManager.cpp",
			"Shibboleth_ResourceManager.cpp",
			"Shibboleth_ShaderLoader.cpp",
			"Shibboleth_ShaderProgramLoader.cpp",
			"Shibboleth_TextureLoader.cpp"
		}

		includedirs
		{
			"include",
			"../Shared/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include"
		}

	project "ManagersDLL"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "SharedLib"
		language "C++"

		targetname "GameManagers"

		files { "Shibboleth_ManagersDLL.cpp" }

		includedirs
		{
			"include",
			"../Shared/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include"
		}

		dependson
		{
			"Shared", "Gaff", "jansson",
			"OtterUI", "Game", "Gleam"
		}

		links
		{
			"Shared", "Gaff", "jansson",
			"OtterUI", "Game", "Gleam"
		}

	project "StatesDLL"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "SharedLib"
		language "C++"

		targetname "GameStates"

		files
		{
			"include/Shibboleth_CreateResourceLoadersState.h",
			"include/Shibboleth_LoadComponentsState.h",
			"include/Shibboleth_SetupOtterUIState.h",
			"Shibboleth_CreateResourceLoadersState.cpp",
			"Shibboleth_LoadComponentsState.cpp",
			"Shibboleth_SetupOtterUIState.cpp",
			"Shibboleth_StatesDLL.cpp"
		}

		includedirs
		{
			"include",
			"../Shared/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include"
		}

		dependson
		{
			"Shared", "Gaff", "jansson",
			"libjpeg", "libpng", "libtiff",
			"OtterUI", "ResIL", "ResILU",
			"zlib", "Gleam", "Game"
		}

		links
		{
			"Shared", "Gaff", "jansson",
			"libjpeg", "libpng", "libtiff",
			"OtterUI", "ResIL", "ResILU",
			"zlib", "Gleam", "Game"
		}
