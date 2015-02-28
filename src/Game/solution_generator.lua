local manager_files =
{
	"include/Shibboleth_*Manager.h",
	"Shibboleth_*Manager.cpp",
};

local manager_module_files =
{
	"Shibboleth_ManagersModule.cpp"
};

local state_files =
{
	"include/Shibboleth_*State.h",
	"Shibboleth_*State.cpp",
};

local state_module_files =
{
	"Shibboleth_StatesModule.cpp"
};

local component_files =
{
	"include/Shibboleth_*Component.h",
	"Shibboleth_*Component.cpp"
};

local component_module_files =
{
	"Shibboleth_ComponentsModule.cpp"
};

local message_files =
{
	"include/Shibboleth_*Message.h",
	"Shibboleth_*Message.cpp"
};



solution "Game"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/game")
	end

	platforms { "x86", "x64" }
	configurations { "Debug", "Release" }
	warnings "Extra"
	flags { "Unicode", "NoRTTI", "SEH", "Symbols" } -- unfortunately need exceptions for crash handlers :(

	nativewchar "Default"
	floatingpoint "Fast"
	debugdir "../../workingdir"

	filter { "options:symbols" }
		flags { "Symbols" }
		-- defines { "SYMBOL_BUILD" }

	-- filter { "system:windows", "options:symbols" }
	-- 	links { "Dbghelp" }

	filter { "options:ltcg" }
		flags { "LinkTimeOptimization", "NoIncrementalLink" }

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
	dofile("../../dependencies/esprit/include_external.lua")

group ""
	dofile("../Shared/include_external.lua")
	dofile("../Memory/include_external.lua")

	project "Game"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

		files { "**.h", "**.cpp", "**.inl", "solution_generator.lua" }

		excludes(manager_files)
		excludes(manager_module_files)
		excludes(state_files)
		excludes(state_module_files)
		excludes(component_files)
		excludes(component_module_files)

		includedirs
		{
			"include",
			"../Shared/include",
			"../Memory/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/LuaState",
			"../../dependencies/LuaJIT-2.0.3/src",
			"../../dependencies/utf8-cpp",
			"../../dependencies/assimp/include",
			"../../dependencies/esprit/include",
			"../../dependencies/minizip",
			"../../dependencies/zlib"
		}

		filter { "configurations:Debug or Release", "platforms:x86 or x64" }
			targetsuffix ""

		filter {}

	project "Components"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

		files(component_files)

		includedirs
		{
			"include",
			"../Shared/include",
			"../Memory/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/LuaState",
			"../../dependencies/LuaJIT-2.0.3/src",
			"../../dependencies/utf8-cpp",
			"../../dependencies/assimp/include",
			"../../dependencies/esprit/include"
		}

		filter { "configurations:Debug or Release", "platforms:x86 or x64" }
			targetsuffix ""

		filter {}

	project "Managers"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

		files(manager_files)

		includedirs
		{
			"include",
			"../Shared/include",
			"../Memory/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/LuaState",
			"../../dependencies/LuaJIT-2.0.3/src",
			"../../dependencies/utf8-cpp"
		}

		filter { "configurations:Debug or Release", "platforms:x86 or x64" }
			targetsuffix ""

		filter {}

	project "Messages"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

		files(message_files)

		includedirs
		{
			"include",
			"../Shared/include",
			"../Memory/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/utf8-cpp"
		}

		filter { "configurations:Debug or Release", "platforms:x86 or x64" }
			targetsuffix ""

		filter {}

	project "States"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "StaticLib"
		language "C++"

		files(state_files)

		includedirs
		{
			"include",
			"../Shared/include",
			"../Memory/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/LuaBridge",
			"../../dependencies/LuaState",
			"../../dependencies/LuaJIT-2.0.3/src",
			"../../dependencies/utf8-cpp"
		}

		filter { "configurations:Debug or Release", "platforms:x86 or x64" }
			targetsuffix ""

		filter {}

	project "ManagersModule"
		if _ACTION then
			location ("../../project/" .. _ACTION .. "/game")
		end

		kind "SharedLib"
		language "C++"

		targetname "GameManagers"

		files(manager_module_files)

		includedirs
		{
			"include",
			"../Shared/include",
			"../Memory/include",
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
			"OtterUI", "Game", "Gleam",
			"Managers", "Memory"

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
			"OtterUI", "Game", "Gleam",
			"Managers", "Memory"
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

		files(state_module_files)

		includedirs
		{
			"include",
			"../Shared/include",
			"../Memory/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/Gleam/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/utf8-cpp",
			"../../dependencies/jansson"
		}

		dependson
		{
			"Shared", "Gaff", "jansson",
			"libjpeg", "libpng", "libtiff",
			"OtterUI", "ResIL", "ResILU",
			"zlib", "Gleam", "Game",
			"esprit", "Managers",
			"Components", "States",
			"Memory"

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
			"assimp", "minizip",
			"esprit", "Managers",
			"Components", "States",
			"Memory"
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

		files(component_module_files)

		includedirs
		{
			"include",
			"../Shared/include",
			"../Memory/include",
			"../../dependencies/OtterUI/inc",
			"../../dependencies/jansson",
			"../../dependencies/Gleam/include",
			"../../dependencies/Gaff/include",
			"../../dependencies/LuaState",
			"../../dependencies/LuaJIT-2.0.3/src",
			"../../dependencies/utf8-cpp",
			"../../dependencies/assimp/include",
			"../../dependencies/esprit/include"
		}

		dependson
		{
			"Shared", "Gaff", "jansson",
			"libjpeg", "libpng", "libtiff",
			"OtterUI", "ResIL", "ResILU",
			"zlib", "Gleam", "Game",
			"esprit", "Components",
			"Managers", "Memory"

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
			"esprit", "Components",
			"Managers", "Memory"
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
