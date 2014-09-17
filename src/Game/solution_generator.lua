local manager_files =
{
	"Shibboleth_ManagersModule.cpp"
};

local state_files =
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
};

local components_files =
{
	"include/Shibboleth_LuaComponent.h",
	"include/Shibboleth_ModelComponent.h",
	"Shibboleth_LuaComponent.cpp",
	"Shibboleth_ComponentsModule.cpp",
	"Shibboleth_ModelComponent.cpp"
};


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

	filter { "options:simd" }
		defines { "USE_SIMD" }

	filter { "options:simd_set_aligned"}
		defines { "SIMD_SET_ALIGNED" }

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
	dofile("../../dependencies/assimp/include_external.lua")
	dofile("../../dependencies/minizip/include_external.lua")

group ""
	dofile("../Shared/include_external.lua")

	project "Game"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

		files { "**.h", "**.cpp", "**.inl", "solution_generator.lua" }

		excludes(manager_files)
		excludes(state_files)
		excludes(components_files)

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
			"../../dependencies/utf8-cpp",
			"../../dependencies/assimp/include"
		}

	project "ManagersModule"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "SharedLib"
		language "C++"

		targetname "GameManagers"

		files(manager_files)

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
			"../../dependencies/utf8-cpp"
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

		files(state_files)

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
			"../../dependencies/LuaJIT-2.0.3/src",
			"../../dependencies/utf8-cpp"
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
			"zlib", "Gleam", "Game",
			"assimp", "minizip"
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

		files(components_files)

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
			"../../dependencies/utf8-cpp",
			"../../dependencies/assimp/include"
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
