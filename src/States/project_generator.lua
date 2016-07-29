group "States Module"

project "States"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/states")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

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
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include",
		"../../frameworks/esprit/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/rapidjson",
		"../../dependencies/LuaState",
		"../../dependencies/LuaBridge",
		"../../dependencies/LuaJIT/src",
		"../../dependencies/utf8-cpp",
		"../../dependencies/assimp/include",
		"../../dependencies/minizip",
		"../../dependencies/zlib"
	}

	dofile("../../utils/os_conditionals.lua")

project "StatesModule"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/states")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	dofile("../../utils/module_suffix.lua")

	kind "SharedLib"
	language "C++"

	flags { "FatalWarnings" }

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
		"../../frameworks/Gaff/include",
		"../../frameworks/Gleam/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/utf8-cpp",
		"../../dependencies/rapidjson"
	}

	dependson
	{
		"Shared", "Gaff",
		"libjpeg", "libpng",
		"libtiff", "OtterUI",
		"ResIL", "ResILU",
		"zlib", "Gleam",
		"Game", "esprit",
		"Components",
		"States", "Memory",
		"LuaJIT", "Boxer",
		"BulletCollision",
		"BulletDynamics",
		"LinearMath",
		"assimp", "minizip"
	}

	links
	{
		"Shared", "Gaff",
		"libjpeg", "libpng",
		"libtiff", "OtterUI",
		"ResIL", "ResILU",
		"zlib", "Gleam",
		"Game", "esprit",
		"Components",
		"States", "Memory",
		"LuaJIT", "Boxer",
		"BulletCollision",
		"BulletDynamics",
		"LinearMath",
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

	dofile("../../utils/os_conditionals.lua")
