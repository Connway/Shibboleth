group "States Module"

project "States"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/states")
	end

	configurations { "Debug", "Release" }
	dofile("../../config_map.lua")

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	excludes { "Shibboleth_StatesModule.cpp" }

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Game/include",
		"../Managers/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/jansson",
		"../../dependencies/Gleam/include",
		"../../dependencies/Gaff/include",
		"../../dependencies/LuaState",
		"../../dependencies/LuaBridge",
		"../../dependencies/LuaJIT/src",
		"../../dependencies/utf8-cpp",
		"../../dependencies/assimp/include",
		"../../dependencies/esprit/include",
		"../../dependencies/minizip",
		"../../dependencies/zlib"
	}

	dofile("../../os_conditionals.lua")

project "StatesModule"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/states")
	end

	configurations { "Debug", "Release" }
	dofile("../../config_map.lua")

	dofile("../../module_suffix.lua")

	kind "SharedLib"
	language "C++"

	targetname "ShibbolethStates"

	files { "Shibboleth_StatesModule.cpp" }

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Game/include",
		"../Managers/include",
		"../Components/include",
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

	dofile("../../os_conditionals.lua")
