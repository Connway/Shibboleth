group "Managers Module"

project "Managers"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/managers")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	kind "StaticLib"
	language "C++"

	flags { "FatalWarnings" }

	files { "**.h", "**.cpp", "**.inl" }
	excludes { "Shibboleth_ManagersModule.cpp" }

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Game/include",
		"../Components/include",
		"../../dependencies/assimp/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/rapidjson",
		"../../dependencies/Gleam/include",
		"../../dependencies/Gaff/include",
		"../../dependencies/esprit/include",
		"../../dependencies/LuaState",
		"../../dependencies/LuaJIT/src",
		"../../dependencies/utf8-cpp",
		"../../dependencies/bullet"
	}

	dofile("../../utils/os_conditionals.lua")

project "ManagersModule"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/managers")
	end

	configurations { "Debug", "Release" }
	dofile("../../utils/config_map.lua")

	dofile("../../utils/module_suffix.lua")

	kind "SharedLib"
	language "C++"

	flags { "FatalWarnings" }

	targetname "ShibbolethManagers"

	files { "Shibboleth_ManagersModule.cpp" }

	configuration "windows"
		includedirs { "../../dependencies/dirent" }

	configuration {}

	includedirs
	{
		"include",
		"../Shared/include",
		"../Memory/include",
		"../Game/include",
		"../Components/include",
		"../../dependencies/OtterUI/inc",
		"../../dependencies/rapidjson",
		"../../dependencies/Gleam/include",
		"../../dependencies/Gaff/include",
		"../../dependencies/LuaState",
		"../../dependencies/LuaJIT/src",
		"../../dependencies/utf8-cpp",
		"../../dependencies/bullet"
	}

	dependson
	{
		"Shared", "Gaff",
		"OtterUI", "Game",
		 "Gleam", "Managers",
		 "Memory", "LuaJIT",
		 "Boxer", "Components",
		"BulletCollision",
		"BulletDynamics",
		"LinearMath"
	}

	links
	{
		"Shared", "Gaff",
		"OtterUI", "Game",
		 "Gleam", "Managers",
		 "Memory", "LuaJIT",
		 "Boxer", "Components",
		"BulletCollision",
		"BulletDynamics",
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
