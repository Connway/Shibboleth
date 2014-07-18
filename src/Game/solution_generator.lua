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
	dofile("../../dependencies/LuaJIT-2.0.3/include_external.lua")

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
			"include/Shibboleth_Object.h",
			"include/Shibboleth_IComponent.h",
			"include/Shibboleth_IResourceLoader.h",
			"include/Shibboleth_OtterUI*.h",
			"include/Shibboleth_RenderManager.h",
			"include/Shibboleth_ResourceDefines.h",
			"include/Shibboleth_ResourceManager.h",
			"include/Shibboleth_ResourceWrapper.h",
			"include/Shibboleth_ShaderLoader.h",
			"include/Shibboleth_ShaderProgramLoader.h",
			"include/Shibboleth_TextureLoader.h",
			"include/Shibboleth_LuaManager.h",
			"include/Shibboleth_LuaLoader.h",
			"include/Shibboleth_UpdateManager.h",

			"Shibboleth_ComponentManager.cpp",
			"Shibboleth_Object.cpp",
			"Shibboleth_OtterUI*.cpp",
			"Shibboleth_RenderManager.cpp",
			"Shibboleth_ResourceManager.cpp",
			"Shibboleth_ShaderLoader.cpp",
			"Shibboleth_ShaderProgramLoader.cpp",
			"Shibboleth_TextureLoader.cpp",
			"Shibboleth_LuaManager.cpp",
			"Shibboleth_LuaLoader.cpp",
			"Shibboleth_UpdateManager.cpp",
		}

		includedirs
		{
			"include",
			"../Shared/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/LuaState",
			"../../dependencies/LuaJIT-2.0.3/src",
		}

	project "ManagersModule"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "SharedLib"
		language "C++"

		targetname "GameManagers"

		files { "Shibboleth_ManagersModule.cpp" }

		includedirs
		{
			"include",
			"../Shared/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/LuaState",
			"../../dependencies/LuaJIT-2.0.3/src",
		}

		dependson
		{
			"Shared", "Gaff", "jansson",
			"OtterUI", "Game", "Gleam"

			--[[
				Adding LuaJIT as a dependency for some reason makes VS2013
				not link in referenced project outputs. This means users
				will have to make sure that they run the LuaJIT project
				before the ComponentsModule project has had a chance to
				link.
			--]]
			-- "LuaJIT"
		}

		links
		{
			"Shared", "Gaff", "jansson",
			"OtterUI", "Game", "Gleam"
		}

		filter { "configurations:Debug", "platforms:x86" }
			links { "../../build/output/x86/Debug/lua51" }

		filter { "configurations:Debug", "platforms:x64" }
			links { "../../build/output/x64/Debug/lua51" }

		filter { "configurations:Release", "platforms:x86" }
			links { "../../build/output/x86/Release/lua51" }

		filter { "configurations:Release", "platforms:x64" }
			links { "../../build/output/x64/Release/lua51" }

		filter {}

	project "StatesModule"
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
			"include/Shibboleth_RegisterLuaClassesState.h",
			"Shibboleth_CreateResourceLoadersState.cpp",
			"Shibboleth_LoadComponentsState.cpp",
			"Shibboleth_RegisterLuaClassesState.cpp",
			"Shibboleth_SetupOtterUIState.cpp",
			"Shibboleth_StatesModule.cpp"
		}

		includedirs
		{
			"include",
			"../Shared/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/LuaBridge",
			"../../dependencies/LuaState",
			"../../dependencies/LuaJIT-2.0.3/src"
		}

		dependson
		{
			"Shared", "Gaff", "jansson",
			"libjpeg", "libpng", "libtiff",
			"OtterUI", "ResIL", "ResILU",
			"zlib", "Gleam", "Game"

			--[[
				Adding LuaJIT as a dependency for some reason makes VS2013
				not link in referenced project outputs. This means users
				will have to make sure that they run the LuaJIT project
				before the ComponentsModule project has had a chance to
				link.
			--]]
			-- "LuaJIT"
		}

		links
		{
			"Shared", "Gaff", "jansson",
			"libjpeg", "libpng", "libtiff",
			"OtterUI", "ResIL", "ResILU",
			"zlib", "Gleam", "Game"
		}

		filter { "configurations:Debug", "platforms:x86" }
			links { "../../build/output/x86/Debug/lua51" }

		filter { "configurations:Debug", "platforms:x64" }
			links { "../../build/output/x64/Debug/lua51" }

		filter { "configurations:Release", "platforms:x86" }
			links { "../../build/output/x86/Release/lua51" }

		filter { "configurations:Release", "platforms:x64" }
			links { "../../build/output/x64/Release/lua51" }

		filter {}

	project "ComponentsModule"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "SharedLib"
		language "C++"

		targetname "GameComponents"

		files
		{
			"include/Shibboleth_LuaComponent.h",
			"Shibboleth_LuaComponent.cpp",
			"Shibboleth_ComponentsModule.cpp"
		}

		includedirs
		{
			"include",
			"../Shared/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/LuaState",
			"../../dependencies/LuaJIT-2.0.3/src"
		}

		dependson
		{
			"Shared", "Gaff", "jansson",
			"libjpeg", "libpng", "libtiff",
			"OtterUI", "ResIL", "ResILU",
			"zlib", "Gleam", "Game"

			--[[
				Adding LuaJIT as a dependency for some reason makes VS2013
				not link in referenced project outputs. This means users
				will have to make sure that they run the LuaJIT project
				before the ComponentsModule project has had a chance to
				link.
			--]]
			-- "LuaJIT"
		}

		links
		{
			"Shared", "Gaff", "jansson",
			"libjpeg", "libpng", "libtiff",
			"OtterUI", "ResIL", "ResILU",
			"zlib", "Gleam", "Game"
		}

		filter { "configurations:Debug", "platforms:x86" }
			links { "../../build/output/x86/Debug/lua51" }

		filter { "configurations:Debug", "platforms:x64" }
			links { "../../build/output/x64/Debug/lua51" }

		filter { "configurations:Release", "platforms:x86" }
			links { "../../build/output/x86/Release/lua51" }

		filter { "configurations:Release", "platforms:x64" }
			links { "../../build/output/x64/Release/lua51" }

		-- Don't know if this works, but trying it out
		filter { "action:gmake" }
			dependson { "LuaJIT" }

		filter {}
