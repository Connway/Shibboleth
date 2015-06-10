local manager_files =
{
	"include/Shibboleth_*Manager.h",
	"Shibboleth_*Manager.cpp",
	"include/Shibboleth_IRenderPipeline.h",
};

local manager_module_files =
{
	"Shibboleth_ManagersModule.cpp"
};

local state_files =
{
	"include/Shibboleth_*State.h",
	"Shibboleth_*State.cpp"
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

	configurations { "Debug", "Release" }
	dofile("../../solution_settings.lua")

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
			"../../dependencies/utf8-cpp",
			"../../dependencies/jansson"
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
			"Managers", "Memory", "Components"

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
			"Managers", "Memory", "Components"
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

		dofile("../../module_suffix.lua")

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

		dofile("../../module_suffix.lua")

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

		dofile("../../module_suffix.lua")
