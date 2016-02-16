group "Components Module"

project "Components"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/components")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"

	files { "**.h", "**.cpp", "**.inl" }
	excludes { "Shibboleth_ComponentsModule.cpp" }

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
		"../../dependencies/LuaJIT/src",
		"../../dependencies/utf8-cpp",
		"../../dependencies/assimp/include",
		"../../dependencies/esprit/include"
	}

	dofile("../../utils/os_conditionals.lua")

project "ComponentsModule"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/components")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	dofile("../../utils/module_suffix.lua")

	kind "SharedLib"
	language "C++"

	targetname "ShibbolethComponents"

	files { "Shibboleth_ComponentsModule.cpp" }

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
		"../../dependencies/LuaJIT/src",
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
		"Managers", "Memory",
		"LuaJIT"
	}

	links
	{
		"Shared", "Gaff", "jansson",
		"libjpeg", "libpng", "libtiff",
		"OtterUI", "ResIL", "ResILU",
		"zlib", "Gleam", "Game",
		"esprit", "Components",
		"Managers", "Memory",
		"BulletCollision", "BulletDynamics",
		"LinearMath"
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

	dofile("../../utils/os_conditionals.lua")
